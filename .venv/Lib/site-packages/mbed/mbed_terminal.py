#!/usr/bin/env python

# Copyright (c) 2016 ARM Limited, All Rights Reserved
# SPDX-License-Identifier: Apache-2.0

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.

# You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
# either express or implied.


# pylint: disable=too-many-arguments, too-many-locals, too-many-branches, too-many-lines, line-too-long,
# pylint: disable=too-many-nested-blocks, too-many-public-methods, too-many-instance-attributes, too-many-statements
# pylint: disable=invalid-name, missing-docstring, bad-continuation


# Global class used for global config
class MbedTerminal(object):
    serial = None # Serial() object
    port = None
    baudrate = None
    echo = None

    def __init__(self, port, baudrate=9600, echo=True, timeout=10):
        self.port = port
        self.baudrate = int(baudrate)
        self.timeout = int(timeout)
        self.echo = False if str(echo).lower() == 'off' else True

        try:
            from serial import Serial, SerialException
            self.serial = Serial(self.port, baudrate=self.baudrate, timeout=self.timeout)
            self.serial.flush()
            self.serial.reset_input_buffer()
        except (IOError, ImportError, OSError, Exception):
            self.serial = None
            return

    def terminal(self, print_header=True):
        try:
            import serial.tools.miniterm as miniterm
        except (IOError, ImportError, OSError):
            return False

        term = miniterm.Miniterm(self.serial, echo=self.echo)
        term.exit_character = '\x03'
        term.menu_character = '\x14'
        term.set_rx_encoding('UTF-8')
        term.set_tx_encoding('UTF-8')

        def console_print(text):
            term.console.write('--- %s ---\n' % text)

        def get_print_help():
            return """
--- Mbed Serial Terminal (0.3a)
--- Based on miniterm from pySerial
---
--- Ctrl+b     Send Break (reset target)
--- Ctrl+c     Exit terminal
--- Ctrl+e     Toggle local echo
--- Ctrl+h     Help
--- Ctrl+t     Menu escape key, followed by:
---    p       Change COM port
---    b       Change baudrate
---    Tab     Show detailed terminal info
---    Ctrl+a  Change encoding (default UTF-8)
---    Ctrl+f  Edit filters
---    Ctrl+l  Toggle EOL
---    Ctrl+r  Toggle RTS
---    Ctrl+d  Toggle DTR
---    Ctrl+c  Send control character to remote
---    Ctrl+t  Send control character to remote
"""

        def print_help():
            term.console.write(get_print_help())


        def input_handler():
            menu_active = False
            while term.alive:
                try:
                    c = term.console.getkey()
                except KeyboardInterrupt:
                    c = '\x03'
                if not term.alive:
                    break
                if menu_active and c in ['p', 'b', '\t', '\x01', '\x03', '\x04', '\x05', '\x06', '\x0c', '\x14']:
                    term.handle_menu_key(c)
                    menu_active = False
                elif c == term.menu_character:
                    console_print('[MENU]')
                    menu_active = True # next char will be for menu
                elif c == '\x02': # ctrl+b sendbreak
                    console_print('[RESET]')
                    self.reset()
                elif c == '\x03': # ctrl+c
                    console_print('[QUIT]')
                    term.stop()
                    term.alive = False
                    break
                elif c == '\x05': # ctrl+e
                    console_print('[ECHO %s]' % ('OFF' if term.echo else 'ON'))
                    term.echo = not term.echo
                elif c == '\x08': # ctrl+h
                    print_help()
#                elif c == '\t': # tab/ctrl+i
#                    term.dump_port_settings()
                else:
                    text = c
                    for transformation in term.tx_transformations:
                        text = transformation.tx(text)
                    term.serial.write(term.tx_encoder.encode(text))
                    if term.echo:
                        echo_text = c
                        for transformation in term.tx_transformations:
                            echo_text = transformation.echo(echo_text)
                        term.console.write(echo_text)
        term.writer = input_handler

        if print_header:
            console_print("Terminal on {p.name} - {p.baudrate},{p.bytesize},{p.parity},{p.stopbits}".format(p=term.serial))

        term.start()

        try:
            term.join(True)
        except KeyboardInterrupt:
            pass
        term.join()
        term.close()

        return True

    def reset(self):
        try:
            self.serial.sendBreak()
        except:
            try:
                self.serial.setBreak(False) # For Linux the following setBreak() is needed to release the reset signal on the target mcu.
            except:
                return False
        return True
