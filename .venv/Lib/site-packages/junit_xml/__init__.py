#!/usr/bin/env python
# -*- coding: UTF-8 -*-
from collections import defaultdict
import sys
import re
import xml.etree.ElementTree as ET
import xml.dom.minidom

from six import u, iteritems, PY2

try:
    # Python 2
    unichr
except NameError:  # pragma: nocover
    # Python 3
    unichr = chr

"""
Based on the understanding of what Jenkins can parse for JUnit XML files.

<?xml version="1.0" encoding="utf-8"?>
<testsuites errors="1" failures="1" tests="4" time="45">
    <testsuite errors="1" failures="1" hostname="localhost" id="0" name="test1"
               package="testdb" tests="4" timestamp="2012-11-15T01:02:29">
        <properties>
            <property name="assert-passed" value="1"/>
        </properties>
        <testcase classname="testdb.directory" name="1-passed-test" time="10"/>
        <testcase classname="testdb.directory" name="2-failed-test" time="20">
            <failure message="Assertion FAILED: failed assert" type="failure">
                the output of the testcase
            </failure>
        </testcase>
        <testcase classname="package.directory" name="3-errord-test" time="15">
            <error message="Assertion ERROR: error assert" type="error">
                the output of the testcase
            </error>
        </testcase>
        <testcase classname="package.directory" name="3-skipped-test" time="0">
            <skipped message="SKIPPED Test" type="skipped">
                the output of the testcase
            </skipped>
        </testcase>
        <testcase classname="testdb.directory" name="3-passed-test" time="10">
            <system-out>
                I am system output
            </system-out>
            <system-err>
                I am the error output
            </system-err>
        </testcase>
    </testsuite>
</testsuites>
"""


def decode(var, encoding):
    """
    If not already unicode, decode it.
    """
    if PY2:
        if isinstance(var, unicode):
            ret = var
        elif isinstance(var, str):
            if encoding:
                ret = var.decode(encoding)
            else:
                ret = unicode(var)
        else:
            ret = unicode(var)
    else:
        ret = str(var)
    return ret


