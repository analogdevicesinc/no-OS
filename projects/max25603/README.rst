MAX25603 no-OS Example Project
==============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`MAX25603 Evaluation Kit <max25603evkit>`

Overview
--------

The MAX25603 Evaluation Kit is designed for evaluating the MAX25603, a
high-voltage, high-brightness LED buck-boost controller used in
automotive applications. It operates over a 5V to 60V input range and
delivers up to 1A to LED strings. The kit supports both analog and
digital dimming, incorporates input current limiting, and provides LED
protection mechanisms. Ideal for automotive headlamp applications, it
can configure multiple lighting functions such as clearance, low beam,
and high beam within a single LED string. The design includes shunting
FETs for segment control and integrates a proven PCB layout for
performance reliability.

Applications
------------

- Combination Automotive Head Lamps

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The MAX25603 Evaluation Kit necessitates a power supply delivering an
input voltage between 5V and 60V. It is engineered to supply up to 1A
to LED strings, supporting Voltages from 0V to 60V per string. This
configuration is suitable for automotive LED applications, including
multifunctional headlamps that incorporate clearance, low beam, and high
beam operations within a single LED string. Additionally, the kit
features both analog and digital dimming controls, input current
limiting, and protective mechanisms for LEDs, ensuring efficient and
stable performance across various automotive lighting applications.

Connections
~~~~~~~~~~~

The MAX25603 Evaluation Kit supports various automotive LED functions
through specific connectors. The table below outlines these connections
and their functions.

+-----------------------+-----------------------+-----------------------+
| Jumper                | Shunt Position        | Feature               |
+-----------------------+-----------------------+-----------------------+
| J1                    | Open                  | Apply an external DC  |
|                       |                       | voltage between 0.2V  |
|                       |                       | to 1.2V for setting   |
|                       |                       | the LED current.      |
+-----------------------+-----------------------+-----------------------+
| J1                    | Closed                | Uses the resistive    |
|                       |                       | divider from VCC and  |
|                       |                       | the potentiometer R3  |
|                       |                       | to set the LED        |
|                       |                       | current.              |
+-----------------------+-----------------------+-----------------------+
| J2                    | Open                  | Uses the resistive    |
|                       |                       | divider from VCC and  |
|                       |                       | the potentiometer R3  |
|                       |                       | to set the LED        |
|                       |                       | current.              |
+-----------------------+-----------------------+-----------------------+
| J2                    | 1-2                   | R4 is placed in       |
|                       |                       | series with R3 to     |
|                       |                       | lower the LED current |
|                       |                       | when EN2 is high.     |
+-----------------------+-----------------------+-----------------------+
| J2                    | 2-3                   | R4 is placed in       |
|                       |                       | series with R3 to     |
|                       |                       | lower the LED current |
|                       |                       | when EN1 is high.     |
+-----------------------+-----------------------+-----------------------+
| J3                    | Open                  | Apply an external 0   |
|                       |                       | to 5V signal on the   |
|                       |                       | SHUNT pin.            |
+-----------------------+-----------------------+-----------------------+
| J3                    | 1-2                   | SHUNT pin is tied to  |
|                       |                       | VCC. The error        |
|                       |                       | amplifier will always |
|                       |                       | be connected to       |
|                       |                       | COMP2.                |
+-----------------------+-----------------------+-----------------------+
| J3                    | 2-3                   | SHUNT pin is tied to  |
|                       |                       | GND. The error        |
|                       |                       | amplifier will always |
|                       |                       | be connected to       |
|                       |                       | COMP1.                |
+-----------------------+-----------------------+-----------------------+
| J4                    | Open                  | Apply an external 0   |
|                       |                       | to 5V PWM clock       |
|                       |                       | source for PWM        |
|                       |                       | dimming on Q6.        |
+-----------------------+-----------------------+-----------------------+
| J4                    | 1-2                   | EN2 pin pulled to VCC |
|                       |                       | for 100% duty on Q6.  |
+-----------------------+-----------------------+-----------------------+
| J4                    | 2-3                   | EN2 pin pulled to GND |
|                       |                       | to turn off Q6.       |
+-----------------------+-----------------------+-----------------------+
| J5                    | Open                  | Apply an external 0   |
|                       |                       | to 5V PWM clock       |
|                       |                       | source for PWM        |
|                       |                       | dimming on Q5.        |
+-----------------------+-----------------------+-----------------------+
| J5                    | 1-2                   | EN1 pin pulled to VCC |
|                       |                       | for 100% duty on Q5.  |
+-----------------------+-----------------------+-----------------------+
| J5                    | 2-3                   | EN1 pin pulled to GND |
|                       |                       | to turn off Q5.       |
+-----------------------+-----------------------+-----------------------+
| J6                    | Open                  | Disconnects the       |
|                       |                       | source of shunting    |
|                       |                       | FET Q7 from LED2+.    |
+-----------------------+-----------------------+-----------------------+
| J6                    | Closed                | Connects the source   |
|                       |                       | of shunting FET Q7 to |
|                       |                       | LED2+.                |
+-----------------------+-----------------------+-----------------------+
| J7                    | Open                  | Disconnects the drain |
|                       |                       | of shunting FET Q7    |
|                       |                       | from LB+.             |
+-----------------------+-----------------------+-----------------------+
| J7                    | Closed                | Connects the drain of |
|                       |                       | shunting FET Q7 to    |
|                       |                       | LB+.                  |
+-----------------------+-----------------------+-----------------------+
| J8                    | Open                  | Overvoltage           |
|                       |                       | protection threshold  |
|                       |                       | is set by the         |
|                       |                       | resistive divider     |
|                       |                       | from VOUTCAP.         |
+-----------------------+-----------------------+-----------------------+
| J8                    | 1-2                   | R49 is placed in      |
|                       |                       | parallel with R46 and |
|                       |                       | lowers the            |
|                       |                       | overvoltage           |
|                       |                       | protection threshold  |
|                       |                       | when EN2 is high.     |
+-----------------------+-----------------------+-----------------------+
| J8                    | 2-3                   | R49 is placed in      |
|                       |                       | parallel with R46 and |
|                       |                       | lowers the            |
|                       |                       | overvoltage           |
|                       |                       | protection threshold  |
|                       |                       | when EN1 is high.     |
+-----------------------+-----------------------+-----------------------+
| J10                   | Open                  | LED2+ LEDs are driven |
|                       |                       | through the Q6.       |
+-----------------------+-----------------------+-----------------------+
| J10                   | Closed                | Bypasses Q6. Used to  |
|                       |                       | compare performance   |
|                       |                       | without the LED       |
|                       |                       | current clamp.        |
+-----------------------+-----------------------+-----------------------+
| J11                   | Open                  | LED1+ LEDs are driven |
|                       |                       | through Q5.           |
+-----------------------+-----------------------+-----------------------+
| J11                   | Closed                | Bypasses Q5. Used to  |
|                       |                       | compare performance   |
|                       |                       | without the LED       |
|                       |                       | current clamp.        |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from: 
:git-no-OS:`Project Common Data Path </projects/max25603/src/common>`

