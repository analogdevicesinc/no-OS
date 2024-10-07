"""Unit tests for WMI modules

Some tests are optional, since they rely on remote machines and
usernames / passwords. To enable these, copy wmitest.master.ini
to wmitest.ini and set the parameters you have available.

The watcher tests spawn temporary processes and temporary
logical drives. These may get left behind.
"""

#
# TODO:
# - Test for negative timezone
# - Test for share name with embedded single quote
#

import os, sys
import datetime
try:
  import ConfigParser
except ImportError:
  import configparser as ConfigParser
import operator
try:
  import Queue
except ImportError:
  import queue as Queue
try:
  next
except NameError:
  def next (iterator): return iterator.next ()
import subprocess
import tempfile
import threading
import time
import unittest
import warnings

import pythoncom
import win32api
import win32con
import win32file

import wmi

ini = ConfigParser.SafeConfigParser ()
ini.read (["wmitest.master.ini", "wmitest.ini"])
settings = {}
if ini.has_section ("settings"):
  settings.update (ini.items ("settings"))
excludes = [i.strip () for i in settings.get ("excludes", "").split (",")]

COMPUTERS = [None, "."]
if "machine" in settings:
  COMPUTERS.append (settings['machine'])
IMPERSONATION_LEVELS = [None, "identify", "impersonate", "delegate"]
AUTHENTICATION_LEVELS = [None, "default", "none", "connect", "call", "pkt", "pktintegrity", "pktprivacy"]
AUTHORITIES = [None]
if set (["domain", "machine"]) <= set (settings):
  #~ AUTHORITIES.append ("kerberos:%s" % settings['domain'])
  AUTHORITIES.append ("ntlmdomain:%s" % settings['domain'])
PRIVILEGES = [None, ['security', '!shutdown']]
NAMESPACES = [None, "root/cimv2", "default"]