class TestSuite(object):
    """
    Suite of test cases.
    Can handle unicode strings or binary strings if their encoding is provided.
    """

    def __init__(self, name, test_cases=None, hostname=None, id=None,
                 package=None, timestamp=None, properties=None, file=None,
                 log=None, url=None, stdout=None, stderr=None):
        self.name = name
        if not test_cases:
            test_cases = []
        try:
            iter(test_cases)
        except TypeError:
            raise Exception('test_cases must be a list of test cases')
        self.test_cases = test_cases
        self.timestamp = timestamp
        self.hostname = hostname
        self.id = id
        self.package = package
        self.file = file
        self.log = log
        self.url = url
        self.stdout = stdout
        self.stderr = stderr
        self.properties = properties

    def build_xml_doc(self, encoding=None):
        """
        Builds the XML document for the JUnit test suite.
        Produces clean unicode strings and decodes non-unicode with the help of encoding.
        @param encoding: Used to decode encoded strings.
        @return: XML document with unicode string elements
        """

        # build the test suite element
        test_suite_attributes = dict()
        test_suite_attributes['name'] = decode(self.name, encoding)
        if any(c.assertions for c in self.test_cases):
            test_suite_attributes['assertions'] = \
                str(sum([int(c.assertions) for c in self.test_cases if c.assertions]))
        test_suite_attributes['disabled'] = \
            str(len([c for c in self.test_cases if not c.is_enabled]))
        test_suite_attributes['failures'] = \
            str(len([c for c in self.test_cases if c.is_failure()]))
        test_suite_attributes['errors'] = \
            str(len([c for c in self.test_cases if c.is_error()]))
        test_suite_attributes['skipped'] = \
            str(len([c for c in self.test_cases if c.is_skipped()]))
        test_suite_attributes['time'] = \
            str(sum(c.elapsed_sec for c in self.test_cases if c.elapsed_sec))
        test_suite_attributes['tests'] = str(len(self.test_cases))

        if self.hostname:
            test_suite_attributes['hostname'] = decode(self.hostname, encoding)
        if self.id:
            test_suite_attributes['id'] = decode(self.id, encoding)
        if self.package:
            test_suite_attributes['package'] = decode(self.package, encoding)
        if self.timestamp:
            test_suite_attributes['timestamp'] = decode(self.timestamp, encoding)
        if self.timestamp:
            test_suite_attributes['file'] = decode(self.file, encoding)
        if self.timestamp:
            test_suite_attributes['log'] = decode(self.log, encoding)
        if self.timestamp:
            test_suite_attributes['url'] = decode(self.url, encoding)

        xml_element = ET.Element("testsuite", test_suite_attributes)

        # add any properties
        if self.properties:
            props_element = ET.SubElement(xml_element, "properties")
            for k, v in self.properties.items():
                attrs = {'name': decode(k, encoding), 'value': decode(v, encoding)}
                ET.SubElement(props_element, "property", attrs)

        # add test suite stdout
        if self.stdout:
            stdout_element = ET.SubElement(xml_element, "system-out")
            stdout_element.text = decode(self.stdout, encoding)

        # add test suite stderr
        if self.stderr:
            stderr_element = ET.SubElement(xml_element, "system-err")
            stderr_element.text = decode(self.stderr, encoding)

        # test cases
        for case in self.test_cases:
            test_case_attributes = dict()
            test_case_attributes['name'] = decode(case.name, encoding)
            if case.assertions:
                # Number of assertions in the test case
                test_case_attributes['assertions'] = "%d" % case.assertions
            if case.elapsed_sec:
                test_case_attributes['time'] = "%f" % case.elapsed_sec
            if case.timestamp:
                test_case_attributes['timestamp'] = decode(case.timestamp, encoding)
            if case.classname:
                test_case_attributes['classname'] = decode(case.classname, encoding)
            if case.status:
                test_case_attributes['status'] = decode(case.status, encoding)
            if case.category:
                test_case_attributes['class'] = decode(case.category, encoding)
            if case.file:
                test_case_attributes['file'] = decode(case.file, encoding)
            if case.line:
                test_case_attributes['line'] = decode(case.line, encoding)
            if case.log:
                test_case_attributes['log'] = decode(case.log, encoding)
            if case.url:
                test_case_attributes['url'] = decode(case.url, encoding)

            test_case_element = ET.SubElement(
                xml_element, "testcase", test_case_attributes)

            # failures
            if case.is_failure():
                attrs = {'type': 'failure'}
                if case.failure_message:
                    attrs['message'] = decode(case.failure_message, encoding)
                if case.failure_type:
                    attrs['type'] = decode(case.failure_type, encoding)
                failure_element = ET.Element("failure", attrs)
                if case.failure_output:
                    failure_element.text = decode(case.failure_output, encoding)
                test_case_element.append(failure_element)

            # errors
            if case.is_error():
                attrs = {'type': 'error'}
                if case.error_message:
                    attrs['message'] = decode(case.error_message, encoding)
                if case.error_type:
                    attrs['type'] = decode(case.error_type, encoding)
                error_element = ET.Element("error", attrs)
                if case.error_output:
                    error_element.text = decode(case.error_output, encoding)
                test_case_element.append(error_element)

            # skippeds
            if case.is_skipped():
                attrs = {'type': 'skipped'}
                if case.skipped_message:
                    attrs['message'] = decode(case.skipped_message, encoding)
                skipped_element = ET.Element("skipped", attrs)
                if case.skipped_output:
                    skipped_element.text = decode(case.skipped_output, encoding)
                test_case_element.append(skipped_element)

            # test stdout
            if case.stdout:
                stdout_element = ET.Element("system-out")
                stdout_element.text = decode(case.stdout, encoding)
                test_case_element.append(stdout_element)

            # test stderr
            if case.stderr:
                stderr_element = ET.Element("system-err")
                stderr_element.text = decode(case.stderr, encoding)
                test_case_element.append(stderr_element)

        return xml_element

    @staticmethod
    def to_xml_string(test_suites, prettyprint=True, encoding=None):
        """
        Returns the string representation of the JUnit XML document.
        @param encoding: The encoding of the input.
        @return: unicode string
        """

        try:
            iter(test_suites)
        except TypeError:
            raise Exception('test_suites must be a list of test suites')

        xml_element = ET.Element("testsuites")
        attributes = defaultdict(int)
        for ts in test_suites:
            ts_xml = ts.build_xml_doc(encoding=encoding)
            for key in ['failures', 'errors', 'tests', 'disabled']:
                attributes[key] += int(ts_xml.get(key, 0))
            for key in ['time']:
                attributes[key] += float(ts_xml.get(key, 0))
            xml_element.append(ts_xml)
        for key, value in iteritems(attributes):
            xml_element.set(key, str(value))

        xml_string = ET.tostring(xml_element, encoding=encoding)
        # is encoded now
        xml_string = TestSuite._clean_illegal_xml_chars(
            xml_string.decode(encoding or 'utf-8'))
        # is unicode now

        if prettyprint:
            # minidom.parseString() works just on correctly encoded binary strings
            xml_string = xml_string.encode(encoding or 'utf-8')
            xml_string = xml.dom.minidom.parseString(xml_string)
            # toprettyxml() produces unicode if no encoding is being passed or binary string with an encoding
            xml_string = xml_string.toprettyxml(encoding=encoding)
            if encoding:
                xml_string = xml_string.decode(encoding)
            # is unicode now
        return xml_string

    @staticmethod
    def to_file(file_descriptor, test_suites, prettyprint=True, encoding=None):
        """
        Writes the JUnit XML document to a file.
        """
        xml_string = TestSuite.to_xml_string(
            test_suites, prettyprint=prettyprint, encoding=encoding)
        # has problems with encoded str with non-ASCII (non-default-encoding) characters!
        file_descriptor.write(xml_string)

    @staticmethod
    def _clean_illegal_xml_chars(string_to_clean):
        """
        Removes any illegal unicode characters from the given XML string.

        @see: http://stackoverflow.com/questions/1707890/fast-way-to-filter-illegal-xml-unicode-chars-in-python
        """

        illegal_unichrs = [
            (0x00, 0x08), (0x0B, 0x1F), (0x7F, 0x84), (0x86, 0x9F),
            (0xD800, 0xDFFF), (0xFDD0, 0xFDDF), (0xFFFE, 0xFFFF),
            (0x1FFFE, 0x1FFFF), (0x2FFFE, 0x2FFFF), (0x3FFFE, 0x3FFFF),
            (0x4FFFE, 0x4FFFF), (0x5FFFE, 0x5FFFF), (0x6FFFE, 0x6FFFF),
            (0x7FFFE, 0x7FFFF), (0x8FFFE, 0x8FFFF), (0x9FFFE, 0x9FFFF),
            (0xAFFFE, 0xAFFFF), (0xBFFFE, 0xBFFFF), (0xCFFFE, 0xCFFFF),
            (0xDFFFE, 0xDFFFF), (0xEFFFE, 0xEFFFF), (0xFFFFE, 0xFFFFF),
            (0x10FFFE, 0x10FFFF)]

        illegal_ranges = ["%s-%s" % (unichr(low), unichr(high))
                          for (low, high) in illegal_unichrs
                          if low < sys.maxunicode]

        illegal_xml_re = re.compile(u('[%s]') % u('').join(illegal_ranges))
        return illegal_xml_re.sub('', string_to_clean)