The macros used in Common Data are defined in platform specific files
found in: 
:git-no-OS:`Project Platform Configuration Path </projects/max25603/src/platform>`

Basic example
~~~~~~~~~~~~~

The basic example code for the MAX25603 Evaluation Kit illustrates the
setup and functionality of the MAX25603 LED driver. The process starts
with initializing the device through the ``max25603_init`` function,
followed by selecting the relevant component. Key functions
``max25603_set_enable`` and ``max25603_set_beam`` are used to activate
the desired features. The code includes error handling to ensure
stability in operation. The primary flow involves configuring device
parameters, enabling the LED driver, initiating a low beam setup,
monitoring completion of operations, and subsequently disabling the beam
and the device. This structured flow serves as a practical example for
implementing reliable LED control in automotive lighting systems.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Hardware Used
^^^^^^^^^^^^^

- MAX25603 Evaluation Kit
- AD-APARD32690-SL (MAX32690) Development Platform

Connections
^^^^^^^^^^^

+-----------------+-----------------+-----------------+-----------------+
| Signal /        | MAX25603 EV Kit | AD-APARD32690-SL| Purpose         |
| Function        | Pin             | Pin             |                 |
+-----------------+-----------------+-----------------+-----------------+
| Power Input     | VINP1           | External 12 V   | Powers the      |
|                 |                 | supply          | MAX25603 EV     |
|                 |                 |                 | Kit. The        |
|                 |                 |                 | APARD32690-SL   |
|                 |                 |                 | is powered      |
|                 |                 |                 | separately via  |
|                 |                 |                 | USB or onboard  |
|                 |                 |                 | regulators.     |
+-----------------+-----------------+-----------------+-----------------+
| LED Output      | LED1+, LED2+    | LED connection  | Connect LED     |
|                 |                 | (design         | string anodes   |
|                 |                 | dependent)      | to LED1+/LED2+; |
|                 |                 |                 | LED cathodes    |
|                 |                 |                 | connect to GND. |
+-----------------+-----------------+-----------------+-----------------+
| Enable / PWM    | EN1             | D2 (GPIO) or    | Enables channel |
| Channel 1       |                 | GPIO P0.14      | 1 or provides   |
|                 |                 | (jumper-        | PWM dimming     |
|                 |                 | configured)     | control from    |
|                 |                 |                 | the MCU.        |
+-----------------+-----------------+-----------------+-----------------+
| Enable / PWM    | EN2             | D3 (GPIO) or    | Enables channel |
| Channel 2       |                 | GPIO P2.12      | 2 or provides   |
|                 |                 | (jumper-        | PWM dimming     |
|                 |                 | configured)     | control.        |
+-----------------+-----------------+-----------------+-----------------+
| SHUNT / Mode    | SHUNT           | D4 (GPIO)       | Optional        |
| Control         |                 |                 | digital control |
| (optional)      |                 |                 | for             |
|                 |                 |                 | compensation or |
|                 |                 |                 | operating mode  |
|                 |                 |                 | selection.      |
+-----------------+-----------------+-----------------+-----------------+
| Ground          | GND             | GND             | Common ground   |
|                 |                 |                 | reference       |
|                 |                 |                 | between both    |
|                 |                 |                 | boards          |
|                 |                 |                 | (required).     |
+-----------------+-----------------+-----------------+-----------------+
| Logic Reference | VCC (logic      | 3.3 V or 5 V    | Only required   |
| (optional)      | reference)      | pin             | if the EV Kit   |
|                 |                 |                 | logic needs an  |
|                 |                 |                 | external        |
|                 |                 |                 | reference;      |
|                 |                 |                 | ensure voltage  |
|                 |                 |                 | compatibility   |
|                 |                 |                 | (3.3 V          |
|                 |                 |                 | typical).       |
+-----------------+-----------------+-----------------+-----------------+

Build Command
~~~~~~~~~~~~~

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32690
   # to flash the code
   make run
