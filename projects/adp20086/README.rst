ADP20086 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Parts
---------------

* `ADP20086 <https://www.analog.com/en/products/adp20086.html>`_

Supported Evaluation Boards
---------------------------

* `ADP20086-EVALZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adp20086.html>`_

Hardware Connections
--------------------

The pin assignments below are the defaults in
``src/platform/maxim/parameters.h`` for the MAX32655FTHR; adjust them there if
the evaluation board is wired differently.

+-----------+---------------------+------------------------------------------+
| Signal    | MAX32655FTHR pin    | Notes                                    |
+===========+=====================+==========================================+
| SDA / SCL | I2C2                | Requires bus pull-ups.                   |
+-----------+---------------------+------------------------------------------+
| EN        | P1_7                | Optional. Needs J3 in the GPIO_EN        |
|           |                     | position (1-3).                          |
+-----------+---------------------+------------------------------------------+
| INTB      | P1_9                | Optional. Open drain and active low;     |
|           |                     | needs an external pull-up of at least    |
|           |                     | 2 kΩ to the I/O supply.                  |
+-----------+---------------------+------------------------------------------+

Both EN and INTB are optional. To build for a board where one of them is not
connected, set the corresponding ``en_gpio_ip`` / ``intb_gpio_ip`` field to
``NULL`` in ``src/common/common_data.c``; the driver and the IIO attribute list
adapt to whichever pins are present.

INTB must share a GPIO port with no other interrupt source used by the
application, because the Maxim GPIO interrupt controller is identified by its
port (``GPIO_IRQ_ID``). Note also that the examples enable the port interrupt in
the NVIC themselves, in ``src/platform/maxim/main.c`` — the Maxim GPIO IRQ
driver does not do this.

Power-Up Sequencing
-------------------

Supply VIN before VCC. The device runs its built-in self-test once, as VCC
rises past its UVLO threshold; if VIN is absent at that moment the self-test
fails and latches ``STAT5.BIST``, which makes ``adp20086_init()`` return
``-ENODEV`` on every subsequent attempt. The bit only clears on the next clean
VCC rise, so recover by power-cycling the board with VIN already applied.