class TestBasicConnections (unittest.TestCase):

  def test_basic_connection (self):
    "Check that a standard connection works"
    self.assert_ (wmi.WMI ())

  def test_remote_connection (self):
    "Check that a remote connection works, if specified"
    if "machine" in settings:
      self.assert_ (wmi.WMI (settings['machine']))
    else:
      warnings.warn ("Skipping test_remote_connection")

  def test_simple_moniker (self):
    "Check that a simple moniker works"
    self.assert_ (wmi.WMI (moniker="winmgmts:"))

  def test_moniker_with_class (self):
    "Check that specifying a class in moniker works"
    c0 = wmi.WMI ().Win32_ComputerSystem
    c1 = wmi.WMI (moniker="winmgmts:Win32_ComputerSystem")
    self.assert_ (c0 == c1)

  def test_moniker_with_instance (self):
    "Check that specifying an instance in the moniker works"
    for c0 in wmi.WMI ().Win32_ComputerSystem ():
      break
    c1 = wmi.WMI (moniker='winmgmts:Win32_ComputerSystem.Name="%s"' % c0.Name)
    self.assert_ (c0 == c1)

  def test_impersonation_levels (self):
    "Check that specifying an impersonation level works"
    for impersonation in IMPERSONATION_LEVELS:
      self.assert_ (wmi.WMI (impersonation_level=impersonation))

  def test_authentication_levels (self):
    "Check that specifying an authentication level works"
    for authentication in AUTHENTICATION_LEVELS:
      try:
        c = wmi.WMI (authentication_level=authentication)
      except wmi.x_access_denied:
        warnings.warn ("Access denied for authentication level %s" % authentication)
      else:
        self.assert_ (c)

  def test_authority (self):
    "Check that specifying an authority works"
    for authority in AUTHORITIES:
      self.assert_ (wmi.WMI (authority=authority))

  def test_privileges (self):
    "Check that specifying privileges works"
    for privileges in PRIVILEGES:
      self.assert_ (wmi.WMI (privileges=privileges))

  def test_namespace (self):
    "Check that specifying a namespace works"
    for namespace in NAMESPACES:
      self.assert_ (wmi.WMI (namespace=namespace))

  def test_suffix (self):
    "Check that a suffix returns the class of that name"
    self.assert_ (wmi.WMI (namespace="DEFAULT", suffix="StdRegProv") == wmi.WMI (namespace="DEFAULT").StdRegProv)

  def test_user_password (self):
    "Check that username & password are passed through for a remote connection"
    if set (["machine", "user", "password"]) <= set (settings):
      self.assert_ (wmi.WMI (computer=settings['machine'], user=settings['user'], password=settings['password']))
    else:
      warnings.warn ("Skipping test_user_password because no machine, user or password")

  def test_too_much_authentication (self):
    "Check that user/password plus privs / suffix raises exception"
    self.assertRaises (wmi.x_wmi_authentication, wmi.WMI, computer='***', user="***", password="***", privileges=["***"])
    self.assertRaises (wmi.x_wmi_authentication, wmi.WMI, computer='***', user="***", password="***", suffix="***")

  def test_user_password_with_impersonation_level (self):
    "Check that an impersonation level works with a username / password"
    if not (set (["machine", "user", "password"]) <= set (settings)):
      warnings.warn ("Skipping test_user_password_with_impersonation_level because no machine, user or password")
    else:
      self.assert_ (
        wmi.WMI (
          computer=settings['machine'],
          user=settings['user'],
          password=settings['password'],
          impersonation_level="impersonate"
        )
      )

  def test_user_password_with_invalid_impersonation_level (self):
    "Check that an impersonation level works with a username / password"
    if not (set (["machine", "user", "password"]) <= set (settings)):
      warnings.warn ("Skipping test_user_password_with_invalid_impersonation_level because no machine, user or password")
    else:
      self.assertRaises (
        wmi.x_wmi_authentication,
        wmi.WMI,
        computer=settings['machine'],
        user=settings['user'],
        password=settings['password'],
        impersonation_level="***"
      )

  def test_user_password_with_authentication_level (self):
    "Check that an invalid impersonation level raises x_wmi_authentication"
    if not (set (["machine", "user", "password"]) <= set (settings)):
      warnings.warn ("Skipping test_user_password_with_authentication_level because no machine, user or password")
    else:
      self.assert_ (
        wmi.WMI (
          computer=settings['machine'],
          user=settings['user'],
          password=settings['password'],
          authentication_level="pktIntegrity"
        )
      )

  def test_user_password_with_invalid_authentication_level (self):
    "Check that an invalid authentication level raises x_wmi_authentication"
    if not (set (["machine", "user", "password"]) <= set (settings)):
      warnings.warn ("Skipping test_user_password_with_invalid_authentication_level because no machine, user or password")
    else:
      self.assertRaises (
        wmi.x_wmi_authentication,
        wmi.WMI,
        computer=settings['machine'],
        user=settings['user'],
        password=settings['password'],
        authentication_level="***"
      )

  def test_local_user_password (self):
    "Check that user/password for local connection raises exception"
    self.assertRaises (wmi.x_wmi_authentication, wmi.WMI, user="***", password="***")

  def test_find_classes (self):
    "Check ability to switch class scan on and off"
    self.assert_ (wmi.WMI (find_classes=True)._classes)
    self.assertFalse (wmi.WMI (find_classes=False)._classes)

  def test_find_classes_false (self):
    "By default, don't scan for classes but load them on demand"
    self.assertFalse (wmi.WMI ()._classes)
    self.assert_ (wmi.WMI ().classes)

  def test_classes_acts_as_list (self):
    self.assert_ (wmi.WMI ().classes.index)

  def test_classes_acts_as_dict (self):
    self.assert_ (wmi.WMI ().classes.keys)

class TestThreadedConnection (unittest.TestCase):


  def test_initialised_thread (self):
    """A WMI connection in a thread which has been initialised for COM
    should succeed.
    """
    def f (q):
      pythoncom.CoInitialize ()
      try:
        try:
          wmi.WMI ()
        except:
          q.put (False)
        else:
          q.put (True)
      finally:
        pythoncom.CoUninitialize ()

    q = Queue.Queue ()
    threading.Thread (target=f, args=(q,)).start ()
    self.assert_ (q.get ())

  def test_uninitialised_thread (self):
    """A WMI connection in a thread which has not been initialised
    for COM should fail with a wmi-specific exception.
    """
    def f (q):
      try:
        wmi.WMI ()
      except wmi.x_wmi_uninitialised_thread:
        q.put (True)
      except:
        q.put (False)
      else:
        q.put (False)

    q = Queue.Queue ()
    threading.Thread (target=f, args=(q,)).start ()
    self.assert_ (q.get ())

