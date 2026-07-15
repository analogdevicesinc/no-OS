*************
IIO Framework
*************

Most no-OS example projects expose their device through an **IIO** (Industrial
I/O) interface, and the :doc:`getting_started` tutorial drives one end to end.
This page explains what that interface is, the object model behind it, and how a
driver plugs into it — so that when you read an ``iio_*`` driver or add IIO
support to your own, the pieces make sense.

If you have not yet built and run an IIO application, do :doc:`getting_started`
first.

.. contents:: Table of Contents
    :depth: 2

What IIO is, and why no-OS uses it
==================================

IIO is the same device model the Linux kernel uses for ADCs, DACs, sensors, and
other data-converter-style hardware. no-OS implements the device side of that
model in firmware: a small server (**IIOD**) runs on the target and speaks the
IIO protocol over a transport (UART, USB CDC-ACM, or network). Any standard IIO
**client** on a host can then discover and drive the device without knowing
anything device-specific:

* the `libiio <https://github.com/analogdevicesinc/libiio>`_ command-line tools —
  ``iio_info`` and ``iio_attr`` to enumerate the device and read/write
  attributes, ``iio_readdev`` / ``iio_writedev`` to stream buffered samples in
  and out, and ``iio_reg`` for register-level debug access,
* the `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_
  or `Scopy <https://github.com/analogdevicesinc/scopy>`_ GUIs,
* or any application linked against libiio.

The payoff is uniformity: every no-OS device that ships an IIO driver is
inspected, configured, and streamed the same way, with tools that already
exist. You get attribute discovery, buffered acquisition, and triggering "for
free" once your driver fills in the model below.

The object model
================

An IIO device is described by a handful of nested structures, all defined in
``iio/iio_types.h``. From the top down:

.. code-block::

   iio_device
   ├── channels[]        (iio_channel)   the data streams: voltage0, temp, ...
   │   └── attributes[]  (iio_attribute) per-channel: scale, offset, ...
   ├── attributes[]      (iio_attribute) device-global: sampling_frequency, ...
   ├── debug_attributes[]                register-level debug access
   ├── buffer_attributes[]               buffer configuration
   └── callbacks                         read/write, buffer, trigger hooks

Device
------

``struct iio_device`` is the root descriptor. It lists the device's channels
and attribute arrays, and provides the function pointers that IIOD calls to do
work — attribute access, buffer lifecycle, and (optionally) register debug and
trigger handling. A real example, from ``drivers/adc/adc_demo/iio_adc_demo.c``:

.. code-block:: c

   struct iio_device adc_demo_iio_descriptor = {
           .num_ch          = TOTAL_ADC_CHANNELS,
           .channels        = iio_adc_channels,
           .attributes      = iio_adc_global_attributes,
           .debug_attributes = NULL,
           .buffer_attributes = NULL,
           .pre_enable      = update_adc_channels,     /* before a capture   */
           .post_disable    = close_adc_channels,      /* after a capture    */
           .trigger_handler = adc_demo_trigger_handler,/* on a trigger event */
           .submit          = adc_submit_samples,      /* move buffered data */
           .debug_reg_read  = iio_adc_demo_reg_read,
           .debug_reg_write = iio_adc_demo_reg_write,
   };

Channel
-------

``struct iio_channel`` describes one data stream the device presents — a single
ADC input, a DAC output, a temperature reading. Key fields:

* ``ch_type`` — the kind of quantity (``IIO_VOLTAGE``, ``IIO_TEMP``,
  ``IIO_ACCEL``, ...); it determines the client-visible name prefix.
* ``channel`` / ``indexed`` — the numeric index (``voltage0``, ``voltage1``).
* ``ch_out`` — input vs output (an ADC channel vs a DAC channel).
* ``scan_index`` / ``scan_type`` — the channel's position and sample format
  (bits, storage size, sign, endianness) within a buffered capture.
* ``attributes`` — the per-channel attribute array.

Attribute
---------

``struct iio_attribute`` is a named, readable/writable property exposed to the
client, implemented by a pair of callbacks:

.. code-block:: c

   struct iio_attribute {
           const char *name;
           /* ... */
           int (*show)(void *device, char *buf, uint32_t len,
                       const struct iio_ch_info *channel, intptr_t priv);
           int (*store)(void *device, char *buf, uint32_t len,
                        const struct iio_ch_info *channel, intptr_t priv);
   };