class TestCase(object):
    """A JUnit test case with a result and possibly some stdout or stderr"""

    def __init__(self, name, classname=None, elapsed_sec=None, stdout=None,
                 stderr=None, assertions=None, timestamp=None, status=None,
                 category=None, file=None, line=None, log=None, group=None,
                 url=None):
        self.name = name
        self.assertions = assertions
        self.elapsed_sec = elapsed_sec
        self.timestamp = timestamp
        self.classname = classname
        self.status = status
        self.category = category
        self.file = file
        self.line = line
        self.log = log
        self.url = url
        self.stdout = stdout
        self.stderr = stderr

        self.is_enabled = True
        self.error_message = None
        self.error_output = None
        self.error_type = None
        self.failure_message = None
        self.failure_output = None
        self.failure_type = None
        self.skipped_message = None
        self.skipped_output = None

    def add_error_info(self, message=None, output=None, error_type=None):
        """Adds an error message, output, or both to the test case"""
        if message:
            self.error_message = message
        if output:
            self.error_output = output
        if error_type:
            self.error_type = error_type

    def add_failure_info(self, message=None, output=None, failure_type=None):
        """Adds a failure message, output, or both to the test case"""
        if message:
            self.failure_message = message
        if output:
            self.failure_output = output
        if failure_type:
            self.failure_type = failure_type

    def add_skipped_info(self, message=None, output=None):
        """Adds a skipped message, output, or both to the test case"""
        if message:
            self.skipped_message = message
        if output:
            self.skipped_output = output

    def is_failure(self):
        """returns true if this test case is a failure"""
        return self.failure_output or self.failure_message

    def is_error(self):
        """returns true if this test case is an error"""
        return self.error_output or self.error_message

    def is_skipped(self):
        """returns true if this test case has been skipped"""
        return self.skipped_output or self.skipped_message