class TestMoniker (unittest.TestCase):

  def test_moniker (self):
    """Look at all possible options for moniker construction and pass
    them through to a WMI connector
    """
    for computer in COMPUTERS:
      if computer in (None, "."):
        local_authorities = [None]
      else:
        local_authorities = AUTHORITIES
      for impersonation_level in IMPERSONATION_LEVELS:
        for authentication_level in AUTHENTICATION_LEVELS:
          for authority in local_authorities:
            for privileges in PRIVILEGES:
              for namespace in NAMESPACES:
                moniker = wmi.construct_moniker (
                  computer=computer,
                  impersonation_level=impersonation_level,
                  authority=authority,
                  privileges=privileges,
                  namespace=namespace
                )
                self.assert_ (wmi.WMI (moniker=moniker), "Moniker failed: %s" % moniker)

  def test_moniker_root_namespace (self):
    "Check that namespace is prefixed by root if needed"
    self.assertEquals (wmi.construct_moniker (namespace="default"), "winmgmts:root/default")
    self.assertEquals (wmi.construct_moniker (namespace="root/default"), "winmgmts:root/default")

class TestFunctions (unittest.TestCase):

  times = [
    ((2000, 1, 1), "20000101******.******+***"),
    ((2000, 1, 1, 10, 0, 0), "20000101100000.******+***"),
    ((2000, 1, 1, 10, 0, 0, 100), "20000101100000.000100+***"),
    ((2000, 1, 1, 10, 0, 0, 100, "GMT"), "20000101100000.000100+GMT")
  ]

  def test_signed_to_unsigned (self):
    tests = [
      (0, 0),
      (-1, 0xffffffff),
      (+1, 1),
      (0x7fffffff, 0x7fffffff),
      (-0x7fffffff, 0x80000001)
    ]
    for signed, unsigned in tests:
      self.assertEquals (wmi.signed_to_unsigned (signed), unsigned)

  def test_from_1601 (self):
    "Check conversion from 100-ns intervals since 1601 (!)"
    self.assertEquals (wmi.from_1601 (0), datetime.datetime (1601, 1, 1))
    self.assertEquals (wmi.from_1601 (24 * 60 * 60 * 10 * 1000 * 1000), datetime.datetime (1601, 1, 2))

  def test_from_time (self):
    "Check conversion from time-tuple to time-string"
    for t, s in self.times:
      self.assertEquals (wmi.from_time (*t), s)

  def test_to_time (self):
    "Check conversion from time-string to time-tuple"
    for t, s in self.times:
      t = tuple (list (t) + ([None] * 8))[:8]
      self.assertEquals (wmi.to_time (s), t)

  def test_get_wmi_type (self):
    "Check that namespace, class & instance are identified correctly"
    self.assertEquals (wmi.get_wmi_type (wmi.WMI ()), "namespace")
    self.assertEquals (wmi.get_wmi_type (wmi.WMI ().Win32_ComputerSystem), "class")
    for i in wmi.WMI ().Win32_ComputerSystem ():
      self.assertEquals (wmi.get_wmi_type (i), "instance")

  def test_registry (self):
    """Convenience Registry function is identical to picking
    the StdRegProv class out of the DEFAULT namespace"""
    self.assertEquals (wmi.Registry (), wmi.WMI (namespace="DEFAULT").StdRegProv)

class TestWMI (unittest.TestCase):

  def setUp (self):
    self.connection = wmi.WMI (namespace="root/cimv2", find_classes=False)
    self.logical_disks = set (self.connection.Win32_LogicalDisk ())

class TestNamespace (TestWMI):

  def test_subclasses_of_simple (self):
    self.assert_ ("Win32_ComputerSystem" in self.connection.subclasses_of ())

  def test_subclasses_of_subtree (self):
    self.assert_ ("Win32_Desktop" in self.connection.subclasses_of ("CIM_Setting"))

  def test_subclasses_of_pattern (self):
    self.assert_ (set (["Win32_LogicalDisk", "Win32_MappedLogicalDisk"]) <= set (self.connection.subclasses_of ("CIM_LogicalDevice", "Win32_.*Disk")))

  def test_instances (self):
    self.assertEquals (self.logical_disks, set (self.connection.instances ("Win32_LogicalDisk")))

  def test_new (self):
    "Check this is an alias for the new method of the equivalent class"
    self.assertEquals (self.connection.new ("Win32_Process")._instance_of, self.connection.Win32_Process)

  def test_query (self):
    self.assertEquals (self.logical_disks, set (self.connection.query ("SELECT * FROM Win32_LogicalDisk")))

  def test_ipython_attributes_with_find_classes (self):
    connection = wmi.WMI (find_classes=True)
    self.assertEquals (sorted (connection._getAttributeNames ()), sorted (i for i in connection.classes if not i.startswith ("__")))

  def test_getattr (self):
    "Check that WMI classes are returned by attribute access on their namespace"
    connection = wmi.WMI (find_classes=True)
    for c in list (connection.classes)[:5]:
      wmi_class = getattr (connection, c)
      self.assert_ (isinstance (wmi_class, wmi._wmi_class))
      self.assertEquals (wmi_class._class_name, c)

  def test_watch_for (self):
    """Check that the watch_for method returns a watcher. The watcher itself
    will be tested elsewhere.
    """
    watcher = self.connection.watch_for (
      wmi_class="Win32_Process"
    )
    self.assert_ (isinstance (watcher, wmi._wmi_watcher))