``show`` renders the current value into ``buf`` when the client reads the
attribute; ``store`` parses ``buf`` and applies it when the client writes.
Attributes are what surface things like ``sampling_frequency``, ``scale``, and
``offset``. Every attribute array is **NULL-terminated** with the
``END_ATTRIBUTES_ARRAY`` marker (``{.name = NULL}``) so IIOD knows where it
ends.

Buffers
-------

Buffered acquisition streams many samples at once instead of reading one
attribute at a time. When a client starts a capture, IIOD calls the device's
``pre_enable`` hook, then repeatedly invokes ``submit`` to move sample data
between the device and the IIO buffer, and finally ``post_disable`` when the
capture stops. The application supplies the backing memory through a
``struct iio_data_buffer`` (a ``size`` and a ``buff`` pointer) when it registers
the device.

Triggers
--------

A trigger paces buffered acquisition. no-OS supports both **hardware triggers**
(a timer or GPIO interrupt, via ``iio/iio_trigger.*``) and **software
triggers**. When a trigger fires, IIOD invokes the device's
``trigger_handler``, which performs one round of sampling. The IIO Demo's
``iio_timer_trigger`` and ``iio_wifi_with_sw_trig`` variants exercise these two
paths; see :doc:`getting_started` for how to build them.

How it runs: the ``iio_app`` helper
===================================

Most projects do not talk to IIOD directly. They use the thin ``iio_app`` layer
(``iio/iio_app/iio_app.h``), which wires the IIO devices to a transport and runs
the server loop. The application:

1. Builds an array of ``struct iio_app_device``, each binding a name, the device
   instance, its ``iio_device`` descriptor, and optional read/write buffers:

   .. code-block:: c

      struct iio_app_device iio_devices[] = {
              {
                      .name           = "adc_demo",
                      .dev            = adc_demo_desc,
                      .dev_descriptor = &adc_demo_iio_descriptor,
                      .read_buff      = &adc_buff,
              },
      };

2. Fills a ``struct iio_app_init_param`` with that array (plus the UART/transport
   configuration) and calls ``iio_app_init()``.

3. Calls ``iio_app_run()``, which starts IIOD and services client requests until
   the program ends.

That is the entire application-side contract: describe your devices, hand them
to ``iio_app``, and run. The transport (plain UART, USB CDC-ACM, or Wi-Fi) is
chosen by the project variant, which is why the same device is reachable over a
serial cable in the ``iio`` variant and over the network in the ``iio_wifi``
variant without any change to the device model.

Software architecture
======================

The runtime is a small stack of modules. The application drives ``iio_app``,
which owns the ``iio`` core; the ``iio`` core answers requests decoded by
``iiod`` (the IIO-protocol engine), and the whole thing talks to a host
**client** over the chosen transport:

.. code-block::

   ┌────────────────────────────────────────────────┐
   │ Application                                    │
   │ builds iio_app_device[], then calls            │
   │ iio_app_init() and iio_app_run()               │
   └───────────────────────┬────────────────────────┘
                           │ iio_app_init() / iio_app_run()
                           ▼
   ┌────────────────────────────────────────────────┐
   │ iio_app                                        │
   │ iio_app_init(): creates and inits an iio_desc, │
   │ registers the devices, opens the transport;    │
   │ iio_app_run(): calls iio_step() in a loop      │
   └───────────────────────┬────────────────────────┘
                           │ iio_step()
                 ┌─────────┴───────────────────────┐
                 ▼                                 ▼
   ┌───────────────────────────┐   ┌────────────────────────────────┐
   │ iio                       │   │ iiod                           │
   │ • holds iio_desc          ◄──►│ • parses the IIO protocol      │
   │ • walks iio_device[]      │   │ • dispatches each request to   │
   │ • read/write attributes   │   │   the matching iio operation   │
   │ • moves buffered samples  │   │   (attr I/O, buffer, trigger)  │
   │ • generates the XML       │   └───────────────┬────────────────┘
   └───────────────────────────┘
                                                   │
                                                   ▼
                                   ┌────────────────────────────────┐
                                   │ transport                      │
                                   │ UART · USB CDC-ACM · TCP/IP    │
                                   └───────────────┬────────────────┘
                                                   │
                                                   ▼
                                   ┌────────────────────────────────┐
                                   │ Client (host)                  │
                                   │ iio_info, iio_readdev,         │
                                   │ iio_writedev, IIO Oscilloscope,│
                                   │ Scopy, pyadi-iio, …            │
                                   └────────────────────────────────┘

