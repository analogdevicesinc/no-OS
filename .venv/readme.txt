*****************
Python WMI Module
*****************

What is it?
===========

Windows Management Instrumentation (WMI) is Microsoft's implementation of
Web-Based Enterprise Management (WBEM), an industry initiative to provide
a Common Information Model (CIM) for pretty much any information about a
computer system.

The Python WMI module is a lightweight wrapper on top of the pywin32
extensions, and hides some of the messy plumbing needed to get Python to
talk to the WMI API. It's pure Python and should work with any version of
Python from 2.1 onwards (list comprehensions) and any recent version of
pywin32.


Where do I get it?
==================

http://timgolden.me.uk/python/wmi/index.html


Copyright & License?
====================

(c) Tim Golden <mail@timgolden.me.uk> 5th June 2003
Licensed under the (GPL-compatible) MIT License:
http://www.opensource.org/licenses/mit-license.php


How do I install it?
====================

When all's said and done, it's just a module. But for those
who like setup programs::

  python setup.py install

It's also pip/easy_install-able


How do I use it?
================

There's a tutorial here: http://timgolden.me.uk/python/wmi/tutorial.html,
and some examples at: http://timgolden.me.uk/python/wmi/cookbook.html
but as a quick taster, try this, to show all stopped services::

  import wmi

  c = wmi.WMI ()
  for s in c.Win32_Service ():
    if s.State == 'Stopped':
      print s.Caption, s.State


Prerequisites
=============

If you're running a recent Python (2.1+) on a recent Windows (2k, 2k3, XP)
and you have Mark Hammond's win32 extensions installed, you're probably
up-and-running already. Otherwise...

Windows
-------
If you're running Win9x / NT4 you'll need to get WMI support
from Microsoft. Microsoft URLs change quite often, so I suggest you
do this: http://www.google.com/search?q=wmi+downloads

Python
------
http://www.python.org/ (just in case you didn't know)

pywin32 (was win32all)
----------------------
http://starship.python.net/crew/mhammond/win32/Downloads.html
Specifically, builds 154/155 fixed a problem which affected the WMI
moniker construction. You can still work without this fix, but some
more complex monikers will fail.

makepy
------
(NB my own experience over several systems is that this
step isn't necessary. However, if you have problems...)
You may have to compile makepy support for some typelibs. The following
are reported to be significant:

Microsoft WMI Scripting Library
WMI ADSI Extension Type Library
WMICntl Type Library

If you've not done this before, start the PythonWin environment, select
Tools > Com Makepy utility from the menu, select the library by name, and
click [OK].