class TestClass (TestWMI):

  def test_class_from_namespace (self):
    self.assert_ (self.connection.Win32_ComputerSystem._namespace is self.connection)

  def test_class_without_namespace (self):
    wmi_class = wmi.GetObject ("winmgmts:Win32_ComputerSystem")
    self.assert_ (wmi._wmi_class (None, wmi_class)._namespace)

  def test_query (self):
    self.assertEquals (
      set (self.connection.Win32_ComputerSystem.query ()),
      set (self.connection.query ("SELECT * FROM Win32_ComputerSystem"))
    )

  def test_query_with_where (self):
    this_drive = os.getcwd ()[:2]
    for drive in self.connection.Win32_LogicalDisk (Name=this_drive):
      self.assertEquals (drive.Name, this_drive)

  def test_query_with_fields (self):
    this_drive = os.getcwd ()[:2]
    properties = set (["MediaType"])
    self.assert_ ("Name" not in properties)
    for drive in self.connection.Win32_LogicalDisk (properties, Name=this_drive):
      self.assertEquals (set (drive.properties), set (properties))
      self.assert_ (drive.MediaType)
      self.assertRaises (AttributeError, getattr, drive, "Name")

  def test_watch_for (self):
    """Check that the watch_for method returns a watcher. The watcher itself
    will be tested elsewhere.
    """
    watcher = self.connection.Win32_Process.watch_for ()
    self.assert_ (isinstance (watcher, wmi._wmi_watcher))

  def test_instances (self):
    self.assertEquals (
      set (self.connection.Win32_LogicalDisk ()),
      set (self.connection.Win32_LogicalDisk.instances ())
    )

  def test_new (self):
    process = self.connection.Win32_Process.new ()
    self.assertEquals (wmi.get_wmi_type (process), "instance")
    self.assertEquals (process._instance_of, self.connection.Win32_process)


class TestWatcher (TestWMI):

  def new_letter (self):
    return \
      set ("%s:" % chr (i) for i in range (ord ('A'), 1 + ord ('Z'))).\
      difference (d.DeviceID for d in self.connection.Win32_LogicalDisk ()).\
      pop ()

  @staticmethod
  def create (new_letter):
    print "about to create drive with letter", new_letter
    here = os.path.dirname (os.path.abspath (__file__))
    win32file.DefineDosDevice (0, new_letter, here)
    try:
      #
      # This sleep is needed for the WMI pollster to react
      #
      time.sleep (2)
    finally:
      win32file.DefineDosDevice (2, new_letter, here)

  def test_creation (self):
    try:
      new_letter = self.new_letter ()
    except KeyError:
      warnings.warn ("Unable to find a spare drive letter to map.")
      return

    watcher = self.connection.Win32_LogicalDisk.watch_for (
      notification_type="Creation",
      DeviceID=new_letter
    )
    t = threading.Timer (2, self.create, (new_letter,))
    t.start ()
    found_disk = watcher (timeout_ms=20000)
    self.assert_ (isinstance (found_disk, wmi._wmi_object))
    self.assertEqual (found_disk.Caption, new_letter)
    t.join ()

  def test_event_with_no_params (self):
    try:
      new_letter = self.new_letter ()
    except KeyError:
      warnings.warn ("Unable to find a spare drive letter to map.")
      return

    watcher = self.connection.Win32_LogicalDisk.watch_for ()
    t = threading.Timer (2, self.create, (new_letter,))
    t.start ()
    found_disk = watcher (timeout_ms=20000)
    self.assert_ (isinstance (found_disk, wmi._wmi_object))
    self.assertEqual (found_disk.Caption, new_letter)
    t.join ()

  def test_valid_notification_types (self):
    for notification_type in ['operation', 'modification', 'creation', 'deletion']:
      self.assert_ (self.connection.Win32_LogicalDisk.watch_for (notification_type=notification_type))

  def test_invalid_notification_types (self):
    self.assertRaises (wmi.x_wmi, self.connection.Win32_LogicalDisk.watch_for, notification_type="***")


  def do_not_test_extrinsic_event (self):

    #
    # This doesn't seem implementable at the moment
    # as a test. I can't find a reproducible extrinsic
    # event except for Win32_DeviceChangeEvent and that
    # one would require someone to, eg, plug in / unplug
    # a USB stick.
    #
    # It looks as though Win32_ProcessStartTrace should work
    # and it does on my laptop; just not on my desktop.
    #

    def _create (queue):
      queue.put (subprocess.Popen ([sys.executable, "-c", "import time; time.sleep (10)"]))

    watcher = self.connection.Win32_ProcessStartTrace.watch_for (
      fields=["*"]##,
      #~ ProcessName=os.path.basename (sys.executable)
    )
    q = Queue.Queue ()
    t = threading.Timer (2, _create, (q,))
    try:
      t.start ()
      found_process = watcher (timeout_ms=20000)
      spawned_process = q.get_nowait ()
      self.assert_ (isinstance (found_process, wmi._wmi_event))
      self.assertEqual (int (found_process.ProcessID), spawned_process.pid)
    finally:
      t.cancel ()