* **iio_app** — the application interface (``iio/iio_app/iio_app.c``).
  ``iio_app_init()`` builds the ``iio_desc``, registers the devices, and opens
  the communication transport; ``iio_app_run()`` then drives the main loop, where
  requests are received, parsed, and executed. Every IIO project uses it.
* **iio** — the core (``iio/iio.c``). It holds the ``iio_desc``, walks the
  registered ``iio_device`` descriptors to read/write attributes and move
  buffered data, and generates the XML that describes the devices to the client.
* **iiod** — the IIO-protocol engine (``iio/iiod.c``, formerly the external
  ``libtinyiiod``). It parses the wire protocol and calls the matching ``iio``
  operation.
* **transport** — the byte pipe between target and host. Which one is used is a
  project-variant choice, not a code change: the ``iio`` variant runs over
  UART/USB CDC-ACM, the ``iio_wifi`` variant over TCP/IP.

Request lifecycle
-----------------

Once ``iio_app_run()`` is looping, a single client request flows through the
stack like this:

#. The client (say ``iio_readdev``) sends a protocol command over the transport.
#. ``iio_step()`` hands the received bytes to **iiod**, which parses the command
   and identifies the target device, channel, and operation.
#. **iiod** calls the corresponding **iio** operation — an attribute
   ``show``/``store``, or a buffer ``pre_enable`` → ``submit`` → ``post_disable``
   sequence for a capture (see `Buffers`_ and `Triggers`_ above).
#. **iio** invokes the device's callbacks from its ``iio_device`` descriptor,
   which read or write the actual hardware through the core driver.
#. The result (attribute value, sample buffer, or the device XML) travels back
   out through **iiod** and the transport to the client.

Attribute discovery works the same way: on connect the client asks for the
context description, and **iio** generates the XML from the registered
``iio_device`` descriptors — which is how a generic tool like ``iio_info`` or
IIO Oscilloscope can present your device without knowing anything about it in
advance.

Connecting a client
-------------------

Once a project is built and running, connect any libiio client over the
transport the project exposes. To enumerate a device and its attributes:

.. code-block:: bash

   # serial (USB CDC-ACM / UART)
   iio_info -u serial:/dev/ttyACM0,57600,8n1n

   # network (Wi-Fi / Ethernet variants, Linux host)
   iio_info -u ip:<host>

To stream buffered samples — for example, loading a waveform into the IIO Demo's
``dac_demo`` so it shows up in a GUI client:

.. code-block:: bash

   cat projects/iio_demo/sample_sine.dat | iio_writedev -u serial:/dev/ttyACM0,57600,8n1n -s 100 dac_demo

A Python abstraction, `pyadi-iio <https://analogdevicesinc.github.io/pyadi-iio/>`_,
wraps many ADI devices on top of libiio for scripting and prototyping.

Anatomy of an IIO driver
========================

A no-OS device driver and its IIO layer are kept separate, by convention in two
files:

.. code-block::

   drivers/<class>/<part>/
   ├── <part>.c / <part>.h        the core driver (register access, no IIO)
   └── iio_<part>.c / iio_<part>.h the IIO wrapper: channels, attributes,
                                   descriptor, and callbacks

The core driver knows nothing about IIO — it is the same
init_param/descriptor-style driver described in :doc:`drivers_guide`. The
``iio_<part>`` layer builds the object model on top of it: it declares the
channel array, the attribute arrays with their ``show``/``store`` callbacks that
call into the core driver, and the ``iio_device`` descriptor that ties them
together (like ``adc_demo_iio_descriptor`` above). This separation means the
core driver stays usable in a non-IIO application, and the IIO model is added
only when a project wants it.

Where to go next
================

* :doc:`getting_started` — build and run the IIO Demo, then stream data with
  ``iio_readdev`` / ``iio_writedev``.
* :doc:`architecture` — how the IIO framework sits on top of the ``no_os_*``
  HAL and the rest of the stack.
* :doc:`drivers_guide` — writing the core driver that an ``iio_<part>`` layer
  wraps.
* The ``adc_demo`` / ``dac_demo`` drivers
  (``drivers/adc/adc_demo``, ``drivers/dac/dac_demo``) — the smallest complete
  IIO drivers to read as a template.
* `IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_,
  `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_,
  and `Scopy <https://github.com/analogdevicesinc/scopy>`_
  — the client-side documentation.