class TestMethods (TestWMI):

  def test_exists (self):
    "Check that a well-known method is available by attribute"
    self.assert_ (self.connection.Win32_Process.Create)

  def test_params (self):
    "Check that the names and arrayness of params are picked up when not arrays"
    self.assertEquals (
      [(n, False) for n in ["CommandLine", "CurrentDirectory", "ProcessStartupInformation"]],
      self.connection.Win32_Process.Create.in_parameter_names
    )
    self.assertEquals (
      [("ProcessId", False), ("ReturnValue", False)],
      self.connection.Win32_Process.Create.out_parameter_names
    )

  def test_positional_params (self):
    dir = tempfile.mkdtemp ()
    filename = "abc.txt"
    contents = str (datetime.datetime.now ())
    handle, result = self.connection.Win32_Process.Create (
      "cmd /c echo %s > %s" % (contents, filename),
      dir,
      self.connection.Win32_ProcessStartup.new (ShowWindow=0)
    )
    time.sleep (0.5)
    self.assertEqual (open (os.path.join (dir, filename)).read (), contents + " \n")

  def test_named_params (self):
    dir = tempfile.mkdtemp ()
    filename = "abc.txt"
    contents = str (datetime.datetime.now ())
    handle, result = self.connection.Win32_Process.Create (
      ProcessStartupInformation=self.connection.Win32_ProcessStartup.new (ShowWindow=0),
      CurrentDirectory=dir,
      CommandLine="cmd /c echo %s > %s" % (contents, filename)
    )
    time.sleep (0.5)
    self.assertEqual (open (os.path.join (dir, filename)).read (), contents + " \n")

  def test_in_params_with_array (self):
    "Check that the names and arrayness of params are picked up when arrays"
    self.assertEquals (
      [("DNSServerSearchOrder", True)],
      self.connection.Win32_NetworkAdapterConfiguration.SetDNSServerSearchOrder.in_parameter_names
    )

  def test_instance_methods_are_distinct (self):
    """Check that the methods of difference instances of a class are distinct.
    This caused a problem when calling .Terminate on one process killed another.
    """
    methods = [d.Reset for d in self.logical_disks]
    for i in range (len (methods)-1):
      self.assertNotEqual (methods[i], methods[i+1])

  def test_call_from_class (self):
    "Check that a method can be called from a class"
    self.assert_ (self.connection.Win32_Process.Create (
      CommandLine=sys.executable + " -c pass",
      ProcessStartupInformation=self.connection.Win32_ProcessStartup.new (ShowWindow=0)
    ))

  def test_call_from_instance (self):
    "Check that a method can be called from an instance"
    handle, _ = self.connection.Win32_Process.Create (
      CommandLine=sys.executable,
      ProcessStartupInformation=self.connection.Win32_ProcessStartup.new (ShowWindow=0)
    )
    result = 1
    for p in self.connection.Win32_Process (Handle=handle):
      result, = p.Terminate ()
    self.assertEqual (result, 0)

class TestProperties (TestWMI):

  def test_access (self):
    "Check that all properties are available as attributes"
    for d in self.logical_disks:
      break
    for p in d.ole_object.Properties_:
      self.assertEqual (p.Value, getattr (d, p.Name))

  def test_attribute_passthrough (self):
    "Check that unknown attributes are passed through to the underlying object"
    for d in self.logical_disks:
      break
    #
    # Can't rely on the COM Objects testing identical or equal;
    # have to check their values and their emptiness.
    #
    self.assert_ (d.Properties_)
    self.assert_ (d.ole_object.Properties_)
    self.assertEqual (
      [p.Value for p in d.Properties_],
      [p.Value for p in d.ole_object.Properties_]
    )

  def test_settable (self):
    "Check that a writeable property can be written"
    name = str (time.time ()).split (".")[0]
    old_value = "***"
    new_value = "!!!"
    username = win32api.GetUserNameEx (win32con.NameSamCompatible)
    self.assert_ (not self.connection.Win32_Environment (Name=name, UserName=username))
    self.connection.Win32_Environment.new (Name=name, UserName=username, VariableValue=old_value).put ()
    for envvar in self.connection.Win32_Environment (Name=name, UserName=username):
      self.assertEqual (envvar.VariableValue, old_value)
      envvar.VariableValue = new_value
    try:
      for envvar in self.connection.Win32_Environment (Name=name, UserName=username):
        self.assertEqual (envvar.VariableValue, new_value)
    finally:
      for envvar in self.connection.Win32_Environment (Name=name, UserName=username):
        envvar.VariableValue = None

class TestInstances (TestWMI):

  def test_hashable (self):
    "Ensure instances are hashable so can be used in a set/dict"
    self.assert_ (dict.fromkeys (self.logical_disks))

  def test_equalable (self):
    "Ensure instances compare equal"
    self.assertEqual (self.logical_disks, self.logical_disks)

  def test_sortable (self):
    "Ensure instances sort by full path/key"
    self.assertEqual (
      sorted (self.logical_disks),
      sorted (self.logical_disks, key=operator.attrgetter ("DeviceID"))
    )

  def test_references (self):
    "Ensure that associations are special-cased to return wrapped objects"
    for d in self.logical_disks:
      break
    for r in d.references ("Win32_LogicalDiskRootDirectory"):
      self.assert_ (r.is_association)
      self.assertEqual (r.GroupComponent, d)
      self.assert_ (isinstance (r.GroupComponent, wmi._wmi_object))
      self.assert_ (isinstance (r.PartComponent, wmi._wmi_object))

  def test_associators (self):
    "Ensure that associators are returned by association / result"
    for d in self.logical_disks:
      if d.DeviceID == os.path.abspath (__file__)[:2]:
        break
    else:
      raise RuntimeError ("Unable to find the logical drive corresponding to this file")
    root_dir = d.associators (wmi_association_class="Win32_LogicalDiskRootDirectory")[0]
    self.assertEqual (root_dir.Name.lower (), d.Name.lower () + "\\".lower ())
    root_dir = d.associators (wmi_result_class="Win32_Directory")[0]
    self.assertEqual (root_dir.Name.lower (), d.Name.lower () + "\\")

  def test_derivation (self):
    "Check that derivation mimics WMI-provided Derivation_ property"
    for d in self.logical_disks:
      break
    self.assertEqual (d.derivation (), d.ole_object.Derivation_)

  def test_keys (self):
    "Check that the readonly keys property returns the keys for an object"
    self.assertEqual (self.connection.Win32_LogicalDisk.keys, ['DeviceID'])
    self.assertEqual (next (iter (self.logical_disks)).keys, ['DeviceID'])

class TestInstanceCreation (TestWMI):

  def test_create_instance (self):
    self.assert_ (isinstance (self.connection.Win32_ProcessStartup.new (ShowWindow=2), wmi._wmi_object))

class TestAssociations (TestWMI):

  def test_all_properties_available (self):
    #
    # An association can contain not only the associated
    # classes but also extra information as well. Ensure
    # that both types of data are correctly handled.
    #
    for q in self.connection.Win32_DiskQuota ():
      for p in q.properties:
        try:
          getattr (q, p)
        except wmi.x_wmi:
          assert False, "Error getting %s from %s" % (p, q)
      else:
        assert True

if __name__ == '__main__':
  unittest.main ()
