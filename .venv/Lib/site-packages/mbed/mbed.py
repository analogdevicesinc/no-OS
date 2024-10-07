#!/usr/bin/env python

# Copyright (c) 2016-2019 Arm Limited, All Rights Reserved
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

from __future__ import print_function

try:
    # Python 2
    basestring = (unicode, str)
    from urlparse import urlparse
    from urllib2 import urlopen
    from urllib import quote
except NameError:
    # Python 3
    basestring = str
    from urllib.parse import urlparse, quote
    from urllib.request  import urlopen

import traceback
import sys
import re
import subprocess
import os
import json
import platform
import contextlib
import shutil
import stat
import errno
import ctypes
from itertools import chain, repeat
import time
import zipfile
import argparse
from random import randint
from contextlib import contextmanager


# Application version
ver = '1.10.5'

# Default paths to Mercurial and Git
hg_cmd = 'hg'
git_cmd = 'git'

# override python command when running standalone Mbed CLI
python_cmd = sys.executable
if os.path.basename(python_cmd).startswith('mbed'):
    python_cmd = 'python'

ignores = [
    # Version control folders
    ".hg",
    ".git",
    ".svn",
    ".CVS",
    ".cvs",

    # Version control fallout
    "*.orig",

    # mbed Tools
    "BUILD",
    ".build",
    ".export",

    # Online IDE caches
    ".msub",
    ".meta",
    ".ctags*",

    # uVision project files
    "*.uvproj",
    "*.uvopt",

    # Eclipse project files
    "*.project",
    "*.cproject",
    "*.launch",

    # IAR project files
    "*.ewp",
    "*.eww",

    # GCC make
    "/Makefile",
    "Debug",

    # HTML files
    "*.htm",

    # Settings files
    ".mbed",
    "*.settings",
    "mbed_settings.py",

    # Python
    "*.py[cod]",
    "# subrepo ignores",
    ]

# git & url (no #rev)
regex_repo_url = r'^(git\://|file\://|ssh\://|https?\://|)(([^/:@]+)(\:([^/:@]+))?@)?([^/:]{3,})(\:\d+)?[:/](.+?)(\.git|\.hg|\/?)$'
# mbed url is subset of hg. mbed doesn't support ssh transport though so https? urls cannot be converted to ssh
regex_mbed_url = r'^(https?)://(([^/:@]+)(\:([^/:@]+))?@)?([\w\-\.]*mbed\.(co\.uk|org|com))(\:\d+)?[:/](.+?)/?$'
# mbed sdk builds url are treated specially
regex_build_url = r'^(https?://([\w\-\.]*mbed\.(co\.uk|org|com))/(users|teams)/([\w\-]{1,32})/(repos|code)/([\w\-]+))/builds/?([\w\-]{6,40}|tip)?/?$'

# valid .lib reference to local (unpublished) repo - dir#rev
regex_local_ref = r'^([\w.+-][\w./+-]*?)/?(?:#(.*))?$'
# valid .lib reference to repo - url#rev
regex_url_ref = r'^(.*/([\w.+-]+)(?:\.\w+)?)/?(?:#(.*))?$'

# match official release tags
regex_rels_official = r'^(release|rel|mbed-os|[rv]+)?[.-]?\d+(\.\d+)*$'
# match rc/beta/alpha release tags
regex_rels_all = r'^(release|rel|mbed-os|[rv]+)?[.-]?\d+(\.\d+)*([a-z0-9.-]+)?$'

# base url for all mbed related repos (used as sort of index)
mbed_base_url = 'https://github.com/ARMmbed'
# default mbed OS url
mbed_os_url = 'https://github.com/ARMmbed/mbed-os'
# default mbed library url
mbed_lib_url = 'https://mbed.org/users/mbed_official/code/mbed/builds/'
# mbed SDK tools needed for programs based on mbed SDK library
mbed_sdk_tools_url = 'https://mbed.org/users/mbed_official/code/mbed-sdk-tools'

# a list of public SCM service (github/butbucket) which support http, https and ssh schemas
public_scm_services = ['bitbucket.org', 'github.com', 'gitlab.com']

# commands that don't get the current work path shown
skip_workpath_commands = ["config", "cfg", "conf"]

# verbose logging
verbose = False
very_verbose = False
install_requirements = True
cache_repositories = True

mbed_app_file_name = "mbed_app.json"

# stores current working directory for recursive operations
cwd_root = ""
_cwd = os.getcwd()

# Logging and output
def log(msg, is_error=False):
    sys.stderr.write(msg) if is_error else sys.stdout.write(msg)

def message(msg):
    if very_verbose:
        return "[mbed-%s] %s\n" % (os.getpid(), msg)
    else:
        return "[mbed] %s\n" % msg

def info(msg, level=1):
    if level <= 0 or verbose:
        for line in msg.splitlines():
            log(message(line))

def action(msg):
    for line in msg.splitlines():
        log(message(line))

def warning(msg):
    lines = msg.splitlines()
    log(message("WARNING: %s" % lines.pop(0)), True)
    for line in lines:
        log("       %s\n" % line, True)
    log("---\n", True)

def error(msg, code=-1):
    lines = msg.splitlines()
    log(message("ERROR: %s" % lines.pop(0)), True)
    for line in lines:
        log("       %s\n" % line, True)
    log("---\n", True)
    sys.exit(code)

def offline_warning(offline, top=True):
    if top and offline:
        log("\n")
        log(".=============================== OFFLINE MODE ================================.\n")
        log("|   Offline mode is enabled. No connections to remote repositories will be    |\n")
        log("|           made and only locally cached repositories will be used.           |\n")
        log("|   This might break some actions if non-cached repositories are referenced.  |\n")
        log("'============================================================================='\n\n")

def progress_cursor():
    while True:
        for cursor in '|/-\\':
            yield cursor

progress_spinner = progress_cursor()

def progress():
    sys.stdout.write(next(progress_spinner))
    sys.stdout.flush()
    sys.stdout.write('\b')

def show_progress(title, percent, max_width=80):
    if sys.stdout.isatty():
        percent = round(float(percent), 2)
        show_percent = '%.2f' % percent
        bwidth = max_width - len(str(title)) - len(show_percent) - 6 # 6 equals the spaces and paddings between title, progress bar and percentage
        sys.stdout.write('%s |%s%s| %s%%\r' % (str(title), '#' * int(percent * bwidth // 100), '-' * (bwidth - int(percent * bwidth // 100)), show_percent))
        sys.stdout.flush()

def hide_progress(max_width=80):
    if sys.stdout.isatty():
        sys.stdout.write("\r%s\r" % (' ' * max_width))

def create_default_mbed_app():
    # Default data content
    if not os.path.exists(mbed_app_file_name):
        data = {'target_overrides':{'K64F':{'platform.stdio-baud-rate': 9600}}}
        with open(mbed_app_file_name, "w") as mbed_app_file:
            json.dump(data, mbed_app_file, indent=4)

# Process execution
class ProcessException(Exception):
    pass


def popen(command, **kwargs):
    # print for debugging
    info("Exec \"%s\" in \"%s\"" % (' '.join(command), getcwd()))
    proc = None
    try:
        proc = subprocess.Popen(command, **kwargs)
    except OSError as e:
        if e.args[0] == errno.ENOENT:
            error(
                "Could not execute \"%s\" in \"%s\".\n"
                "You can verify that it's installed and accessible from your current path by executing \"%s\".\n" % (' '.join(command), getcwd(), command[0]), e.args[0])
        else:
            raise e

    if proc and proc.wait() != 0:
        raise ProcessException(proc.returncode, command[0], ' '.join(command), getcwd())
    return proc

def pquery(command, output_callback=None, stdin=None, **kwargs):
    if very_verbose:
        info("Exec \"%s\" in \"%s\"" % (' '.join(command), getcwd()))
    try:
        proc = subprocess.Popen(command, bufsize=0, stdout=subprocess.PIPE, stderr=subprocess.PIPE, **kwargs)
    except OSError as e:
        if e.args[0] == errno.ENOENT:
            error(
                "Could not execute \"%s\" in \"%s\".\n"
                "You can verify that it's installed and accessible from your current path by executing \"%s\".\n" % (' '.join(command), getcwd(), command[0]), e.args[0])
        else:
            raise e

    if output_callback:
        line = ""
        while 1:
            s = str(proc.stderr.read(1))
            line += s
            if s == '\r' or s == '\n':
                output_callback(line, s)
                line = ""

            if proc.returncode is None:
                proc.poll()
            else:
                break

    stdout, _ = proc.communicate(stdin)

    if very_verbose:
        log(stdout.decode(sys.getfilesystemencoding()).strip() + "\n")

    if proc.returncode != 0:
        raise ProcessException(proc.returncode, command[0], ' '.join(command), getcwd())

    return stdout.decode(sys.getfilesystemencoding())

def rmtree_readonly(directory):
    if os.path.islink(directory):
        os.remove(directory)
    else:
        def remove_readonly(func, path, _):
            os.chmod(path, stat.S_IWRITE)
            func(path)
        shutil.rmtree(directory, onerror=remove_readonly)

def sizeof_fmt(num, suffix='B'):
    for unit in ['','K','M','G','T','P','E','Z']:
        if abs(num) < 1024.0:
            return "%3.1f%s%s" % (num, unit, suffix)
        num /= 1024.0
    return "%.1f%s%s" % (num, 'Yi', suffix)

# Directory navigation
@contextlib.contextmanager
def cd(newdir):
    global _cwd
    prevdir = getcwd()
    os.chdir(newdir)
    _cwd = newdir
    try:
        yield
    finally:
        os.chdir(prevdir)
        _cwd = prevdir

def getcwd():
    global _cwd
    return _cwd

def relpath(root, path):
    return path[len(root)+1:]


def staticclass(cls):
    for k, v in cls.__dict__.items():
        if hasattr(v, '__call__') and not k.startswith('__'):
            setattr(cls, k, staticmethod(v))

    return cls


# Handling for multiple version controls
scms = {}
def scm(name):
    def _scm(cls):
        scms[name] = cls()
        return cls
    return _scm

# pylint: disable=no-self-argument, no-method-argument, no-member, no-self-use, unused-argument
@scm('bld')
@staticclass
class Bld(object):
    name = 'bld'
    default_branch = 'default'

    def init(path):
        if not os.path.exists(path):
            os.mkdir(path)
        else:
            if len(os.listdir(path)) > 1:
                error("Directory \"%s\" is not empty." % path)

    def cleanup():
        info("Cleaning up library build folder")
        for fl in os.listdir('.'):
            if not fl.startswith('.'):
                if os.path.isfile(fl):
                    os.remove(fl)
                else:
                    shutil.rmtree(fl)

    def clone(url, path=None, depth=None, protocol=None):
        m = Bld.isvalidurl(url)
        if not m:
            raise ProcessException(1, "Not a library build URL")

        try:
            Bld.init(path)
            with cd(path):
                rev = Hg.remoteid(m.group(1), 'tip')
                if not rev:
                    error("Unable to fetch library build information")
                Bld.seturl(url+'/'+rev)
        except Exception as e:
            if os.path.isdir(path):
                rmtree_readonly(path)
            error(e.args[1], e.args[0])

    def fetch_rev(url, rev):
        rev_file = os.path.join('.'+Bld.name, '.rev-' + rev + '.zip')
        try:
            if not os.path.exists(rev_file):
                action("Downloading library build \"%s\" (might take a while)" % rev)
                inurl = urlopen(url)
                with open(rev_file, 'wb') as outfd:
                    data = None
                    while data != '':
                        # Download and write the data in 1 MB chunks
                        data = inurl.read(1024 * 1024)
                        outfd.write(data)
        except Exception:
            if os.path.isfile(rev_file):
                os.remove(rev_file)
            raise Exception(128, "Download failed!\nPlease try again later.")

    def unpack_rev(rev):
        rev_file = os.path.join('.'+Bld.name, '.rev-' + rev + '.zip')
        try:
            with zipfile.ZipFile(rev_file) as zf:
                action("Unpacking library build \"%s\" in \"%s\"" % (rev, getcwd()))
                zf.extractall('.')
        except:
            if os.path.isfile(rev_file):
                os.remove(rev_file)
            raise Exception(128, "An error occurred while unpacking library archive \"%s\" in \"%s\"" % (rev_file, getcwd()))

    def checkout(rev, clean=False):
        url = Bld.geturl()
        m = Bld.isvalidurl(url)
        if not m:
            raise ProcessException(1, "Not a library build URL")
        rev = Hg.remoteid(m.group(1), rev)
        if not rev:
            error("Unable to fetch library build information")

        arch_url = m.group(1) + '/archive/' + rev + '.zip'
        Bld.fetch_rev(arch_url, rev)

        if rev != Bld.getrev() or clean:
            Bld.cleanup()

            info("Checkout \"%s\" in %s" % (rev, os.path.basename(getcwd())))
            try:
                Bld.unpack_rev(rev)
                Bld.seturl(url+'/'+rev)
            except Exception as e:
                error(e.args[1], e.args[0])

    def update(rev=None, clean=False, clean_files=False, is_local=False):
        return Bld.checkout(rev, clean)

    def untracked():
        return ""

    def isvalidurl(url):
        return re.match(regex_build_url, url.strip().replace('\\', '/'))

    def seturl(url):
        info("Setting url to \"%s\" in %s" % (url, getcwd()))
        if not os.path.exists('.'+Bld.name):
            os.mkdir('.'+Bld.name)

        fl = os.path.join('.'+Bld.name, 'bldrc')
        try:
            with open(fl, 'w') as f:
                f.write(url)
        except IOError:
            error("Unable to write bldrc file in \"%s\"" % fl, 1)

    def geturl():
        with open(os.path.join('.bld', 'bldrc')) as f:
            url = f.read().strip()
        m = Bld.isvalidurl(url)
        return m.group(1)+'/builds' if m else ''

    def getrev():
        with open(os.path.join('.bld', 'bldrc')) as f:
            url = f.read().strip()

        m = Bld.isvalidurl(url)
        return m.group(8) if m else ''

    def getbranch():
        return "default"

    def gettags(rev=None):
        return []


# pylint: disable=no-self-argument, no-method-argument, no-member, no-self-use, unused-argument
@scm('hg')
@staticclass
class Hg(object):
    name = 'hg'
    default_branch = 'default'
    ignore_file = os.path.join('.hg', 'hgignore')

    def init(path=None):
        popen([hg_cmd, 'init'] + ([path] if path else []) + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def cleanup():
        return True

    def clone(url, name=None, depth=None, protocol=None):
        if verbose or very_verbose:
            popen([hg_cmd, 'clone', formaturl(url, protocol), name] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))
        else:
            pquery([hg_cmd, 'clone', '--config', 'progress.assume-tty=true', formaturl(url, protocol), name], output_callback=Hg.action_progress)
            hide_progress()

    def add(dest):
        info("Adding reference \"%s\"" % dest)
        try:
            popen([hg_cmd, 'add', dest] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))
        except ProcessException:
            pass

    def remove(dest):
        info("Removing reference \"%s\" " % dest)
        try:
            pquery([hg_cmd, 'rm', '-f', dest] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))
        except ProcessException:
            pass

    def commit(msg=None):
        popen([hg_cmd, 'commit'] + (['-m', msg] if msg else [])  + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def publish(all_refs=None):
        popen([hg_cmd, 'push'] + (['--new-branch'] if all_refs else []) + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def fetch():
        info("Fetching revisions from remote repository to \"%s\"" % os.path.basename(getcwd()))
        popen([hg_cmd, 'pull'] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def discard():
        info("Discarding local changes in \"%s\"" % os.path.basename(getcwd()))
        popen([hg_cmd, 'update', '-C'] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def checkout(rev, clean=False, clean_files=False):
        info("Checkout \"%s\" in %s" % (rev if rev else "latest", os.path.basename(getcwd())))
        if clean_files:
            files = pquery([hg_cmd, 'status', '--no-status', '-ui']).splitlines()
            for f in files:
                info("Remove untracked file \"%s\"" % f)
                os.remove(f)
        popen([hg_cmd, 'update'] + (['-C'] if clean else []) + (['-r', rev] if rev else []) + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def update(rev=None, clean=False, clean_files=False, is_local=False):
        if not is_local:
            Hg.fetch()
        Hg.checkout(rev, clean, clean_files)

    def status():
        return pquery([hg_cmd, 'status'] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def dirty():
        return pquery([hg_cmd, 'status', '-q'])

    def untracked():
        return pquery([hg_cmd, 'status', '--no-status', '-u']).splitlines()

    def outgoing():
        try:
            pquery([hg_cmd, 'outgoing'])
            return 1
        except ProcessException as e:
            if e.args[0] != 1:
                raise e
            return 0

    def seturl(url):
        info("Setting url to \"%s\" in %s" % (url, getcwd()))
        hgrc = os.path.join('.hg', 'hgrc')
        tagpaths = '[paths]'
        remote = 'default'
        lines = []

        try:
            with open(hgrc) as f:
                lines = f.read().splitlines()
        except IOError:
            pass

        if tagpaths in lines:
            idx = lines.index(tagpaths)
            m = re.match(r'^([\w_]+)\s*=\s*(.*)$', lines[idx+1])
            if m:
                remote = m.group(1)
                del lines[idx+1]
            lines.insert(idx, remote+' = '+url)
        else:
            lines.append(tagpaths)
            lines.append(remote+' = '+url)

    def geturl():
        tagpaths = '[paths]'
        default_url = ''
        url = ''

        try:
            with open(os.path.join('.hg', 'hgrc')) as f:
                lines = f.read().splitlines()
                if tagpaths in lines:
                    idx = lines.index(tagpaths)
                    m = re.match(r'^([\w_]+)\s*=\s*(.*)$', lines[idx+1])
                    if m:
                        if m.group(1) == 'default':
                            default_url = m.group(2)
                        else:
                            url = m.group(2)
        except IOError:
            pass

        if default_url:
            url = default_url

        return formaturl(url or pquery([hg_cmd, 'paths', 'default']).strip())

    def getrev():
        if os.path.isfile(os.path.join('.hg', 'dirstate')):
            from io import open
            with open(os.path.join('.hg', 'dirstate'), 'rb') as f:
                return "".join('{:02x}'.format(x) for x in bytearray(f.read(6)))
        else:
            return ""

    def getbranch():
        return pquery([hg_cmd, 'branch']).strip() or ""

    def gettags():
        tags = []
        refs = pquery([hg_cmd, 'tags']).strip().splitlines() or []
        for ref in refs:
            m = re.match(r'^(.+?)\s+(\d+)\:([a-f0-9]+)$', ref)
            if m:
                tags.append([m.group(3), m.group(1)])
        return tags

    def remoteid(url, rev=None):
        return pquery([hg_cmd, 'id', '--id', url] + (['-r', rev] if rev else [])).strip() or ""

    def hgrc():
        hook = 'ignore.local = .hg/hgignore'
        hgrc = os.path.join('.hg', 'hgrc')
        try:
            with open(hgrc) as f:
                exists = hook in f.read().splitlines()
        except IOError:
            exists = False

        if not exists:
            try:
                with open(hgrc, 'a') as f:
                    f.write('[ui]\n')
                    f.write(hook + '\n')
            except IOError:
                error("Unable to write hgrc file in \"%s\"" % hgrc, 1)

    def ignores():
        Hg.hgrc()
        try:
            with open(Hg.ignore_file, 'w') as f:
                f.write("syntax: glob\n"+'\n'.join(ignores)+'\n')
        except IOError:
            error("Unable to write ignore file in \"%s\"" % os.path.join(getcwd(), Hg.ignore_file), 1)

    def ignore(dest):
        Hg.hgrc()
        try:
            with open(Hg.ignore_file) as f:
                exists = dest in f.read().splitlines()
        except IOError:
            exists = False

        if not exists:
            try:
                with open(Hg.ignore_file, 'a') as f:
                    f.write(dest + '\n')
            except IOError:
                error("Unable to write ignore file in \"%s\"" % os.path.join(getcwd(), Hg.ignore_file), 1)

    def unignore(dest):
        Hg.ignore_file = os.path.join('.hg', 'hgignore')
        try:
            with open(Hg.ignore_file) as f:
                lines = f.read().splitlines()
        except IOError:
            lines = []

        if dest in lines:
            lines.remove(dest)
            try:
                with open(Hg.ignore_file, 'w') as f:
                    f.write('\n'.join(lines) + '\n')
            except IOError:
                error("Unable to write ignore file in \"%s\"" % os.path.join(getcwd(), Hg.ignore_file), 1)

    def action_progress(line, sep):
        m = re.match(r'(\w+).+?\s+(\d+)/(\d+)\s+.*?', line)
        if m:
            if m.group(1) == "manifests":
                show_progress('Downloading', (float(m.group(2)) / float(m.group(3))) * 20)
            if m.group(1) == "files":
                show_progress('Downloading', (float(m.group(2)) / float(m.group(3))) * 100)


# pylint: disable=no-self-argument, no-method-argument, no-member, no-self-use, unused-argument
@scm('git')
@staticclass
class Git(object):
    name = 'git'
    default_branch = 'master'
    ignore_file = os.path.join('.git', 'info', 'exclude')

    def init(path=None):
        popen([git_cmd, 'init'] + ([path] if path else []) + ([] if very_verbose else ['-q']))

    def cleanup():
        info("Cleaning up Git index")
        pquery([git_cmd, 'checkout', '--detach', 'HEAD'] + ([] if very_verbose else ['-q'])) # detach head so local branches are deletable
        branches = []
        lines = pquery([git_cmd, 'branch']).strip().splitlines() # fetch all local branches
        for line in lines:
            if re.match(r'^\*?\s+\((.+)\)$', line):
                continue
            line = re.sub(r'\s+', '', line)
            branches.append(line)

        for branch in branches: # delete all local branches so the new repo clone is not poluted
            pquery([git_cmd, 'branch', '-D', branch])

    def clone(url, name=None, depth=None, protocol=None):
        if verbose or very_verbose:
            popen([git_cmd, 'clone', formaturl(url, protocol), name] + (['--depth', depth] if depth else []) + (['-v'] if very_verbose else ([] if verbose else ['-q'])))
        else:
            pquery([git_cmd, 'clone', '--progress', formaturl(url, protocol), name] + (['--depth', depth] if depth else []), output_callback=Git.action_progress)
            hide_progress()

    def add(dest):
        info("Adding reference "+dest)
        try:
            popen([git_cmd, 'add', dest] + (['-v'] if very_verbose else []))
        except ProcessException:
            pass

    def remove(dest):
        info("Removing reference "+dest)
        try:
            pquery([git_cmd, 'rm', '-f', dest] + ([] if very_verbose else ['-q']))
        except ProcessException:
            pass

    def commit(msg=None):
        popen([git_cmd, 'commit', '-a'] + (['-m', msg] if msg else []) + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def publish(all_refs=None):
        if all_refs:
            popen([git_cmd, 'push', '--all'] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))
        else:
            remote = Git.getremote()
            branch = Git.getbranch()
            if remote and branch:
                popen([git_cmd, 'push', remote, branch] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))
            else:
                err = "Unable to publish outgoing changes for \"%s\" in \"%s\".\n" % (os.path.basename(getcwd()), getcwd())
                if not remote:
                    error(err+"The local repository is not associated with a remote one.", 1)
                if not branch:
                    error(err+"Working set is not on a branch.", 1)

    def fetch():
        info("Fetching revisions from remote repository to \"%s\"" % os.path.basename(getcwd()))
        popen([git_cmd, 'fetch', '--all', '--tags', '--force'] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def discard(clean_files=False):
        info("Discarding local changes in \"%s\"" % os.path.basename(getcwd()))
        pquery([git_cmd, 'reset', 'HEAD'] + ([] if very_verbose else ['-q'])) # unmarks files for commit
        pquery([git_cmd, 'checkout', '.'] + ([] if very_verbose else ['-q'])) # undo  modified files
        pquery([git_cmd, 'clean', '-fd'] + (['-x'] if clean_files else []) + (['-q'] if very_verbose else ['-q'])) # cleans up untracked files and folders

    def merge(dest):
        info("Merging \"%s\" with \"%s\"" % (os.path.basename(getcwd()), dest))
        popen([git_cmd, 'merge', dest] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def checkout(rev, clean=False):
        if not rev:
            return
        info("Checkout \"%s\" in %s" % (rev, os.path.basename(getcwd())))
        branch = None
        refs = Git.getbranches(rev)
        for ref in refs: # re-associate with a local or remote branch (rev is the same)
            m = re.match(r'^(.*?)\/(.*?)$', ref)
            if m and m.group(2) != "HEAD": # matches origin/<branch> and isn't HEAD ref
                if not os.path.exists(os.path.join('.git', 'refs', 'heads', m.group(2))): # okay only if local branch with that name doesn't exist (git will checkout the origin/<branch> in that case)
                    branch = m.group(2)
            elif ref != "HEAD":
                branch = ref # matches local branch and isn't HEAD ref

            if branch:
                info("Revision \"%s\" matches a branch \"%s\" reference. Re-associating with branch" % (rev, branch))
                popen([git_cmd, 'checkout', branch] + ([] if very_verbose else ['-q']))
                break

        if not branch:
            popen([git_cmd, 'checkout', rev] + (['-f'] if clean else []) + ([] if very_verbose else ['-q']))

    def update(rev=None, clean=False, clean_files=False, is_local=False):
        if not is_local:
            Git.fetch()
        if clean:
            Git.discard(clean_files)
        if rev:
            Git.checkout(rev, clean)
        else:
            remote = Git.getremote()
            branch = Git.getbranch()
            if remote and branch:
                try:
                    Git.merge('%s/%s' % (remote, branch))
                except ProcessException:
                    pass
            else:
                err = "Unable to update \"%s\" in \"%s\"." % (os.path.basename(getcwd()), getcwd())
                if not remote:
                    info(err+"\nThe local repository is not associated with a remote one.\nYou should associate your repository with a remote one.")
                if not branch:
                    info(err+"\nThe working set is not on a branch.\nYou should switch to a branch or create a new one from the current revision.")

    def status():
        return pquery([git_cmd, 'status', '-s'] + (['-v'] if very_verbose else []))

    def dirty():
        return pquery([git_cmd, 'status', '-uno', '--porcelain'])

    def untracked():
        return pquery([git_cmd, 'ls-files', '--others', '--exclude-standard']).splitlines()

    def outgoing():
        # Get default remote
        remote = Git.getremote()
        if not remote:
            return -1
        # Get current branch
        branch = Git.getbranch()
        if not branch:
            # Default to "master" in detached mode
            branch = "master"
        # Check if local branch exists. If not, then just carry on
        try:
            pquery([git_cmd, 'rev-parse', '%s' % branch])
        except ProcessException:
            return 0
        # Check if remote branch exists. If not, then it's a new branch
        try:
            if not pquery([git_cmd, 'rev-parse', '%s/%s' % (remote, branch)]):
                return 1
        except ProcessException:
            return 1
        # Check for outgoing commits for the same remote branch only if it exists locally and remotely
        return 1 if pquery([git_cmd, 'log', '%s/%s..%s' % (remote, branch, branch)]) else 0

    # Checks whether current working tree is detached
    def isdetached():
        return True if Git.getbranch() == "" else False

    # Finds default remote
    def getremote():
        remote = None
        remotes = Git.getremotes('push')
        for r in remotes:
            remote = r[0]
            # Prefer origin which is Git's default remote when cloning
            if r[0] == "origin":
                break
        return remote

    # Finds all associated remotes for the specified remote type
    def getremotes(rtype='fetch'):
        result = []
        remotes = pquery([git_cmd, 'remote', '-v']).strip().splitlines()
        for remote in remotes:
            remote = re.split(r'\s', remote)
            t = re.sub('[()]', '', remote[2])
            if not rtype or rtype == t:
                result.append([remote[0], remote[1], t])
        return result

    def seturl(url):
        info("Setting url to \"%s\" in %s" % (url, getcwd()))
        return pquery([git_cmd, 'remote', 'set-url', 'origin', url]).strip()

    def geturl():
        url = ""
        remotes = Git.getremotes()
        for remote in remotes:
            url = remote[1]
            if remote[0] == "origin": # Prefer origin URL
                break
        return formaturl(url)

    def getrev():
        return pquery([git_cmd, 'rev-parse', 'HEAD']).strip()

    # Gets current branch or returns empty string if detached
    def getbranch(rev='HEAD'):
        try:
            branch = pquery([git_cmd, 'rev-parse', '--symbolic-full-name', '--abbrev-ref', rev]).strip()
        except ProcessException:
            branch = "master"
        return branch if branch != "HEAD" else ""

    # Get all refs
    def getrefs():
        try:
            return pquery([git_cmd, 'show-ref', '--dereference']).strip().splitlines()
        except ProcessException:
            return []

    # Finds branches (local or remote). Will match rev if specified
    def getbranches(rev=None, ret_rev=False):
        result = []
        refs = Git.getrefs()
        for ref in refs:
            m = re.match(r'^(.+)\s+refs\/(heads|remotes)\/(.+)$', ref)
            if m and (not rev or m.group(1).startswith(rev)):
                result.append(m.group(1) if ret_rev else m.group(3))
        return result

    # Finds tags. Will match rev if specified
    def gettags():
        tags = []
        refs = Git.getrefs()
        for ref in refs:
            m = re.match(r'^(.+)\s+refs\/tags\/(.+)$', ref)
            if m:
                t = m.group(2)
                if re.match(r'^(.+)\^\{\}$', t): # detect tag "pointer"
                    t = re.sub(r'\^\{\}$', '', t) # remove "pointer" chars, e.g. some-tag^{}
                    for tag in tags:
                        if tag[1] == t:
                            tags.remove(tag)
                tags.append([m.group(1), t])
        return tags

    # Finds branches a rev belongs to
    def revbranches(rev):
        branches = []
        lines = pquery([git_cmd, 'branch', '-a', '--contains'] + ([rev] if rev else [])).strip().splitlines()
        for line in lines:
            if re.match(r'^\*?\s+\((.+)\)$', line):
                continue
            line = re.sub(r'\s+', '', line)
            branches.append(line)
        return branches

    def ignores():
        try:
            ignore_file_parent_directory = os.path.dirname(Git.ignore_file)
            if not os.path.exists(ignore_file_parent_directory):
                os.mkdir(ignore_file_parent_directory)

            with open(Git.ignore_file, 'w') as f:
                f.write('\n'.join(ignores)+'\n')
        except IOError:
            error("Unable to write ignore file in \"%s\"" % os.path.join(getcwd(), Git.ignore_file), 1)

    def ignore(dest):
        try:
            with open(Git.ignore_file) as f:
                exists = dest in f.read().splitlines()
        except IOError:
            exists = False

        if not exists:
            try:
                ignore_file_parent_directory = os.path.dirname(Git.ignore_file)
                if not os.path.exists(ignore_file_parent_directory):
                    os.mkdir(ignore_file_parent_directory)

                with open(Git.ignore_file, 'a') as f:
                    f.write(dest.replace("\\", "/") + '\n')
            except IOError:
                error("Unable to write ignore file in \"%s\"" % os.path.join(getcwd(), Git.ignore_file), 1)
    def unignore(dest):
        try:
            with open(Git.ignore_file) as f:
                lines = f.read().splitlines()
        except IOError:
            lines = []

        if dest in lines:
            lines.remove(dest)
            try:
                ignore_file_parent_directory = os.path.dirname(Git.ignore_file)
                if not os.path.exists(ignore_file_parent_directory):
                    os.mkdir(ignore_file_parent_directory)

                with open(Git.ignore_file, 'w') as f:
                    f.write('\n'.join(lines) + '\n')
            except IOError:
                error("Unable to write ignore file in \"%s\"" % os.path.join(getcwd(), Git.ignore_file), 1)

    def action_progress(line, sep):
        m = re.match(r'([\w :]+)\:\s*(\d+)% \((\d+)/(\d+)\)', line)
        if m:
            if m.group(1) == "remote: Compressing objects" and int(m.group(4)) > 100:
                show_progress('Preparing', (float(m.group(3)) / float(m.group(4))) * 100)
            if m.group(1) == "Receiving objects":
                show_progress('Downloading', (float(m.group(3)) / float(m.group(4))) * 80)
            if m.group(1) == "Resolving deltas":
                show_progress('Downloading', (float(m.group(3)) / float(m.group(4))) * 20 + 80)
            if m.group(1) == "Checking out files":
                show_progress('Checking out', (float(m.group(3)) / float(m.group(4))) * 100)

_environment_markers = {
    "platform_system": platform.system()
}

_comparators = {
    "!=": lambda a, b: a != b,
    "==": lambda a, b: a == b,
}

def _eval_environment_marker(marker):
    m = re.match(r'\s*([^!=]+)\s*([!=]+)\s*([^\s]*)', marker)
    if m:
        try:
            environment_marker_value = _environment_markers[m.group(1)]
            environment_marker_cmp = m.group(3).strip("\"'")
            return _comparators[m.group(2)](environment_marker_value, environment_marker_cmp)
        except KeyError as e:
            pass

    raise Exception("Unsupported environment marker: {}".format(marker))

# Repository object
class Repo(object):
    is_local = False
    is_build = False
    name = None
    path = None
    url = None
    rev = None
    scm = None
    libs = []
    cache = None

    @classmethod
    def fromurl(cls, url, path=None):
        repo = cls()
        m_local = re.match(regex_local_ref, url.strip().replace('\\', '/'))
        m_repo_ref = re.match(regex_url_ref, url.strip().replace('\\', '/'))
        m_bld_ref = re.match(regex_build_url, url.strip().replace('\\', '/'))
        if m_local:
            repo.name = os.path.basename(path or m_local.group(1))
            repo.path = os.path.abspath(path or os.path.join(getcwd(), m_local.group(1)))
            repo.url = m_local.group(1)
            repo.rev = m_local.group(2)
            repo.is_local = True
        elif m_bld_ref:
            repo.name = os.path.basename(path or m_bld_ref.group(7))
            repo.path = os.path.abspath(path or os.path.join(getcwd(), repo.name))
            repo.url = m_bld_ref.group(1)+'/builds'
            repo.rev = m_bld_ref.group(8)
            repo.is_build = True
        elif m_repo_ref:
            repo.name = re.sub(r'\.(git|hg)/?$', '', os.path.basename(path or m_repo_ref.group(2)))
            repo.path = os.path.abspath(path or os.path.join(getcwd(), repo.name))
            repo.url = formaturl(m_repo_ref.group(1))
            repo.rev = m_repo_ref.group(3)
        else:
            error('Invalid repository (%s)' % url.strip(), -1)

        cache_cfg = Global().cache_cfg()
        if cache_repositories and cache_cfg['cache'] == 'enabled':
            repo.cache = cache_cfg['cache_dir']

        return repo

    @classmethod
    def fromlib(cls, lib=None):
        try:
            with open(lib) as f:
                ref = f.read(200)
        except UnicodeDecodeError:
            ref = ""

        m_local = re.match(regex_local_ref, ref.strip().replace('\\', '/'))
        m_repo_ref = re.match(regex_url_ref, ref.strip().replace('\\', '/'))
        m_bld_ref = re.match(regex_build_url, ref.strip().replace('\\', '/'))

        if m_repo_ref:
            rev = m_repo_ref.group(3)
            if rev and rev != 'latest' and rev != 'tip' and not re.match(r'^([a-fA-F0-9]{6,40})$', rev):
                error('Named branches not allowed in .lib, offending lib is {} '.format(os.path.basename(lib)))

        if not (m_local or m_bld_ref or m_repo_ref):
            warning(
                "File \"%s\" in \"%s\" uses a non-standard .lib file extension, which is not compatible with the mbed build tools.\n" % (os.path.basename(lib), os.path.split(lib)[0]))
            return False
        else:
            return cls.fromurl(ref, lib[:-4])

    @classmethod
    def fromrepo(cls, path=None):
        repo = cls()
        if path is None:
            path = Repo.findparent(getcwd())
            if path is None:
                error(
                    "Could not find mbed program in current path \"%s\".\n"
                    "You can fix this by calling \"mbed new .\" or \"mbed config root .\" in the root of your program." % getcwd())

        repo.path = os.path.abspath(path)
        repo.name = os.path.basename(repo.path)

        cache_cfg = Global().cache_cfg()
        if cache_repositories and cache_cfg['cache'] == 'enabled':
            repo.cache = cache_cfg['cache_dir']

        repo.sync()

        if repo.scm is None:
            warning(
                "Program \"%s\" in \"%s\" does not use source control management.\n"
                "To fix this you should use \"mbed new .\" in the root of your program." % (repo.name, repo.path))

        return repo

    @classmethod
    def isrepo(cls, path=None):
        for name, _ in scms.items():
            if os.path.isdir(os.path.join(path, '.'+name)):
                return True

        return False

    @classmethod
    def findparent(cls, path=None):
        path = os.path.abspath(path or getcwd())

        while cd(path):
            if os.path.isfile(os.path.join(path, Cfg.file)) or Repo.isrepo(path):
                return path

            tpath = path
            path = os.path.split(path)[0]
            if tpath == path:
                break

        return None

    @classmethod
    def pathtype(cls, path=None):
        path = os.path.abspath(path or getcwd())

        depth = 0
        while cd(path):
            tpath = path
            path = Repo.findparent(path)
            if path:
                depth += 1
                path = os.path.split(path)[0]
                if tpath == path:       # Reached root.
                    break
            else:
                break

        return "directory" if depth == 0 else ("program" if depth == 1 else "library")

    def revtype(self, rev=None, ret_type=True, ret_rev=True, fmt=3):
        if rev is None or len(rev) == 0:
            output = ('latest' if fmt & 1 else '') + (' revision in the current branch' if fmt & 2 else '')
        elif re.match(r'^([a-fA-F0-9]{6,40})$', rev) or re.match(r'^([0-9]+)$', rev):
            revtags = self.gettags(rev) if rev else []
            output = ('rev ' if fmt & 1 else '') + (('#' + rev[:12] + ((' (tag' + ('s' if len(revtags) > 1 else '') + ': ' + ', '.join(revtags[0:2]) + ')') if len(revtags) else '')) if fmt & 2 and rev else '')
        else:
            output = ('branch/tag' if fmt & 1 else '') + (' "'+rev+'"' if fmt & 2 else '')

        return re.sub(r' \(', ', ', re.sub(r'\)', '', output)) if fmt & 4 else output

    @classmethod
    def isurl(cls, url):
        return re.match(regex_url_ref, url.strip().replace('\\', '/'))

    @classmethod
    def isinsecure(cls, url):
        path = re.sub(r'^(.+)[#?].*$', r'\1', url)
        if os.path.isdir(path): # local paths are secure
            return False
        up = urlparse(url)
        return (not up) or (up.scheme and up.scheme not in ['http', 'https', 'ssh', 'git']) or (up.port and int(up.port) not in [22, 80, 443])

    @property
    def lib(self):
        return self.path + '.' + ('bld' if self.is_build else 'lib')

    @property
    def fullurl(self):
        if self.url:
            return (self.url.rstrip('/') + '/' +
                    (('' if self.is_build else '#') +
                        self.rev if self.rev else ''))

    def sync(self):
        self.url = None
        self.rev = None
        if os.path.isdir(self.path):
            try:
                self.scm = self.getscm()
                if self.scm and self.scm.name == 'bld':
                    self.is_build = True
            except ProcessException:
                pass

            try:
                self.url = self.geturl()
                if not self.url:
                    self.is_local = True
                    ppath = self.findparent(os.path.split(self.path)[0])
                    self.url = relpath(ppath, self.path).replace("\\", "/") if ppath else os.path.basename(self.path)
            except ProcessException:
                pass

            try:
                self.rev = self.getrev()
            except ProcessException:
                pass

            try:
                self.libs = list(self.getlibs())
            except ProcessException:
                pass

    def getscm(self):
        for name, scm in scms.items():
            if os.path.isdir(os.path.join(self.path, '.'+name)):
                return scm

    def gettags(self, rev=None):
        tags = self.scm.gettags() if self.scm else []
        if rev:
            return [tag[1] for tag in tags if tag[0].startswith(rev)]
        else:
            return tags

    # Pass backend SCM commands and parameters if SCM exists
    def __wrap_scm(self, method):
        def __scm_call(*args, **kwargs):
            if self.scm and hasattr(self.scm, method) and callable(getattr(self.scm, method)):
                with cd(self.path):
                    return getattr(self.scm, method)(*args, **kwargs)
        return __scm_call

    def __getattr__(self, attr):
        if attr in ['geturl', 'getrev', 'add', 'remove', 'ignores', 'ignore', 'unignore',
                    'status', 'dirty', 'commit', 'outgoing', 'publish', 'checkout', 'update',
                    'isdetached']:
            wrapper = self.__wrap_scm(attr)
            self.__dict__[attr] = wrapper
            return wrapper
        else:
            raise AttributeError("Repo instance doesn't have attribute '%s'" % attr)

    def remove(self, dest, *args, **kwargs):
        if os.path.isfile(dest):
            try:
                os.remove(dest)
            except OSError:
                pass
        return self.scm.remove(dest, *args, **kwargs)

    def clone(self, url, path, rev=None, depth=None, protocol=None, offline=False, **kwargs):
        # Sorted so repositories that match urls are attempted first
        info("Trying to guess source control management tool. Supported SCMs: %s" % ', '.join([s.name for s in scms.values()]))
        for scm in scms.values():
            main = True
            cache = self.get_cache(url, scm.name)

            # Try to clone with cache ref first
            if cache and not os.path.isdir(path):
                info("Found matching cached repository in \"%s\"" % cache)
                try:
                    if os.path.split(path)[0] and not os.path.isdir(os.path.split(path)[0]):
                        os.makedirs(os.path.split(path)[0])

                    info("Carbon copy from \"%s\" to \"%s\"" % (cache, path))
                    with self.cache_lock_held(url):
                        shutil.copytree(cache, path)

                    #
                    # If no revision was specified, use the branch associated with the cache. In the
                    # github case this will be the default branch (IOTBTOOL-279)
                    #
                    if not rev:
                        with cd(cache):
                            branch = scm.getbranch()
                            if branch:
                                rev = branch
                            else:
                                # Can't find the cache branch; use a sensible default
                                rev = scm.default_branch

                    with cd(path):
                        scm.seturl(formaturl(url, protocol))
                        scm.cleanup()
                        info("Update cached copy from remote repository")
                        scm.update(rev, True, is_local=offline)
                        main = False
                except (ProcessException, IOError):
                    info("Discarding cached repository")
                    if os.path.isdir(path):
                        rmtree_readonly(path)

            # Main clone routine if the clone with cache ref failed (might occur if cache ref is dirty)
            if main:
                if offline:
                    continue
                try:
                    scm.clone(url, path, depth=depth, protocol=protocol, **kwargs)
                except ProcessException:
                    if os.path.isdir(path):
                        rmtree_readonly(path)
                    continue

            self.scm = scm
            self.url = url
            self.path = os.path.abspath(path)
            self.ignores()
            with self.cache_lock_held(url):
                self.set_cache(url)
            return True

        if offline:
            error("Unable to clone repository \"%s\" in offline mode ('--offline' used)." % url)
            if os.path.isdir(path):
                rmtree_readonly(path)

        return False

    def getlibs(self):
        for root, dirs, files in os.walk(self.path):
            dirs[:] = [d for d in dirs  if not d.startswith('.')]
            files[:] = [f for f in files if not f.startswith('.')]

            for f in files:
                if f.endswith('.lib') or f.endswith('.bld'):
                    repo = Repo.fromlib(os.path.join(root, f))
                    if repo:
                        yield repo
                    if f[:-4] in dirs:
                        dirs.remove(f[:-4])

    def write(self):
        up = urlparse(self.url)
        if up.hostname:
            url = up._replace(netloc=up.hostname + (':'+str(up.port) if up.port else '')).geturl() # strip auth string
        else:
            url = self.url # use local repo "urls" as is

        if os.path.isfile(self.lib):
            with open(self.lib) as f:
                lib_repo = Repo.fromurl(f.read().strip())
                if (formaturl(lib_repo.url, 'https') == formaturl(url, 'https') # match URLs in common schema (https)
                        and (lib_repo.rev == self.rev                           # match revs, even if rev is None (valid for repos with no revisions)
                             or (lib_repo.rev and self.rev
                                 and lib_repo.rev == self.rev[0:len(lib_repo.rev)]))):  # match long and short rev formats
                    #print self.name, 'unmodified'
                    return

        if up.hostname in public_scm_services:
            # Safely convert repo URL to https schema if this is a public SCM service (github/butbucket), supporting all schemas.
            # This allows anonymous cloning of public repos without having to have ssh keys and associated accounts at github/bitbucket/etc.
            # Without this anonymous users will get clone errors with ssh repository links even if the repository is public.
            # See https://help.github.com/articles/which-remote-url-should-i-use/
            url = formaturl(url, 'https')

        ref = url.rstrip('/') + '/' + (('' if self.is_build else '#') + self.rev if self.rev else '')
        action("Updating reference \"%s\" -> \"%s\"" % (relpath(cwd_root, self.path) if cwd_root != self.path else self.name, ref))
        with open(self.lib, 'w') as f:
            f.write(ref+"\n")

    def rm_untracked(self):
        untracked = self.scm.untracked()
        for f in untracked:
            if re.match(r'(.+)\.(lib|bld)$', f) and os.path.isfile(f):
                action("Remove untracked library reference \"%s\"" % f)
                os.remove(f)

    def url2cachedir(self, url):
        up = urlparse(formaturl(url, 'https'))
        if self.cache and up and up.netloc:
            return os.path.join(self.cache, quote(up.netloc), quote(re.sub(r'^/', '', up.path)))

    def get_cache(self, url, scm):
        cpath = self.url2cachedir(url)
        if cpath and os.path.isdir(os.path.join(cpath, '.'+scm)):
            return cpath

    def set_cache(self, url):
        cpath = self.url2cachedir(url)
        if cpath and os.path.isdir(self.path):
            try:
                if not os.path.isdir(cpath):
                    os.makedirs(cpath)
                scm_dir = '.'+self.scm.name
                if os.path.isdir(os.path.join(cpath, scm_dir)):
                    rmtree_readonly(os.path.join(cpath, scm_dir))
                shutil.copytree(os.path.join(self.path, scm_dir), os.path.join(cpath, scm_dir))
            except Exception:
                warning("Unable to cache \"%s\" to \"%s\"" % (self.path, cpath))
        return False

    def cache_lock(self, url):
        cpath = self.url2cachedir(url)
        if not cpath:
            return False

        if not os.path.isdir(cpath):
            os.makedirs(cpath)

        lock_dir = os.path.join(cpath, '.lock')
        lock_file = os.path.join(lock_dir, 'pid')
        timeout = 300

        for i in range(timeout):
            if i:
                time.sleep(1)

            if os.path.exists(lock_dir):
                try:
                    if os.path.isfile(lock_file):
                        with open(lock_file, 'r') as f:
                            pid = f.read(8)
                        if not pid:
                            if int(os.path.getmtime(lock_file)) + timeout < int(time.time()):
                                info("Cache lock file exists, but is empty. Cleaning up")
                                os.remove(lock_file)
                                os.rmdir(lock_dir)
                        elif int(pid) != os.getpid() and self.pid_exists(pid):
                            info("Cache lock file exists and process %s is alive." % pid)
                        else:
                            info("Cache lock file exists, but %s is dead. Cleaning up" % pid)
                            os.remove(lock_file)
                            os.rmdir(lock_dir)
                    else:
                        os.rmdir(lock_dir)
                    continue
                except (OSError) as e:
                    continue
            else:
                try:
                    os.mkdir(lock_dir)
                    with open(lock_file, 'w') as f:
                        info("Writing cache lock file %s for pid %s" % (lock_file, os.getpid()))
                        f.write(str(os.getpid()))
                        f.flush()
                        os.fsync(f)
                    break
                except (OSError) as e:
                    ## Windows:
                    ##   <type 'exceptions.WindowsError'>    17 [Error 183] Cannot create a file when that file already exists: 'testing'
                    ##   or when concurrent:                 13 WindowsError(5, 'Access is denied')
                    ## Linux:    <type 'exceptions.OSError'> 17 [Errno 17] File exists: 'testing'
                    ##   or when concurrent & virtualbox     71, OSError(71, 'Protocol error')
                    ##   or when full:                       28, OSError(28, 'No space left on device')
                    if e.errno in (17,13,71,28):
                        continue
                    else:
                        raise e
        else:
            error("Exceeded 5 minutes limit while waiting for other process to finish caching")
        return True

    def cache_unlock(self, url):
        cpath = self.url2cachedir(url)
        if not cpath:
            return False

        lock_dir = os.path.join(cpath, '.lock')
        lock_file = os.path.join(lock_dir, 'pid')
        try:
            if os.path.exists(lock_dir):
                if os.path.isfile(lock_file):
                    try:
                        with open(lock_file, 'r') as f:
                            pid = f.read(8)
                        if int(pid) != os.getpid():
                            error("Cache lock file exists with a different pid (\"%s\" vs \"%s\")" % (pid, os.getpid()))
                        else:
                            info("Cache lock file exists with my pid (\"%s\"). Cleaning up." % (pid))
                    except OSError:
                        error("Unable to unlock cache dir \"%s\"" % (cpath))
                    os.remove(lock_file)
                os.rmdir(lock_dir)
        except (OSError) as e:
            pass
        return True

    @contextmanager
    def cache_lock_held(self, url):
        self.cache_lock(url)
        try:
            yield
        finally:
            self.cache_unlock(url)

    def pid_exists(self, pid):
        try:
            os.kill(int(pid), 0)
        except OSError as err:
            if err.errno == errno.ESRCH:
                return False
            elif err.errno == errno.EPERM:
                return True
            else:
                raise err
        else:
            return True

    def can_update(self, clean, clean_deps):
        err = None
        if (self.is_local or self.url is None) and not clean_deps:
            err = (
                "Preserving local library \"%s\" in \"%s\".\nPlease publish this library to a remote URL to be able to restore it at any time."
                "You can use --ignore switch to ignore all local libraries and update only the published ones.\n"
                "You can also use --clean-deps switch to remove all local libraries. WARNING: This action cannot be undone." % (self.name, self.path))
        elif not clean and self.dirty():
            err = (
                "Uncommitted changes in \"%s\" in \"%s\".\nPlease discard or stash them first and then retry update.\n"
                "You can also use --clean switch to discard all uncommitted changes. WARNING: This action cannot be undone." % (self.name, self.path))
        elif not clean_deps and self.outgoing():
            err = (
                "Unpublished changes in \"%s\" in \"%s\".\nPlease publish them first using the \"publish\" command.\n"
                "You can also use --clean-deps to discard all local commits and replace the library with the one included in this revision. WARNING: This action cannot be undone." % (self.name, self.path))

        return (False, err) if err else (True, "OK")

    def check_repo(self, show_warning=None):
        err = None
        if not os.path.isdir(self.path):
            err = (
                "Library reference \"%s\" points to non-existing library in \"%s\"\n"
                "You can use \"mbed deploy\" to import the missing libraries.\n"
                "You can also use \"mbed sync\" to synchronize and remove all invalid library references." % (os.path.basename(self.lib), self.path))
        elif not self.isrepo(self.path):
            err = (
                "Library reference \"%s\" points to a folder \"%s\", which is not a valid repository.\n"
                "You can remove the conflicting folder manually and use \"mbed deploy\" to import the missing libraries\n"
                "You can also remove library reference \"%s\" and use \"mbed sync\" again." % (os.path.basename(self.lib), self.path, self.lib))

        if err:
            if show_warning:
                warning(err)
            else:
                error(err, 1)
            return False
        return True


# Program class, acts code base root
class Program(object):
    path = None
    name = None
    is_cwd = False
    is_repo = False
    is_classic = False
    build_dir = "BUILD"

    def __init__(self, path=None, print_warning=False):
        path = os.path.abspath(path or getcwd())
        self.path = path
        self.is_cwd = True

        while cd(path):
            tpath = path
            if os.path.isfile(os.path.join(path, Cfg.file)):
                self.path = path
                self.is_cwd = False
                break

            path = os.path.split(path)[0]
            if tpath == path:       # Reached root.
                break

        self.name = os.path.basename(self.path)
        self.is_classic = os.path.isfile(os.path.join(self.path, 'mbed.bld'))

        # is_cwd flag indicates that current dir is assumed to be root, not root repo
        if self.is_cwd and print_warning:
            warning(
                "Could not find mbed program in current path \"%s\".\n"
                "You can fix this by calling \"mbed new .\" in the root of your program." % self.path)

    def get_cfg(self, *args, **kwargs):
        return Cfg(self.path).get(*args, **kwargs) or Global().get_cfg(*args, **kwargs)

    def set_cfg(self, *args, **kwargs):
        return Cfg(self.path).set(*args, **kwargs)

    def list_cfg(self, *args, **kwargs):
        return Cfg(self.path).list(*args, **kwargs)

    def set_root(self):
        return self.set_cfg('ROOT', '.')

    def unset_root(self, path=None):
        fl = os.path.join(path or self.path, Cfg.file)
        if os.path.isfile(fl):
            os.remove(fl)

    # Gets mbed OS dir (unified)
    def get_os_dir(self):
        if self.get_cfg('MBED_OS_DIR', None) is not None:
            return self.get_cfg('MBED_OS_DIR')
        if os.path.isdir(os.path.join(self.path, 'mbed-os')):
            return os.path.join(self.path, 'mbed-os')
        elif self.name == 'mbed-os':
            return self.path
        else:
            return None

    def get_mbedlib_dir(self):
        if os.path.isdir(os.path.join(self.path, 'mbed')):
            return os.path.join(self.path, 'mbed')
        else:
            return None

    # Gets mbed tools dir (unified)
    def get_tools_dir(self):
        paths = []
        # mbed-os dir identified and tools is a sub dir
        mbed_os_path = self.get_os_dir()
        if mbed_os_path:
            paths.append([mbed_os_path, 'tools'])
            paths.append([mbed_os_path, 'core', 'tools'])
        # mbed-os not identified but tools found under cwd/tools
        paths.append([self.path, 'tools'])
        paths.append([self.path, 'core', 'tools'])
        # mbed Classic deployed tools
        paths.append([self.path, '.temp', 'tools'])

        return self._find_file_paths(paths, 'make.py')

    def get_requirements(self):
        paths = []
        mbed_os_path = self.get_os_dir()
        if mbed_os_path:
            paths.append([mbed_os_path, 'tools'])
            paths.append([mbed_os_path])
        # mbed-os not identified but tools found under cwd/tools
        paths.append([self.path, 'tools'])
        # mbed Classic deployed tools
        paths.append([self.path, '.temp', 'tools'])
        # current dir
        paths.append([self.path])

        return self._find_file_paths(paths, 'requirements.txt')

    def _find_file_paths(self, paths, fl):
        for p in paths:
            path = os.path.join(*p)
            if os.path.isdir(path) and os.path.isfile(os.path.join(path, fl)):
                return os.path.join(path)
        return None

    def requirements_contains(self, library_name):
        req_path = self.get_requirements()
        if not req_path:
            return False

        req_file = 'requirements.txt'
        with open(os.path.join(req_path, req_file), 'r') as f:
            return library_name in f.read()

    def check_requirements(self, require_install=False):
        skip_requirements = self.get_cfg("NO_REQUIREMENTS", False)
        if skip_requirements:
            action("Skipping installed requirements check due to configuration flag.")
            return True

        req_path = self.get_requirements() or self.path
        if not req_path:
            return False

        req_file = 'requirements.txt'
        missing = []
        try:
            with open(os.path.join(req_path, req_file), 'r') as f:
                pkg_list = pquery([python_cmd, '-m', 'pip', 'list', '-l']) or ""
                installed_packages = [re.sub(r'-', '_', pkg.split()[0].lower()) for pkg in pkg_list.splitlines() if len(pkg.split())]
                for line in f.read().splitlines():
                    pkg = re.sub(r'-', '_', re.sub(r'^([^<>=@]+).*$', r'\1', line).lower())
                    pkg = re.sub(r'^(git|hg|svn|bzr)\+(.*)/([\w.-]+?)(\.(git|hg))?$', r'\3', pkg)
                    pkg = re.sub(r'\[([\w]+)\]', '', pkg)
                    line_parts = line.split(";")

                    for line_part in line_parts[1:]:
                        if not _eval_environment_marker(line_part):
                            break
                    else:
                        if not pkg in installed_packages:
                            missing.append(pkg)

                if missing and install_requirements and require_install:
                    try:
                        action("Auto-installing missing Python modules (%s)..." % ', '.join(missing))
                        pquery([python_cmd, '-m', 'pip', 'install', '-q', '-r', os.path.join(req_path, req_file)])
                        missing = []
                    except ProcessException:
                        pass
        except (IOError, ImportError, OSError):
            pass

        if missing:
            msg = (
                "Missing Python modules were not auto-installed.\n"
                "The Mbed OS tools in this program require the following Python modules: %s\n"
                "You can install all missing modules by running \"pip install -r %s\" in \"%s\"" % (', '.join(missing), req_file, req_path))
            if os.name == 'posix' and platform.system() == 'Darwin':
                msg += "\nOn Mac you might have to install packages as your user by adding the \"--user\" flag"
            elif os.name == 'posix':
                msg += "\nOn Posix systems (Linux, etc) you might have to switch to superuser account or use \"sudo\""

            if require_install:
                error(msg, 1)
            else:
                warning(msg)

        return True


    # Routines after cloning mbed-os
    def post_action(self, check_reqs=True):
        mbed_tools_path = self.get_tools_dir()

        if not mbed_tools_path and self.is_classic:
            self.add_tools(os.path.join(self.path, '.temp'))
            mbed_tools_path = self.get_tools_dir()

        if not mbed_tools_path:
            warning("Cannot find the mbed tools directory in \"%s\"" % self.path)
            return False

        if (not os.path.isfile(os.path.join(self.path, 'mbed_settings.py')) and
                os.path.isfile(os.path.join(mbed_tools_path, 'default_settings.py'))):
            shutil.copy(os.path.join(mbed_tools_path, 'default_settings.py'), os.path.join(self.path, 'mbed_settings.py'))

        if check_reqs:
            self.check_requirements(True)

    def add_tools(self, path):
        if not os.path.exists(path):
            os.mkdir(path)
        with cd(path):
            tools_dir = 'tools'
            if not os.path.exists(tools_dir):
                try:
                    action("Couldn't find build tools in your program. Downloading the mbed 2.0 SDK tools...")
                    repo = Repo.fromurl(mbed_sdk_tools_url)
                    repo.clone(mbed_sdk_tools_url, tools_dir)
                except Exception:
                    if os.path.exists(tools_dir):
                        rmtree_readonly(tools_dir)
                    error("An error occurred while cloning the mbed SDK tools from \"%s\"" % mbed_sdk_tools_url)

    def update_tools(self, path):
        tools_dir = 'tools'
        if os.path.exists(os.path.join(path, tools_dir)):
            with cd(os.path.join(path, tools_dir)):
                try:
                    action("Updating the mbed 2.0 SDK tools...")
                    repo = Repo.fromrepo()
                    repo.update()
                except Exception:
                    error("An error occurred while update the mbed SDK tools from \"%s\"" % mbed_sdk_tools_url)

    def get_tools(self):
        mbed_tools_path = self.get_tools_dir()
        if not mbed_tools_path:
            error("The mbed tools were not found in \"%s\". \nYou can run \"mbed deploy\" to install dependencies and tools." % self.path, -1)
        return mbed_tools_path

    def get_env(self):
        env = os.environ.copy()
        env['PYTHONPATH'] = os.path.abspath(self.path)
        compilers = ['ARM', 'GCC_ARM', 'IAR', 'ARMC6']
        for c in compilers:
            if self.get_cfg(c+'_PATH'):
                env['MBED_'+c+'_PATH'] = self.get_cfg(c+'_PATH')
        config_options = ['COLOR', 'CLOUD_SDK_API_KEY', 'CLOUD_SDK_HOST']
        for opt in config_options:
            if self.get_cfg(opt):
                env['MBED_' + opt] = self.get_cfg(opt)

        return env

    def get_target(self, target=None):
        target_cfg = self.get_cfg('TARGET')
        target = target if target else target_cfg

        if not target or (target.lower() == 'detect' or target.lower() == 'auto'):
            detected = self.detect_single_target()
            if detected:
                target = detected['name']

        return target

    def get_toolchain(self, toolchain=None):
        toolchain_cfg = self.get_cfg('TOOLCHAIN')
        tchain = toolchain if toolchain else toolchain_cfg
        if tchain is None:
            error("Please specify toolchain using the -t switch or set default toolchain using command \"mbed toolchain\"", 1)
        return tchain

    def get_profile(self, profile=None):
        if not profile:
            profile_cfg = self.get_cfg('PROFILE')
            if profile_cfg:
                profile = [profile_cfg]
        return profile

    def set_defaults(self, target=None, toolchain=None):
        if target and not self.get_cfg('TARGET'):
            self.set_cfg('TARGET', target)
        if toolchain and not self.get_cfg('TOOLCHAIN'):
            self.set_cfg('TOOLCHAIN', toolchain)

    def get_macros(self, more_macros=None):
        macros = more_macros or []
        # backwards compatibility with old MACROS.txt file
        if os.path.isfile('MACROS.txt'):
            with open('MACROS.txt') as f:
                macros.extend(f.read().splitlines())
        return macros


    def ignore_build_dir(self):
        build_path = os.path.join(self.path, self.build_dir)
        if not os.path.exists(build_path):
            os.mkdir(build_path)
        if not os.path.exists(os.path.join(build_path, '.mbedignore')):
            try:
                with open(os.path.join(build_path, '.mbedignore'), 'w') as f:
                    f.write('*\n')
            except IOError:
                error("Unable to write build ignore file in \"%s\"" % os.path.join(build_path, '.mbedignore'), 1)

    def detect_single_target(self, info=None):
        targets = self.get_detected_targets()
        if targets == False:
            error("The target detection requires that the 'mbed-ls' python module is installed.\nYou can install mbed-ls by running \"pip install mbed-ls\".", 1)
        elif len(targets) > 1:
            error("Multiple targets were detected.\nOnly 1 target board should be connected to your system.", 1)
        elif len(targets) == 0:
            error("No targets were detected.\nPlease make sure a target board is connected to this system.\nAlternatively, you can specify target using the -m switch or set default target using command \"mbed target\"", 1)
        else:
            action("Detected \"%s\" connected to \"%s\" and using com port \"%s\"" % (targets[0]['name'], targets[0]['mount'], targets[0]['serial']))
            info = targets[0]

        if info is None:
            error("The detected target doesn't support Mass Storage Device capability (MSD)", 1)
        return info

    def get_detected_targets(self):
        targets = []
        import mbed_os_tools.detect
        oldError = None
        if os.name == 'nt':
            oldError = ctypes.windll.kernel32.SetErrorMode(1) # Disable Windows error box temporarily. note that SEM_FAILCRITICALERRORS = 1
        mbeds = mbed_os_tools.detect.create()
        detect_muts_list = mbeds.list_mbeds()
        if os.name == 'nt':
            ctypes.windll.kernel32.SetErrorMode(oldError)

        for mut in detect_muts_list:
            targets.append({
                'id': mut['target_id'], 'name': mut['platform_name'],
                'mount': mut['mount_point'], 'serial': mut['serial_port']
            })

        return targets


# Global class used for global config
class Global(object):
    def __init__(self):
        self.path = os.path.join(os.path.expanduser("~"), '.mbed')
        if not os.path.exists(self.path):
            try:
                os.mkdir(self.path)
            except (IOError, OSError):
                pass

    def get_cfg(self, *args, **kwargs):
        return Cfg(self.path).get(*args, **kwargs)

    def set_cfg(self, *args, **kwargs):
        return Cfg(self.path).set(*args, **kwargs)

    def list_cfg(self, *args, **kwargs):
        return Cfg(self.path).list(*args, **kwargs)

    def cache_cfg(self, *args, **kwargs):
        return Cfg(self.path).cache(*args, **kwargs)


# Cfg classed used for handling the config backend
class Cfg(object):
    path = None
    file = ".mbed"

    def __init__(self, path):
        self.path = path

    # Sets config value
    def set(self, var, val):
        retval = False
        if not re.match(r'^([\w+-]+)$', var):
            error("%s is invalid config variable name" % var)

        fl = os.path.join(self.path, self.file)
        try:
            with open(fl) as f:
                lines = f.read().splitlines()
        except (IOError, OSError):
            lines = []

        for line in lines:
            m = re.match(r'^([\w+-]+)\=(.*)$', line)
            if m and m.group(1) == var:
                lines.remove(line)

        if not val is None:
            lines += [var+"="+val]

        try:
            with open(fl, 'w') as f:
                f.write('\n'.join(lines) + '\n')
                retval = True
        except (IOError, OSError):
            warning("Unable to write config file %s" % fl)
        return retval

    # Gets config value
    def get(self, var, default_val=None):
        fl = os.path.join(self.path, self.file)
        try:
            with open(fl) as f:
                lines = f.read().splitlines()
        except (IOError, OSError):
            lines = []

        for line in lines:
            m = re.match(r'^([\w+-]+)\=(.*)$', line)
            if m and m.group(1) == var:
                return m.group(2)
        return default_val

    # Get all config var/values pairs
    def list(self):
        fl = os.path.join(self.path, self.file)
        try:
            with open(fl) as f:
                lines = f.read().splitlines()
        except (IOError, OSError):
            lines = []

        vars = {}
        for line in lines:
            m = re.match(r'^([\w+-]+)\=(.*)$', line)
            if m and m.group(1) and m.group(1) != 'ROOT':
                vars[m.group(1)] = m.group(2)
        return vars

    # Get cache configuration
    def cache(self):
        cache_cfg = self.get('CACHE', 'enabled')
        cache_val = 'enabled' if cache_repositories and cache_cfg and cache_cfg != 'none' and cache_cfg != 'off' and cache_cfg != 'disabled' else 'disabled'

        cache_dir_cfg = self.get('CACHE_DIR', None)
        loc = cache_dir_cfg if cache_dir_cfg != 'default' else (cache_cfg if (cache_cfg and cache_cfg != 'on' and cache_cfg != 'off' and cache_cfg != 'none' and cache_cfg != 'enabled' and cache_cfg != 'disabled') else None)
        cache_base = loc or Global().path
        return {'cache': cache_val, 'cache_base': cache_base, 'cache_dir': os.path.join(cache_base, 'mbed-cache')}


def formaturl(url, format="default"):
    url = "%s" % url
    m = re.match(regex_mbed_url, url)
    if m:
        if format == "http": # mbed urls doesn't convert to ssh - only http and https
            url = 'http://%s%s%s/%s' % (m.group(2) or '', m.group(6), m.group(8) or '', m.group(9))
        else:
            url = 'https://%s%s%s/%s' % (m.group(2) or '', m.group(6), m.group(8) or '', m.group(9))
    else:
        m = re.match(regex_repo_url, url)
        if m and m.group(1) == '': # no protocol specified, probably ssh string like "git@github.com:ARMmbed/mbed-os.git"
            url = 'ssh://%s%s%s/%s' % (m.group(2) or 'git@', m.group(6), m.group(7) or '', m.group(8)) # convert to common ssh URL-like format
            m = re.match(regex_repo_url, url)

        if m:
            if format == "ssh":
                url = 'ssh://%s%s%s/%s' % (m.group(2) or 'git@', m.group(6), m.group(7) or '', m.group(8))
            elif format == "http":
                url = 'http://%s%s%s/%s' % (m.group(2) if (m.group(2) and (m.group(5) or m.group(3) != 'git')) else '', m.group(6), m.group(7) or '', m.group(8))
            elif format == "https":
                url = 'https://%s%s%s/%s' % (m.group(2) if (m.group(2) and (m.group(5) or m.group(3) != 'git')) else '', m.group(6), m.group(7) or '', m.group(8))
    return url

# Wrapper for the MbedTermnal functionality
def mbed_sterm(port, baudrate=9600, echo=True, reset=False, sterm=False):
    try:
        from .mbed_terminal import MbedTerminal
    except(ValueError): # relative import fails on Python 2 in non-package mode
        from mbed_terminal import MbedTerminal
    except (IOError, ImportError, OSError):
        error("The serial terminal functionality requires that the 'mbed-terminal' python module is installed.\nYou can install mbed-terminal by running 'pip install mbed-terminal'.", 1)

    mbed_serial = MbedTerminal(port, baudrate=baudrate, echo=echo)
    if mbed_serial.serial:
        if reset:
            if not mbed_serial.reset():
                error("Unable to reset the target board connected to your system.\nThis might be caused by an old interface firmware.\nPlease check the board page for new firmware.", 1)
        if sterm:
            # Some boards will reset the COM port after SendBreak, e.g. STLink based
            if not mbed_serial.serial.is_open:
                mbed_serial = MbedTerminal(port, baudrate=baudrate, echo=echo)

            try:
                if not mbed_serial.terminal():
                    error("Unable to open serial terminal.\nMost likely your pyserial is dated and needs to be updated with 'pip install --upgrade pyserial")
            except:
                pass


# Subparser handling
parser = argparse.ArgumentParser(prog='mbed',
    description="Command-line code management tool for ARM mbed OS - http://www.mbed.com\nversion %s\n\nUse \"mbed <command> -h|--help\" for detailed help.\nOnline manual and guide available at https://github.com/ARMmbed/mbed-cli" % ver,
    formatter_class=argparse.RawTextHelpFormatter)
subparsers = parser.add_subparsers(title="Commands", metavar="           ")
parser.add_argument("--version", action="store_true", dest="version", help="print version number and exit")
subcommands = {}

# Process handling
def subcommand(name, *args, **kwargs):
    def __subcommand(command):
        aliases = []
        if not kwargs.get('description') and kwargs.get('help'):
            kwargs['description'] = kwargs['help']
        if not kwargs.get('formatter_class'):
            kwargs['formatter_class'] = argparse.RawDescriptionHelpFormatter
        if kwargs.get('hidden_aliases'):
            aliases = kwargs.get('hidden_aliases')
            del kwargs['hidden_aliases']

        subparser = subparsers.add_parser(name, **kwargs)
        subcommands[name] = subparser

        for arg in args:
            arg = dict(arg)
            opt = arg['name']
            del arg['name']

            if isinstance(opt, basestring):
                subparser.add_argument(opt, **arg)
            else:
                subparser.add_argument(*opt, **arg)

        subparser.add_argument("-v", "--verbose", action="store_true", dest="verbose", help="Verbose diagnostic output")
        subparser.add_argument("-vv", "--very_verbose", action="store_true", dest="very_verbose", help="Very verbose diagnostic output")

        def thunk(parsed_args):
            argv = [arg['dest'] if 'dest' in arg else arg['name'] for arg in args]
            argv = [(arg if isinstance(arg, basestring) else arg[-1]).strip('-').replace('-', '_')
                    for arg in argv]
            argv = {arg: vars(parsed_args)[arg] for arg in argv
                    if vars(parsed_args)[arg] is not None}

            return command(**argv)

        subparser.set_defaults(command=thunk)

        # set hidden aliases if any
        for alias in aliases:
            subparsers._name_parser_map[alias] = subparsers._name_parser_map[name]

        return command
    return __subcommand


# New command
@subcommand('new',
    dict(name='name', help='Destination name or path'),
    dict(name='--scm', nargs='?', help='Source control management. Currently supported: %s. Default: git' % ', '.join([s.name for s in scms.values()])),
    dict(name='--program', action='store_true', help='Force creation of an mbed program. Default: auto.'),
    dict(name='--library', action='store_true', help='Force creation of an mbed library. Default: auto.'),
    dict(name='--mbedlib', action='store_true', help='Add the mbed library instead of mbed-os into the program.'),
    dict(name='--create-only', action='store_true', help='Only create a program, do not import mbed-os or mbed library.'),
    dict(name='--depth', nargs='?', help='Number of revisions to fetch the mbed OS repository when creating new program. Default: all revisions.'),
    dict(name='--protocol', nargs='?', help='Transport protocol when fetching the mbed OS repository when creating new program. Supported: https, http, ssh, git. Default: inferred from URL.'),
    dict(name='--offline', action='store_true', help='Offline mode will force the use of locally cached repositories and prevent requests to remote repositories.'),
    dict(name='--no-requirements', action='store_true', help='Disables checking for and installing any requirements.'),
    help='Create new mbed program or library',
    description=(
        "Creates a new mbed program if executed within a non-program location.\n"
        "Alternatively creates an mbed library if executed within an existing program.\n"
        "When creating new program, the latest mbed-os release will be downloaded/added\n unless --create-only is specified.\n"
        "Supported source control management: git, hg"))
def new(name, scm='git', program=False, library=False, mbedlib=False, create_only=False, depth=None, protocol=None, offline=False, no_requirements=False):
    global cwd_root
    offline_warning(offline)

    d_path = os.path.abspath(name or getcwd())
    p_path = os.path.dirname(d_path)
    d_type = None
    if program and library:
        error("Cannot use both --program and --library options.", 1)
    elif program or library:
        d_type = 'library' if library else 'program'
    else:
        pp = Program(p_path)
        pd = Program(d_path)
        if pp.path == pd.path:
            d_type = 'library' if os.path.abspath(p_path) != os.path.abspath(d_path) else 'program'
        else:
            d_type = 'library' if not pp.is_cwd and os.path.abspath(p_path) != os.path.abspath(d_path) else 'program'

    if os.path.exists(d_path):
        p = Program(d_path)
        if (d_type == 'program' and not p.is_cwd) or (d_type == 'library' and Repo.isrepo(d_path)):
            error("A %s with name \"%s\" already exists." % (d_type, os.path.basename(d_path)), 1)

    if scm and scm != 'none':
        if os.path.isdir(d_path) and Repo.isrepo(d_path):
            repo = Repo.fromrepo(d_path)
            if repo.scm.name != scm:
                error("A repository already exists in \"%s\" based on %s. Please select a different name or location." % (d_path, scm), 1)
        else:
            repo_scm = [s for s in scms.values() if s.name == scm.lower()]
            if not repo_scm:
                error(
                    "You have specified invalid source control management system\n"
                    "Please specify one of the following SCMs: %s" % ', '.join([s.name for s in scms.values()]), 1)
            repo_scm[0].init(d_path)
    else:
        scm = 'folder'
        if not os.path.isdir(d_path):
            os.mkdir(d_path)

    if len(os.listdir(d_path)) > 1:
        warning("Directory \"%s\" is not empty." % d_path)

    action("Creating new %s \"%s\" (%s)" % (d_type, os.path.basename(d_path), scm))
    p = Program(d_path)
    if d_type == 'program':
        # This helps sub-commands to display relative paths to the created program
        cwd_root = os.path.abspath(d_path)
        p.path = cwd_root
        p.set_root()
        if not create_only and not p.get_os_dir() and not p.get_mbedlib_dir():
            url = mbed_lib_url+'#tip' if mbedlib else mbed_os_url+'#latest'
            d = 'mbed' if mbedlib else 'mbed-os'
            try:
                with cd(d_path):
                    add(url, path=d, depth=depth, protocol=protocol, top=False)
                    create_default_mbed_app()
            except Exception as e:
                if os.path.isdir(os.path.join(d_path, d)):
                    rmtree_readonly(os.path.join(d_path, d))
                raise e
        if d_path:
            os.chdir(d_path)
    else:
        p.unset_root(d_path)
        with cd(p_path):
            sync()

    Program(d_path).post_action(not no_requirements)


# Import command
@subcommand('import',
    dict(name='url', help='URL of the program'),
    dict(name='path', nargs='?', help='Destination name or path. Default: current directory.'),
    dict(name=['-I', '--ignore'], action='store_true', help='Ignore errors related to cloning and updating.'),
    dict(name='--depth', nargs='?', help='Number of revisions to fetch from the remote repository. Default: all revisions.'),
    dict(name='--protocol', nargs='?', help='Transport protocol for the source control management. Supported: https, http, ssh, git. Default: inferred from URL.'),
    dict(name='--insecure', action='store_true', help='Allow insecure repository URLs. By default mbed CLI imports only "safe" URLs, e.g. based on standard ports - 80, 443 and 22. This option enables the use of arbitrary URLs/ports.'),
    dict(name='--offline', action='store_true', help='Offline mode will force the use of locally cached repositories and prevent requests to remote repositories.'),
    dict(name='--no-requirements', action='store_true', help='Disables checking for and installing any requirements.'),
    hidden_aliases=['im', 'imp'],
    help='Import program from URL',
    description=(
        "Imports mbed program and its dependencies from a source control based URL\n"
        "(GitHub, Bitbucket, mbed.org) into the current directory or specified\npath.\n"
        "Use \"mbed add <URL>\" to add a library into an existing program."))
def import_(url, path=None, ignore=False, depth=None, protocol=None, insecure=False, offline=False, no_requirements=False, top=True):
    global cwd_root
    offline_warning(offline, top)

    # translate 'mbed-os' to https://github.com/ARMmbed/mbed-os
    orig_url = url
    if not Repo.isurl(url) and not os.path.exists(url):
        url = mbed_base_url+'/'+url

    repo = Repo.fromurl(url, path)
    if top:
        p = Program(path)
        if p and not p.is_cwd:
            error("Cannot import program in the specified location \"%s\" because it's already part of a program \"%s\".\n"
                  "Please change your working directory to a different location or use \"mbed add\" to import the URL as a library." % (os.path.abspath(repo.path), p.name), 1)

    protocol = protocol or Program().get_cfg('PROTOCOL')
    insecure = insecure or Program().get_cfg('INSECURE')

    if not insecure and Repo.isinsecure(url):
        error("Cannot import \"%s\" in \"%s\" due to arbitrary service schema/port in the repository URL.\nRepositories are usually hosted on service port 443 (https), 80 (http), or 22 (ssh).\nYou can use the \"--insecure\" switch to enable the use of arbitrary repository URLs." % (repo.url, repo.path), 255)

    if os.path.isdir(repo.path) and len(os.listdir(repo.path)) > 1:
        error("Directory \"%s\" is not empty. Please ensure that the destination folder is empty." % repo.path, 1)

    if not Repo.isurl(orig_url) and os.path.exists(orig_url):
        warning("Importing from a local folder \"%s\", not from a URL" % orig_url)

    text = "Importing program" if top else "Adding library"
    action("%s \"%s\" from \"%s\"%s" % (text, relpath(cwd_root, repo.path), formaturl(repo.url, protocol), ' at '+(repo.revtype(repo.rev))))
    if repo.clone(repo.url, repo.path, rev=repo.rev, depth=depth, protocol=protocol, offline=offline):
        with cd(repo.path):
            Program(repo.path).set_root()
            try:
                if repo.rev and repo.getrev() != repo.rev:
                    repo.checkout(repo.rev, True)
            except ProcessException as e:
                err = "Unable to update \"%s\" to %s" % (repo.name, repo.revtype(repo.rev))
                if depth:
                    err = err + ("\nThe --depth option might prevent fetching the whole revision tree and checking out %s." % (repo.revtype(repo.rev)))
                if ignore:
                    warning(err)
                else:
                    error(err, e.args[0])
    else:
        err = "Unable to clone repository (%s)" % url
        if ignore:
            warning(err)
        else:
            error(err, 1)

    repo.sync()

    if top: # This helps sub-commands to display relative paths to the imported program
        cwd_root = repo.path

    with cd(repo.path):
        deploy(ignore=ignore, depth=depth, protocol=protocol, insecure=insecure, offline=offline, top=False)

    if top:
        Program(repo.path).post_action(not no_requirements)


# Add library command
@subcommand('add',
    dict(name='url', help='URL of the library'),
    dict(name='path', nargs='?', help='Destination name or path. Default: current folder.'),
    dict(name=['-I', '--ignore'], action='store_true', help='Ignore errors related to cloning and updating.'),
    dict(name='--depth', nargs='?', help='Number of revisions to fetch from the remote repository. Default: all revisions.'),
    dict(name='--protocol', nargs='?', help='Transport protocol for the source control management. Supported: https, http, ssh, git. Default: inferred from URL.'),
    dict(name='--insecure', action='store_true', help='Allow insecure repository URLs. By default mbed CLI imports only "safe" URLs, e.g. based on standard ports - 80, 443 and 22. This option enables the use of arbitrary URLs/ports.'),
    dict(name='--offline', action='store_true', help='Offline mode will force the use of locally cached repositories and prevent requests to remote repositories.'),
    dict(name='--no-requirements', action='store_true', help='Disables checking for and installing any requirements.'),
    hidden_aliases=['ad'],
    help='Add library from URL',
    description=(
        "Adds mbed library and its dependencies from a source control based URL\n"
        "(GitHub, Bitbucket, mbed.org) into an existing program.\n"
        "Use \"mbed import <URL>\" to import as a program"))
def add(url, path=None, ignore=False, depth=None, protocol=None, insecure=False, offline=False, no_requirements=False, top=True):
    offline_warning(offline, top)

    repo = Repo.fromrepo()
    lib = Repo.fromurl(url, path)
    import_(lib.fullurl, lib.path, ignore=ignore, depth=depth, protocol=protocol, insecure=insecure, offline=offline, top=False)
    repo.ignore(relpath(repo.path, lib.path))
    lib.sync()

    lib.write()
    repo.add(lib.lib)

    if top:
        Program(repo.path).post_action(not no_requirements)


# Remove library
@subcommand('remove',
    dict(name='path', help='Local library name or path'),
    help='Remove library',
    hidden_aliases=['rm', 'rem'],
    description=(
        "Remove specified library, its dependencies and references from the current\n"
        "You can re-add the library from its URL via \"mbed add <URL>\"."))
def remove(path):
    repo = Repo.fromrepo()
    if not Repo.isrepo(path):
        error("Could not find library in path (%s)" % path, 1)

    lib = Repo.fromrepo(path)
    action("Removing library \"%s\" in \"%s\"" % (lib.name, lib.path))
    rmtree_readonly(lib.path)
    repo.remove(lib.lib)
    repo.unignore(relpath(repo.path, lib.path))


# Deploy command
@subcommand('deploy',
    dict(name=['-I', '--ignore'], action='store_true', help='Ignore errors related to cloning and updating.'),
    dict(name='--depth', nargs='?', help='Number of revisions to fetch from the remote repository. Default: all revisions.'),
    dict(name='--protocol', nargs='?', help='Transport protocol for the source control management. Supported: https, http, ssh, git. Default: inferred from URL.'),
    dict(name='--insecure', action='store_true', help='Allow insecure repository URLs. By default mbed CLI imports only "safe" URLs, e.g. based on standard ports - 80, 443 and 22. This option enables the use of arbitrary URLs/ports.'),
    dict(name='--offline', action='store_true', help='Offline mode will force the use of locally cached repositories and prevent requests to remote repositories.'),
    dict(name='--no-requirements', action='store_true', help='Disables checking for and installing any requirements.'),
    help='Find and add missing libraries',
    description=(
        "Import missing dependencies in an existing program or library.\n"
        "Hint: Use \"mbed import <URL>\" and \"mbed add <URL>\" instead of cloning\n"
        "manually and then running \"mbed deploy\""))
def deploy(ignore=False, depth=None, protocol=None, insecure=False, offline=False, no_requirements=False, top=True):
    offline_warning(offline, top)

    repo = Repo.fromrepo()
    repo.ignores()
    for lib in repo.libs:
        if os.path.isdir(lib.path):
            if lib.check_repo():
                with cd(lib.path):
                    update(lib.rev, ignore=ignore, depth=depth, protocol=protocol, insecure=insecure, offline=offline, top=False)
        else:
            import_(lib.fullurl, lib.path, ignore=ignore, depth=depth, protocol=protocol, insecure=insecure, offline=offline, top=False)
            repo.ignore(relpath(repo.path, lib.path))

    if top:
        program = Program(repo.path)
        program.post_action(not no_requirements)
        if program.is_classic:
            program.update_tools(os.path.join(getcwd(), '.temp'))

# Publish command
@subcommand('publish',
    dict(name=['-A', '--all'], dest='all_refs', action='store_true', help='Publish all branches, including new ones. Default: push only the current branch.'),
    dict(name=['-M', '--message'], dest='msg', type=str, nargs='?', help='Commit message. Default: prompts for commit message.'),
    hidden_aliases=['pub'],
    help='Publish program or library',
    description=(
        "Publishes the current program or library and all dependencies to their\nassociated remote repository URLs.\n"
        "This command performs various consistency checks for local uncommitted changes\n"
        "and unpublished revisions and encourages to commit/push them.\n"
        "Online guide about collaboration is available at:\n"
        "www.mbed.com/collab_guide"))
def publish(all_refs=None, msg=None, top=True):
    if top:
        action("Checking for local modifications...")

    repo = Repo.fromrepo()
    if repo.is_local:
        error(
            "%s \"%s\" in \"%s\" is a local repository.\nPlease associate it with a remote repository URL before attempting to publish.\n"
            "Read more about publishing local repositories here:\nhttps://github.com/ARMmbed/mbed-cli/#publishing-local-program-or-library" % ("Program" if top else "Library", repo.name, repo.path), 1)

    for lib in repo.libs:
        if lib.check_repo():
            with cd(lib.path):
                progress()
                publish(all_refs, msg=msg, top=False)

    sync(recursive=False)

    if repo.dirty():
        action("Uncommitted changes in %s \"%s\" in \"%s\"" % (repo.pathtype(repo.path), repo.name, repo.path))
        if msg:
            repo.commit(msg)
        else:
            if sys.version_info[0] == 3:
                input('Press enter to commit and publish: ')
            else:
                raw_input('Press enter to commit and publish: ')
            repo.commit()

    try:
        outgoing = repo.outgoing()
        if outgoing > 0:
            action("Pushing local repository \"%s\" to remote \"%s\"" % (repo.name, repo.url))
            repo.publish(all_refs)
        else:
            if top:
                action("Nothing to publish to the remote repository (the source tree is unmodified)")
    except ProcessException as e:
        if e.args[0] != 1:
            raise e


# Update command
@subcommand('update',
    dict(name='rev', nargs='?', help='Revision, tag or branch'),
    dict(name=['-C', '--clean'], action='store_true', help='Perform a clean update and discard all modified or untracked files. WARNING: This action cannot be undone. Use with caution.'),
    dict(name='--clean-files', action='store_true', help='Remove any local ignored files. Requires \'--clean\'. WARNING: This will wipe all local uncommitted, untracked and ignored files. Use with extreme caution.'),
    dict(name='--clean-deps', action='store_true', help='Remove any local libraries and also libraries containing uncommitted or unpublished changes. Requires \'--clean\'. WARNING: This action cannot be undone. Use with caution.'),
    dict(name=['-I', '--ignore'], action='store_true', help='Ignore errors related to unpublished libraries, unpublished or uncommitted changes, and attempt to update from associated remote repository URLs.'),
    dict(name='--depth', nargs='?', help='Number of revisions to fetch from the remote repository. Default: all revisions.'),
    dict(name='--protocol', nargs='?', help='Transport protocol for the source control management. Supported: https, http, ssh, git. Default: inferred from URL.'),
    dict(name='--insecure', action='store_true', help='Allow insecure repository URLs. By default mbed CLI imports only "safe" URLs, e.g. based on standard ports - 80, 443 and 22. This option enables the use of arbitrary URLs/ports.'),
    dict(name='--offline', action='store_true', help='Offline mode will force the use of locally cached repositories and prevent requests to remote repositories.'),
    dict(name=['-l', '--latest-deps'], action='store_true', help='Update all dependencies to the latest revision of their current branch. WARNING: Ignores lib files'),
    dict(name='--no-requirements', action='store_true', help='Disables checking for and installing any requirements.'),
    hidden_aliases=['up'],
    help='Update to branch, tag, revision or latest',
    description=(
        "Updates the current program or library and its dependencies to specified\nbranch, tag or revision.\n"
        "Alternatively fetches from associated remote repository URL and updates to the\n"
        "latest revision in the current branch."))
def update(rev=None, clean=False, clean_files=False, clean_deps=False, ignore=False, depth=None, protocol=None, insecure=False, offline=False, latest_deps=False, no_requirements=False, top=True):
    offline_warning(offline, top)

    if top and clean:
        sync()

    cwd_type = Repo.pathtype(cwd_root)
    cwd_dest = "program" if cwd_type == "directory" else "library"

    repo = Repo.fromrepo()
    # A copy of repo containing the .lib layout before updating
    repo_orig = Repo.fromrepo()

    if top and not rev and repo.isdetached():
        error(
            "This %s is in detached HEAD state, and you won't be able to receive updates from the remote repository until you either checkout a branch or create a new one.\n"
            "You can checkout a branch using \"%s checkout <branch_name>\" command before running \"mbed update\"." % (cwd_type, repo.scm.name), 1)

    if repo.isdetached() and latest_deps:
        warning(
            "The repo %s is in detached HEAD state, and you won't be able to receive updates from the remote repository until you either checkout a branch or create a new one.\n"
            "You can checkout a branch using \"%s checkout <branch_name>\" command before running \"mbed update\"." % (repo.name, repo.scm.name))

    if repo.is_local and not repo.rev:
        action("Skipping unpublished empty %s \"%s\"" % (
            cwd_type if top else cwd_dest,
            os.path.basename(repo.path) if top else relpath(cwd_root, repo.path)))
    else:
        # Fetch from remote repo
        action("Updating %s \"%s\" to %s" % (
            cwd_type if top else cwd_dest,
            os.path.basename(repo.path) if top else relpath(cwd_root, repo.path),
            repo.revtype(rev)))

        try:
            repo.update(rev, clean, clean_files, offline or repo.is_local)
        except ProcessException as e:
            err = "Unable to update \"%s\" to %s" % (repo.name, repo.revtype(rev))
            if offline:
                err = err + "\nThis might be caused by offline mode ('--offline' used). You should try without offline mode."
            elif depth:
                err = err + ("\nThis might be caused by the '--depth' option, which prevents fetching the whole revision history." % (repo.revtype(repo.rev)))
            if ignore:
                warning(err)
            else:
                error(err, e.args[0])

        repo.rm_untracked()
        if top and cwd_type == 'library':
            repo.sync()
            repo.write()

    # Compare library references (.lib) before and after update, and remove libraries that do not have references in the current revision
    for lib in repo_orig.libs:
        if not os.path.isfile(lib.lib) and os.path.isdir(lib.path): # Library reference doesn't exist in the new revision. Will try to remove library to reproduce original structure
            with cd(lib.path):
                lib_repo = Repo.fromrepo(lib.path)
                gc, msg = lib_repo.can_update(clean, clean_deps)
            if gc:
                action("Removing library \"%s\" (obsolete)" % (relpath(cwd_root, lib.path)))
                rmtree_readonly(lib.path)
                repo.unignore(relpath(repo.path, lib.path))
            else:
                if ignore:
                    warning(msg)
                else:
                    error(msg, 1)

    # Reinitialize repo.libs() to reflect the library files after update
    repo.sync()

    # Recheck libraries as their urls might have changed
    for lib in repo.libs:
        if os.path.isdir(lib.path) and Repo.isrepo(lib.path):
            lib_repo = Repo.fromrepo(lib.path)
            if (not lib.is_local and not lib_repo.is_local and
                formaturl(lib.url, 'https') != formaturl(lib_repo.url, 'https')): # Repository URL has changed
                with cd(lib.path):
                    gc, msg = lib_repo.can_update(clean, clean_deps)
                if gc:
                    action("Removing library \"%s\" (changed URL). Will add from new URL." % (relpath(cwd_root, lib.path)))
                    rmtree_readonly(lib.path)
                    repo.unignore(relpath(repo.path, lib.path))
                else:
                    if ignore:
                        warning(msg)
                    else:
                        error(msg, 1)

    # Import missing repos and update to revs
    for lib in repo.libs:
        if not os.path.isdir(lib.path):
            import_(lib.fullurl, lib.path, ignore=ignore, depth=depth, protocol=protocol, insecure=insecure, offline=offline, top=False)
            repo.ignore(relpath(repo.path, lib.path))
        else:
            with cd(lib.path):
                update(None if latest_deps else lib.rev, clean=clean, clean_files=clean_files, clean_deps=clean_deps, ignore=ignore, depth=depth, protocol=protocol, insecure=insecure, offline=offline, latest_deps=latest_deps, top=False)

    if top:
        program = Program(repo.path)
        program.set_root()
        program.post_action(not no_requirements)
        if program.is_classic:
            program.update_tools(os.path.join(getcwd(), '.temp'))


# Synch command
@subcommand('sync',
    help='Synchronize library references\n\n',
    description=(
        "Synchronizes all library and dependency references (.lib files) in the\n"
        "current program or library.\n"
        "Note that this will remove all invalid library references."))
def sync(recursive=True, keep_refs=False, top=True):
    if top and recursive:
        action("Synchronizing dependency references...")

    repo = Repo.fromrepo()
    repo.ignores()

    for lib in repo.libs:
        if os.path.isdir(lib.path):
            lib.check_repo()
            lib.sync()
            lib.write()
            repo.ignore(relpath(repo.path, lib.path))
            progress()
        else:
            if not keep_refs:
                action("Removing reference \"%s\" -> \"%s\"" % (lib.name, lib.fullurl))
                repo.remove(lib.lib)
                repo.unignore(relpath(repo.path, lib.path))

    for root, dirs, files in os.walk(repo.path):
        dirs[:] = [d for d in dirs  if not d.startswith('.')]
        files[:] = [f for f in files if not f.startswith('.')]

        for d in list(dirs):
            if not Repo.isrepo(os.path.join(root, d)):
                continue

            lib = Repo.fromrepo(os.path.join(root, d))
            if os.path.isfile(lib.lib):
                dirs.remove(d)
                continue

            dirs.remove(d)
            lib.write()
            repo.ignore(relpath(repo.path, lib.path))
            repo.add(lib.lib)
            progress()

    repo.sync()

    if recursive:
        for lib in repo.libs:
            if lib.check_repo():
                with cd(lib.path):
                    sync(keep_refs=keep_refs, top=False)

    # Update the .lib reference in the parent repository
    cwd_type = Repo.pathtype(cwd_root)
    if top and cwd_type == "library":
        repo = Repo.fromrepo()
        repo.write()


# List command
@subcommand('ls',
    dict(name=['-a', '--all'], dest='detailed', action='store_true', help='List repository URL and revision pairs'),
    dict(name=['-I', '--ignore'], action='store_true', help='Ignore errors related to missing libraries.'),
    help='View dependency tree',
    description=(
        "View the dependency tree of the current program or library."))
def list_(detailed=False, prefix='', p_path=None, ignore=False):
    repo = Repo.fromrepo()

    print("%s (%s)" % (prefix + (relpath(p_path, repo.path) if p_path else repo.name), ((repo.url + ('#' + str(repo.rev)[:12] if repo.rev else '') if detailed else repo.revtype(repo.rev, fmt=6)) or 'no revision')))

    for i, lib in enumerate(sorted(repo.libs, key=lambda l: l.path)):
        nprefix = (prefix[:-3] + ('|  ' if prefix[-3] == '|' else '   ')) if prefix else ''
        nprefix += '|- ' if i < len(repo.libs)-1 else '`- '

        if lib.check_repo(ignore):
            with cd(lib.path):
                list_(detailed, nprefix, repo.path, ignore=ignore)


# Command release for cross-SCM release tags of repositories
@subcommand('releases',
    dict(name=['-a', '--all'], dest='detailed', action='store_true', help='Show revision hashes'),
    dict(name=['-u', '--unstable'], dest='unstable', action='store_true', help='Show unstable releases well, e.g. release candidates, alphas, betas, etc'),
    dict(name=['-r', '--recursive'], action='store_true', help='Show release tags for all libraries and sub-libraries as well'),
    hidden_aliases=['rel', 'rels'],
    help='Show release tags',
    description=(
        "Show release tags for the current program or library."))
def releases_(detailed=False, unstable=False, recursive=False, prefix='', p_path=None):
    repo = Repo.fromrepo()
    tags = repo.gettags()
    revtags = repo.gettags(repo.rev) if repo.rev and len(tags) else [] # associated tags with current commit
    regex_rels = regex_rels_all if unstable else regex_rels_official

    # Generate list of tags
    rels = []
    for tag in tags:
        if re.match(regex_rels, tag[1]):
            rels.append(tag[1] + " %s%s" % ('#' + tag[0] if detailed else "", " <- current" if tag[1] in revtags else ""))

    # Print header
    print("%s (%s)" % (prefix + (relpath(p_path, repo.path) if p_path else repo.name), ((repo.url + ('#' + str(repo.rev)[:12] if repo.rev else '') if detailed else repo.revtype(repo.rev, fmt=6)) or 'no revision')))

    # Print list of tags
    rprefix = (prefix[:-3] + ('|  ' if prefix[-3] == '|' else '   ')) if recursive and prefix else ''
    rprefix += '| ' if recursive and len(repo.libs) > 1 else '  '
    if len(rels):
        for rel in rels:
            print(rprefix + '* ' + rel)
    else:
        print(rprefix + 'No release tags detected')

    if recursive:
        for i, lib in enumerate(sorted(repo.libs, key=lambda l: l.path)):
            nprefix = (prefix[:-3] + ('|  ' if prefix[-3] == '|' else '   ')) if prefix else ''
            nprefix += '|- ' if i < len(repo.libs)-1 else '`- '

            if lib.check_repo():
                with cd(lib.path):
                    releases_(detailed, unstable, recursive, nprefix, repo.path)


# Command status for cross-SCM status of repositories
@subcommand('status',
    dict(name=['-I', '--ignore'], action='store_true', help='Ignore errors related to missing libraries.'),
    hidden_aliases=['st', 'stat'],
    help='Show version control status\n\n',
    description=(
        "Show uncommitted changes a program or library and its dependencies."))
def status_(ignore=False):
    repo = Repo.fromrepo()
    if repo.dirty():
        action("Status for \"%s\":" % repo.name)
        log(repo.status()+"\n")

    for lib in repo.libs:
        if lib.check_repo(ignore):
            with cd(lib.path):
                status_(ignore)


# Helper function for compile and test subcommands
def _safe_append_profile_to_build_path(build_path, profile):
    if profile:
        # profile is (or can be) a list, so just get the first element
        if not isinstance(profile, basestring):
            profile = profile[0]

    if profile:
        profile_name_without_extension = os.path.splitext(os.path.basename(profile))[0].upper()
        build_path += '-' + profile_name_without_extension

    return build_path


# Compile command which invokes the mbed OS native build system
@subcommand('compile',
    dict(name=['-t', '--toolchain'], help='Compile toolchain. Example: ARM, GCC_ARM, IAR'),
    dict(name=['-m', '--target'], help='Compile target MCU. Example: K64F, NUCLEO_F401RE, NRF51822...'),
    dict(name=['-D', '--macro'], action='append', help='Add a macro definition'),
    dict(name=['--profile'], action='append', help='Path of a build profile configuration file (or name of Mbed OS profile). Default: develop'),
    dict(name='--library', dest='compile_library', action='store_true', help='Compile the current program or library as a static library.'),
    dict(name='--config', dest='compile_config', action='store_true', help='Show run-time compile configuration'),
    dict(name='--prefix', dest='config_prefix', action='append', help='Restrict listing to parameters that have this prefix'),
    dict(name='--source', action='append', help='Source directory. Default: . (current dir)'),
    dict(name='--build', help='Build directory. Default: build/'),
    dict(name=['-c', '--clean'], action='store_true', help='Clean the build directory before compiling'),
    dict(name=['-f', '--flash'], action='store_true', help='Flash the built firmware onto a connected target.'),
    dict(name=['--sterm'], action='store_true', help='Open serial terminal after compiling. Can be chained with --flash'),
    dict(name=['--baudrate'], help='Serial terminal communication baudrate. Default: 9600'),
    dict(name=['-N', '--artifact-name'], help='Name of the built program or library'),
    dict(name=['-S', '--supported'], dest='supported', const=True, choices=["matrix", "toolchains", "targets"], nargs="?", help='Shows supported matrix of targets and toolchains'),
    dict(name='--app-config', dest="app_config", help="Path of an application configuration file. Default is to look for \"mbed_app.json\"."),
    help='Compile code using the mbed build tools',
    description="Compile this program using the mbed build tools.")
def compile_(toolchain=None, target=None, macro=False, profile=False,
             compile_library=False, compile_config=False, config_prefix=None,
             source=False, build=False, clean=False, flash=False, sterm=False,
             baudrate=9600, artifact_name=None, supported=False, app_config=None):
    # Gather remaining arguments
    args = remainder
    # Find the root of the program
    program = Program(getcwd(), True)
    program.check_requirements()
    # Remember the original path. this is needed for compiling only the libraries and tests for the current folder.
    orig_path = getcwd()
    orig_target = target

    with cd(program.path):
        tools_dir = os.path.abspath(program.get_tools())

    # Prepare environment variables
    env = program.get_env()

    if not source or len(source) == 0:
        source = [os.path.relpath(program.path, orig_path)]

    if supported:
        popen([python_cmd, '-u', os.path.join(tools_dir, 'make.py')]
              + (['-S', supported] if (supported is not True) else ['-S']) + (['-v'] if very_verbose else [])
              + (['--app-config', app_config] if app_config else [])
              + args,
              env=env)
        return

    target = program.get_target(target)
    tchain = program.get_toolchain(toolchain)
    macros = program.get_macros(macro)
    profile = program.get_profile(profile)

    if compile_config:
        # Compile configuration
        popen([python_cmd, os.path.join(tools_dir, 'get_config.py')]
              + ['-t', tchain, '-m', target]
              + list(chain.from_iterable(zip(repeat('--source'), source)))
              + (['-v'] if verbose else [])
              + (['--app-config', app_config] if app_config else [])
              + (list(chain.from_iterable(zip(repeat('--prefix'), config_prefix))) if config_prefix else []),
              env=env)
    else:
        # If the user hasn't supplied a build directory, ignore the default build directory
        if not build:
            program.ignore_build_dir()

        build_path = build

        if compile_library:
            # Compile as a library (current dir is default)
            if not build_path:
                build_path = os.path.join(os.path.relpath(program.path, orig_path), program.build_dir, 'libraries', os.path.basename(orig_path), target.upper(), tchain.upper())
                build_path = _safe_append_profile_to_build_path(build_path, profile)

            popen([python_cmd, '-u', os.path.join(tools_dir, 'build.py')]
                  + list(chain.from_iterable(zip(repeat('-D'), macros)))
                  + ['-t', tchain, '-m', target]
                  + list(chain.from_iterable(zip(repeat('--profile'), profile or [])))
                  + list(chain.from_iterable(zip(repeat('--source'), source)))
                  + ['--build', build_path]
                  + (['-c'] if clean else [])
                  + (['--artifact-name', artifact_name] if artifact_name else [])
                  + (['-v'] if verbose else [])
                  + args,
                  env=env)
        else:
            # Compile as application (root is default)
            if not build_path:
                build_path = os.path.join(os.path.relpath(program.path, orig_path), program.build_dir, target.upper(), tchain.upper())
                build_path = _safe_append_profile_to_build_path(build_path, profile)

            popen([python_cmd, '-u', os.path.join(tools_dir, 'make.py')]
                  + list(chain.from_iterable(zip(repeat('-D'), macros)))
                  + ['-t', tchain, '-m', target]
                  + list(chain.from_iterable(zip(repeat('--profile'), profile or [])))
                  + list(chain.from_iterable(zip(repeat('--source'), source)))
                  + ['--build', build_path]
                  + (['-c'] if clean else [])
                  + (['--artifact-name', artifact_name] if artifact_name else [])
                  + (['--app-config', app_config] if app_config else [])
                  + (['-v'] if verbose else [])
                  + args,
                  env=env)

            if flash or sterm:
                baudrate = baudrate or program.get_cfg('TERM_BAUDRATE', 9600)

                try:
                    from mbed_host_tests.host_tests_toolbox import flash_dev
                except (IOError, ImportError, OSError):
                    error("The '-f/--flash' option requires that the 'mbed-greentea' python module is installed.\nYou can install mbed-greentea by running \"%s -m pip install mbed-greentea\"." % python_cmd, 1)

                connected = False
                # Convert Dual Core target name to mbedls target name
                if target.upper().endswith('_CM4') or target.upper().endswith('_CM7'):
                    target = target[:-4]
                    action('Target to detect: %s' % target)
                targets = program.get_detected_targets()
                if targets:
                    for _target in targets:
                        if _target['name'] is None:
                            continue
                        elif _target['name'].upper() == target.upper():
                            connected = _target

                            # apply new firmware
                            if flash:
                                fw_name = artifact_name if artifact_name else program.name
                                fw_fbase = os.path.join(build_path, fw_name)
                                fw_file = fw_fbase + ('.hex' if os.path.exists(fw_fbase+'.hex') else '.bin')
                                if not os.path.exists(fw_file):
                                    error("Build program file (firmware) not found \"%s\"" % fw_file, 1)
                                if not flash_dev(connected['mount'], fw_file, program_cycle_s=4):
                                    error("Unable to flash the target board connected to your system.", 1)

                            # reset board and/or connect to serial port
                            if flash or sterm:
                                mbed_sterm(connected['serial'], baudrate=baudrate, reset=flash, sterm=sterm)

                if not connected:
                    error("The target board you compiled for is not connected to your system.\nPlease reconnect it and retry the last command.", 1)

    program.set_defaults(target=orig_target, toolchain=tchain)


# Test command
@subcommand('test',
    dict(name=['-t', '--toolchain'], help='Compile toolchain. Example: ARM, GCC_ARM, IAR'),
    dict(name=['-m', '--target'], help='Compile target MCU. Example: K64F, NUCLEO_F401RE, NRF51822...'),
    dict(name=['-D', '--macro'], action='append', help='Add a macro definition'),
    dict(name='--compile-list', dest='compile_list', action='store_true',
         help='List all tests that can be built'),
    dict(name='--run-list', dest='run_list', action='store_true', help='List all built tests that can be ran'),
    dict(name='--compile', dest='compile_only', action='store_true', help='Only compile tests'),
    dict(name='--run', dest='run_only', action='store_true', help='Only run tests'),
    dict(name=['-n', '--tests-by-name'], dest='tests_by_name',
         help='Limit the tests to a list (ex. test1,test2,test3)'),
    dict(name='--source', action='append', help='Source directory. Default: . (current dir)'),
    dict(name='--build', help='Build directory. Default: build/'),
    dict(name=['--profile'], action='append',
         help='Path of a build profile configuration file (or name of Mbed OS profile). Default: develop'),
    dict(name=['-c', '--clean'], action='store_true', help='Clean the build directory before compiling'),
    dict(name='--test-spec', dest="test_spec", help="Path used for the test spec file used when building and running tests (the default path is the build directory)"),
    dict(name='--app-config', dest="app_config", help="Path of an application configuration file. Default is to look for \"mbed_app.json\""),
    dict(name='--test-config', dest="test_config", help="Path or mbed OS keyword of a test configuration file. Example: ethernet, odin_wifi, or path/to/config.json"),
    dict(name='--coverage', choices=['html', 'xml', 'both'], help='Generate code coverage report for unit tests'),
    dict(name='--valgrind', action='store_true', help='Use Valgrind for unit tests'),
    dict(name=['--make-program'], choices=['gmake', 'make', 'mingw32-make', 'ninja'], help='Which make program to use for unit tests'),
    dict(name=['--generator'], choices=['Unix Makefiles', 'MinGW Makefiles', 'Ninja'], help='Which CMake generator to use for unit tests'),
    dict(name='--new', help='generate files for a new unit test', metavar="FILEPATH"),
    dict(name=['-r', '--regex'], help='Run unit tests matching regular expression'),
    dict(name=['--unittests'], action="store_true", help='Run only unit tests'),
    dict(name='--build-data', dest="build_data", default=None, help="Dump build_data to this file"),
    dict(name=['--greentea'], dest="greentea", action='store_true', default=False, help="Run Greentea tests"),
    dict(name=['--icetea'], dest="icetea", action='store_true', default=False,
         help="Run Icetea tests. If used without --greentea flag then run only icetea tests."),
    help='Find, build and run tests',
    description="Find, build, and run tests in a program and libraries")
def test_(toolchain=None, target=None, macro=False, compile_list=False, run_list=False, compile_only=False, run_only=False, tests_by_name=None, source=False, profile=False, build=False, clean=False, test_spec=None, app_config=None, test_config=None, coverage=None, valgrind=None, make_program=None, new=None, generator=None, regex=None, unittests=None, build_data=None, greentea=None, icetea=None):

    # Default behaviour is to run only greentea tests
    if not (greentea or icetea or unittests):
        greentea = True
        icetea = False
        unittests = False

    # Gather remaining arguments
    args = remainder
    # Find the root of the program
    program = Program(getcwd(), True)
    program.check_requirements()
    # Check if current Mbed OS support icetea
    icetea_supported = program.requirements_contains('icetea')

    # Disable icetea if not supported
    if not icetea_supported:
        icetea = False
    if icetea and not os.path.exists(os.path.join(getcwd(), 'TEST_APPS')):
        error("Cannot run icetea tests. Current folder does not contain TEST_APPS folder.", 1)

    # Save original working directory
    orig_path = getcwd()
    orig_target = target

    macros = program.get_macros(macro)
    macros.append("MBED_TEST_MODE")
    tools_dir = program.get_tools()
    build_and_run_tests = not compile_list and not run_list and not compile_only and not run_only

    # Prepare environment variables
    env = program.get_env()

    with cd(program.path):
        if unittests:
            mbed_os_dir = program.get_os_dir()
            if mbed_os_dir is None:
                error("No Mbed OS directory found.")
            unittests_dir = os.path.join(mbed_os_dir, "UNITTESTS")

            tool = os.path.join(unittests_dir, "mbed_unittest.py")
            if os.path.exists(tool):
                # Setup the build path if not specified
                build_path = build
                if not build_path:
                    build_path = os.path.join(os.path.relpath(program.path, orig_path), program.build_dir, 'unittests')

                # Run unit testing tools
                popen([python_cmd, tool]
                        + (["--compile"] if compile_only else [])
                        + (["--run"] if run_only else [])
                        + (["--clean"] if clean else [])
                        + (["--debug"] if profile and "debug" in profile else [])
                        + (["--coverage", coverage] if coverage else [])
                        + (["--valgrind", valgrind] if valgrind else [])
                        + (["--make-program", make_program] if make_program else [])
                        + (["--generator", generator] if generator else [])
                        + (["--regex", regex] if regex else [])
                        + ["--build", build_path]
                        + (["--new", new] if new else [])
                        + (["--verbose"] if verbose else [])
                        + remainder,
                        env=env)
            else:
                warning("Unit testing is not supported with this Mbed OS version.")
        elif icetea or greentea:
            target = program.get_target(target)
            tchain = program.get_toolchain(toolchain)

            icetea_command_base = [python_cmd, '-u', os.path.join(tools_dir, 'run_icetea.py')] \
                  + (['-m', target, '-t', tchain]) \
                  + (['-n', tests_by_name] if tests_by_name else []) \
                  + (['-v'] if verbose else [])

            # Setup the source path if not specified
            if not source or len(source) == 0:
                source = [os.path.relpath(program.path, orig_path)]

            # Setup the build path if not specified
            build_path = build
            if not build_path:
                build_path = os.path.join(os.path.relpath(program.path, orig_path), program.build_dir, 'tests', target.upper(), tchain.upper())
                build_path = _safe_append_profile_to_build_path(build_path, profile)

            if test_spec:
                # Preserve path to given test spec
                test_spec = os.path.relpath(os.path.join(orig_path, test_spec), program.path)
            else:
                # Create the path to the test spec file
                test_spec = os.path.join(build_path, 'test_spec.json')

            if build_data:
                # Preserve path to given build data
                build_data = os.path.relpath(os.path.join(orig_path, build_data), program.path)
            elif icetea_supported:
                # Build data needed only if icetea is supported
                # Create the path to the test build data file
                build_data = os.path.join(build_path, 'build_data.json')

            if compile_list and greentea:
                popen([python_cmd, '-u', os.path.join(tools_dir, 'test.py'), '--list']
                      + list(chain.from_iterable(list(zip(repeat('--profile'), profile or []))))
                      + ['-t', tchain, '-m', target]
                      + list(chain.from_iterable(zip(repeat('--source'), source)))
                      + (['-n', tests_by_name] if tests_by_name else [])
                      + (['-v'] if verbose else [])
                      + (['--app-config', app_config] if app_config else [])
                      + (['--test-config', test_config] if test_config else [])
                      + (['--greentea'] if icetea_supported and greentea else [])
                      + args,
                      env=env)

            if compile_list and icetea:
                popen(icetea_command_base + ['--compile-list'])

            if compile_only or build_and_run_tests:
                # Add icetea binaries in compile list
                tests_by_name_temp = tests_by_name if tests_by_name else ''
                if icetea:
                    proc = popen(icetea_command_base + ['--application-list'], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT)
                    applications_to_add = proc.stdout.read().decode("utf-8")
                    # Filter right row in case that debugger print something there
                    if applications_to_add and 'TEST_APPS-' in applications_to_add:
                        applications_to_add = list(filter(lambda x: 'TEST_APPS-' in x, applications_to_add.split('\n')))[0]
                        if tests_by_name_temp:
                            tests_by_name_temp += ','
                        tests_by_name_temp += applications_to_add

                # If the user hasn't supplied a build directory, ignore the default build directory
                if not build:
                    program.ignore_build_dir()

                popen([python_cmd, '-u', os.path.join(tools_dir, 'test.py')]
                      + list(chain.from_iterable(zip(repeat('-D'), macros)))
                      + list(chain.from_iterable(zip(repeat('--profile'), profile or [])))
                      + ['-t', tchain, '-m', target]
                      + (['-c'] if clean else [])
                      + list(chain.from_iterable(zip(repeat('--source'), source)))
                      + ['--build', build_path]
                      + ['--test-spec', test_spec]
                      + (['--build-data', build_data] if build_data else [])
                      + (['-n', tests_by_name_temp] if tests_by_name_temp else [])
                      + (['-v'] if verbose else [])
                      + (['--app-config', app_config] if app_config else [])
                      + (['--test-config', test_config] if test_config else [])
                      + (['--icetea'] if icetea_supported and icetea else [])
                      + (['--greentea'] if icetea_supported and greentea else [])
                      + args,
                      env=env)

            # Greentea tests
            if greentea:
                if run_list:
                    popen(['mbedgt', '--test-spec', test_spec, '--list']
                          + (['-n', tests_by_name] if tests_by_name else [])
                          + (['-V'] if verbose else [])
                          + args,
                          env=env)

                if run_only or build_and_run_tests:
                    popen(['mbedgt', '--test-spec', test_spec]
                          + (['-n', tests_by_name] if tests_by_name else [])
                          + (['-V'] if verbose else [])
                          + args,
                          env=env)

            # Icetea tests
            if icetea:
                icetea_command = icetea_command_base \
                             + ['--build-data', build_data] \
                             + ['--test-suite', os.path.join(build_path, 'test_suite.json')]

                if run_list:
                    popen(icetea_command + ['--run-list'])

                if run_only or build_and_run_tests:
                    popen(icetea_command)

            program.set_defaults(target=orig_target, toolchain=tchain)


# device management commands
@subcommand('device-management',
    dict(name=['-t', '--toolchain'], help='Toolchain used for mbed compile'),
    dict(name=['-m', '--target'], help='Target used for compile for target MCU. Example: K64F, NUCLEO_F401RE, NRF51822...'),
    dict(name=['--profile'], help=""),
    dict(name='--build', help='Build directory. Default: build/'),
    dict(name='--source', action='append', help='Source directory. Default: . (current dir)'),
    help='device management subcommand',
    add_help=False,
    hidden_aliases=['dev-mgmt', 'dm'],
    description=("Manage Device with Pelion"))
def dev_mgmt(toolchain=None, target=None, source=False, profile=False, build=False):
    orig_path = getcwd()
    program = Program(getcwd(), True)
    program.check_requirements(True)
    with cd(program.path):
        tools_dir = program.get_tools()

    script = os.path.join(tools_dir, 'device_management.py')
    if not os.path.exists(script):
        error('device management is not supported by this version of Mbed OS. Please upgrade.')


    target = target or program.get_cfg("TARGET")
    toolchain = toolchain or program.get_cfg("TOOLCHAIN")

    if build:
        build_path = build
    elif (not build) and target and toolchain:
        build_path = os.path.join(
            os.path.relpath(program.path, orig_path),
            program.build_dir,
            target.upper(),
            toolchain.upper()
        )
        build_path = _safe_append_profile_to_build_path(build_path, profile)
    else:
        build_path = None

    args = remainder
    if args[0] in ('update', 'create'):
        args += (['--toolchain', toolchain] if toolchain else [])
        args += (['--mcu', target] if target else [])
        args += (['--build', build_path] if build_path else [])
    env = program.get_env()
    popen([python_cmd, '-u', script]
          + args
          + list(chain.from_iterable(zip(repeat('--source'), source or []))),
          env=env)

# Export command
@subcommand('export',
    dict(name=['-i', '--ide'], help='IDE to create project files for. Example: UVISION4, UVISION5, GCC_ARM, IAR, COIDE'),
    dict(name=['-m', '--target'], help='Export for target MCU. Example: K64F, NUCLEO_F401RE, NRF51822...'),
    dict(name='--source', action='append', help='Source directory. Default: . (current dir)'),
    dict(name=['--profile'], action='append', help='Path of a build profile configuration file (or name of Mbed OS profile). Default: debug'),
    dict(name=['-c', '--clean'], action='store_true', help='Clean the build directory before compiling'),
    dict(name=['-S', '--supported'], dest='supported', const=True, choices=['matrix', 'ides'], nargs='?', help='Shows supported matrix of targets and toolchains'),
    dict(name='--app-config', dest="app_config", help="Path of an application configuration file. Default is to look for \"mbed_app.json\""),
    dict(name='--no-requirements', action='store_true', help='Disables checking for and installing any requirements.'),
    help='Generate an IDE project',
    description=(
        "Generate IDE project files for the current program."))
def export(ide=None, target=None, source=False, profile=["debug"], clean=False, supported=False, app_config=None, no_requirements=False):
    # Gather remaining arguments
    args = remainder
    # Find the root of the program
    program = Program(getcwd(), True)
    if not no_requirements:
        program.check_requirements()
    # Remember the original path. this is needed for compiling only the libraries and tests for the current folder.
    orig_path = getcwd()
    orig_target = target
    # Change directories to the program root to use mbed OS tools
    with cd(program.path):
        tools_dir = program.get_tools()

    # Prepare environment variables
    env = program.get_env()

    if supported:
        popen([python_cmd, '-u', os.path.join(tools_dir, 'project.py')]
              + (['-S', supported] if (supported is not True) else ['-S']) + (['-v'] if very_verbose else []),
              env=env)
        return

    target = program.get_target(target)
    macros = program.get_macros()

    if not ide:
        error('Please specify ide using the -i switch')

    if not source or len(source) == 0:
        source = [os.path.relpath(program.path, orig_path)]

    program.ignore_build_dir()

    popen([python_cmd, '-u', os.path.join(tools_dir, 'project.py')]
          + list(chain.from_iterable(zip(repeat('-D'), macros)))
          + ['-i', ide.lower()]
          + ['-m', target]
          + list(chain.from_iterable(zip(repeat('--profile'), profile or [])))
          + (['-c'] if clean else [])
          + list(chain.from_iterable(zip(repeat('--source'), source)))
          + (['--app-config', app_config] if app_config else [])
          + args,
          env=env)

    program.set_defaults(target=orig_target)


# Detect command
@subcommand('detect',
    hidden_aliases=['det'],
    help='Detect connected Mbed targets/boards\n\n',
    description=(
        "Detect Mbed targets/boards connected to this system and show supported\n"
        "toolchain matrix."))
def detect():
    # Gather remaining arguments
    args = remainder
    # Find the root of the program
    program = Program(getcwd(), False)
    program.check_requirements()
    # Change directories to the program root to use mbed OS tools
    with cd(program.path):
        tools_dir = program.get_tools_dir()

    if tools_dir:
        # Prepare environment variables
        env = program.get_env()

        try:
            popen([python_cmd, '-u', os.path.join(tools_dir, 'detect_targets.py')]
                  + args,
                  env=env)
        except ProcessException as e:
            error("Failed to correctly detect all targets (run with '-vv' for more information)")
            if very_verbose:
                error(str(e))
    else:
        warning("The mbed-os tools were not found in \"%s\". \nLimited information will be shown about connected targets/boards" % program.path)
        targets = program.get_detected_targets()
        if targets:
            unknown_found = False
            for target in targets:
                if target['name'] is None:
                    unknown_found = True
                    action("Detected unknown target connected to \"%s\" and using com port \"%s\"" % (target['mount'], target['serial']))
                else:
                    action("Detected \"%s\" connected to \"%s\" and using com port \"%s\"" % (target['name'], target['mount'], target['serial']))

            if unknown_found:
                warning("If you're developing a new target, you can mock the device to continue your development. "
                    "Use \"mbedls --mock ID:NAME\" to do so. See \"mbedls --help\" for more information.")


# Serial terminal command
@subcommand('sterm',
    dict(name=['-m', '--target'], help='Compile target MCU. Example: K64F, NUCLEO_F401RE, NRF51822...'),
    dict(name=['-p', '--port'], help='Communication port. Default: auto-detect. Specifying this will also ignore the -m/--target option above.'),
    dict(name=['-b', '--baudrate'], help='Communication baudrate. Default: 9600'),
    dict(name=['-e', '--echo'], help='Switch local echo on/off. Default: on'),
    dict(name=['-r', '--reset'], action='store_true', help='Reset the targets (via SendBreak) before opening terminal.'),
    hidden_aliases=['term'],
    help='Open serial terminal to connected target.\n\n',
    description=(
        "Open serial terminal to connected target (usually board), or connect to a user-specified COM port\n"))
def sterm(target=None, port=None, baudrate=None, echo=None, reset=False, sterm=True):
    # Gather remaining arguments
    args = remainder
    # Find the root of the program
    program = Program(getcwd(), False)

    port = port or program.get_cfg('TERM_PORT', None)
    baudrate = baudrate or program.get_cfg('TERM_BAUDRATE', 9600)
    echo = echo or program.get_cfg('TERM_ECHO', 'on')

    if port:
        action("Opening serial terminal to the specified COM port \"%s\"" % port)
        mbed_sterm(port, baudrate=baudrate, echo=echo, reset=reset, sterm=sterm)
    else:
        action("Detecting connected targets/boards to your system...")
        targets = program.get_detected_targets()
        if not targets:
            error("Couldn't detect connected targets/boards to your system.\nYou can manually specify COM port via the '--port' option.", 1)

        connected = False
        for _target in targets:
            if _target['name'] is None:
                continue
            elif not target or _target['name'].upper() == target.upper():
                connected = _target
                action("Opening serial terminal to \"%s\"" % _target['name'])
                mbed_sterm(_target['serial'], baudrate=baudrate, echo=echo, reset=reset, sterm=sterm)

        if not connected:
            error("The specified \"%s\" target/board is not connected to your system" % target)

# Generic config command
@subcommand('config',
    dict(name='var', nargs='?', help='Variable name. E.g. "target", "toolchain", "protocol"'),
    dict(name='value', nargs='?', help='Value. Will show the currently set default value for a variable if not specified.'),
    dict(name=['-G', '--global'], dest='global_cfg', action='store_true', help='Use global settings, not local'),
    dict(name=['-U', '--unset'], dest='unset', action='store_true', help='Unset the specified variable.'),
    dict(name=['-L', '--list'], dest='list_config', action='store_true', help='List mbed tool configuration. Not to be confused with compile configuration, e.g. "mbed compile --config".'),
    hidden_aliases=['cfg', 'conf'],
    help='Tool configuration',
    description=(
        "Gets, sets or unsets mbed tool configuration options.\n"
        "Options can be global (via the --global switch) or local (per program)\n"
        "Global options are always overridden by local/program options.\n"
        "Currently supported options: target, toolchain, protocol, depth, cache, profile, color"))
def config_(var=None, value=None, global_cfg=False, unset=False, list_config=False):
    name = var
    var = str(var).upper()

    if list_config:
        g = Global()
        g_vars = g.list_cfg().items()
        action("Global config:")
        if g_vars:
            for v in g_vars:
                log("%s=%s\n" % (v[0], v[1]))
        else:
            log("No global configuration is set\n")
        log("\n")

        p = Program(getcwd())
        action("Local config (%s):" % p.path)
        if not p.is_cwd:
            p_vars = p.list_cfg().items()
            if p_vars:
                for v in p_vars:
                    log("%s=%s\n" % (v[0], v[1]))
            else:
                log("No local configuration is set\n")
        else:
            log("Couldn't find valid mbed program in %s\n" % p.path)

    elif name:
        if global_cfg:
            # Global configuration
            g = Global()
            if unset:
                g.set_cfg(var, None)
                action('Unset global %s' % name)
            elif value:
                g.set_cfg(var, value)
                action('%s now set as global %s' % (value, name))
            else:
                value = g.get_cfg(var)
                action(('%s' % value) if value else 'No global %s set' % name)
        else:
            # Find the root of the program
            program = Program(getcwd())
            if program.is_cwd and not var == 'ROOT':
                error(
                    "Could not find mbed program in current path \"%s\".\n"
                    "Change the current directory to a valid mbed program, set the current directory as an mbed program with \"mbed config root .\", or use the '--global' option to set global configuration." % program.path)
            with cd(program.path):
                if unset:
                    if program.set_cfg(var, None):
                        action('Unset default %s in program "%s"' % (name, program.name))
                elif value:
                    if program.set_cfg(var, value):
                        action('%s now set as default %s in program "%s"' % (value, name, program.name))
                else:
                    value = program.get_cfg(var)
                    if value:
                        action('%s' % value)
                    else:
                        action('No default %s set in program "%s"' % (name, program.name))
                        error("run with -h for detailed usage help")
    else:
        error("Too few arguments. Run with -h for detailed help")


# Build system and exporters
@subcommand('target',
    dict(name='name', nargs='?', help='Default target name. Example: K64F, NUCLEO_F401RE, NRF51822...'),
    dict(name=['-G', '--global'], dest='global_cfg', action='store_true', help='Use global settings, not local'),
    dict(name=['-S', '--supported'], dest='supported', action='store_true', help='Shows supported matrix of targets and toolchains'),
    help='Set or get default target',
    description=(
        "Set or get default toolchain\n"
        "This is an alias to \"mbed config [--global] target [name]\"\n"))
def target_(name=None, global_cfg=False, supported=False):
    if supported:
        return compile_(supported=supported)
    return config_('target', name, global_cfg=global_cfg)


@subcommand('toolchain',
    dict(name='name', nargs='?', help='Default toolchain name. Example: ARM, GCC_ARM, IAR'),
    dict(name=['-G', '--global'], dest='global_cfg', action='store_true', help='Use global settings, not local'),
    dict(name=['-S', '--supported'], dest='supported', action='store_true', help='Shows supported matrix of targets and toolchains'),
    help='Set or get default toolchain',
    description=(
        "Set or get default toolchain\n"
        "This is an alias to \"mbed config [--global] toolchain [name]\"\n"))
def toolchain_(name=None, global_cfg=False, supported=False):
    if supported:
        return compile_(supported=supported)
    return config_('toolchain', name, global_cfg=global_cfg)


@subcommand('cache',
    dict(name='on', nargs='?', help='Turn repository caching on. Will use either the default or the user specified cache directory.'),
    dict(name='off', nargs='?', help='Turn repository caching off. Note that this doesn\'t purge cached repositories. See "purge".'),
    dict(name='dir', nargs='?', help='Set cache directory. Set to "default" to let mbed CLI determine the cache directory location (%s/mbed-cache/).' % Global().path),
    dict(name='ls', nargs='?', help='List cached repositories and their sizes.'),
    dict(name='purge', nargs='?', help='Purge cached repositories. Note that this doesn\'t turn caching off'),
    help='Repository cache management\n\n',
    description=(
        "Repository cache management\n"
        "To minimize traffic and reduce import times, Mbed CLI can cache repositories by storing their indexes.\n"
        "By default repository caching is turned on. Turn it off if you experience any problems.\n"))
def cache_(on=False, off=False, dir=None, ls=False, purge=False, global_cfg=False):
    cmd = str(on).lower()
    argument = off
    g = Global()

    cfg = g.cache_cfg()
    if cmd == 'off' or cmd == 'on':
        g.set_cfg('CACHE', 'enabled' if cmd == 'on' else 'disabled')
        cfg = g.cache_cfg()
        action("Repository cache is now %s." % str(cfg['cache']).upper())
        action("Cache location \"%s\"" % cfg['cache_dir'])
    elif cmd == 'dir':
        if not argument:
            error("Please specify directory or path to cache repositories. Alternatively specify \"default\" to cache repositories in the default user home location.")
        if not os.path.exists(argument):
            try:
                os.makedirs(argument)
            except (IOError, ImportError, OSError):
                error("Unable to create cache directory \"%s\"" % argument, 128)
        elif not os.path.isdir(argument):
            error("The specified location \"%s\" is not a directory" % argument, 128)
        elif len(os.listdir(argument)) > 1:
            warning("Directory \"%s\" is not empty." % argument)
        g.set_cfg('CACHE_DIR', argument)
        action('Repository cache location set to \"%s\"' % argument)
    elif cmd == 'ls':
        def get_size_(path):
            size = 0
            for dirpath, dirs, files in os.walk(path):
                for f in files:
                    size += os.path.getsize(os.path.join(dirpath, f))
            return size
        action("Listing cached repositories in \"%s\"" % cfg['cache_base'])
        repos = []
        total_size = 0
        for dirpath, dirs, files in os.walk(cfg['cache_dir']):
            dirs[:] = [d for d in dirs  if not d.startswith('.')]
            if Repo.isrepo(dirpath):
                repo = Repo().fromrepo(dirpath)
                url = repo.url
                size = get_size_(repo.path)
                total_size += size
                log("* %s %s\n" % ('{:68}'.format(url), sizeof_fmt(size).rjust(8)))
                for d in dirs:
                    dirs.remove(d)
        log(("-" * 79) + "\n")
        log("%s %s\n" % ('{:70}'.format('Total size:'), sizeof_fmt(total_size).rjust(8)))
    elif cmd == 'purge':
        action("Purging cached repositories in \"%s\"..." % cfg['cache_base'])
        if os.path.isdir(cfg['cache_dir']):
            rmtree_readonly(cfg['cache_dir'])
        action("Purge complete!")
    elif cmd == "false":
        action("Repository cache is %s." % str(cfg['cache']).upper())
        action("Cache location \"%s\"" % cfg['cache_dir'])
    else:
        print(cmd)
        error("Invalid cache command. Please see \"mbed cache --help\" for valid commands.")


@subcommand('help',
    help='This help screen')
def help_():
    return parser.print_help()



def main():
    global verbose, very_verbose, remainder, cwd_root

    # Help messages adapt based on current dir
    cwd_root = getcwd()

    # Parse/run command
    if len(sys.argv) <= 1:
        help_()
        sys.exit(1)

    if '--version' in sys.argv:
        log(ver+"\n")
        sys.exit(0)

    pargs, remainder = parser.parse_known_args()
    status = 1

    very_verbose = pargs.very_verbose
    verbose = very_verbose or pargs.verbose
    try:
        pathtype = Repo.pathtype(cwd_root)
        if not sys.argv[1].lower() in skip_workpath_commands:
            action('Working path \"%s\" (%s)' % (cwd_root, pathtype))
            if pathtype == "library":
                action('Program path \"%s\"' % Program(cwd_root).path)
        status = pargs.command(pargs)
    except ProcessException as e:
        tip = "" if verbose else "\nTip: You could retry the last command with \"-v\" flag for verbose output\n"

        error(
            "\"%s\" returned error.\n"
            "Code: %d\n"
            "Path: \"%s\"\n"
            "Command: \"%s\"%s" % (e.args[1], e.args[0], e.args[3], e.args[2], tip))
    except OSError as e:
        if e.args[0] == errno.ENOENT:
            error(
                "Could not detect one of the command-line tools.\n"
                "You could retry the last command with \"-v\" flag for verbose output\n", e.args[0])
        else:
            error('OS Error: %s' % e.args[1], e.args[0])
    except KeyboardInterrupt:
        info('User aborted!', -1)
        sys.exit(255)
    except Exception as e:
        if very_verbose:
            traceback.print_exc(file=sys.stdout)
        error("Unknown Error: %s" % e, 255)

    sys.exit(status or 0)


if __name__ == "__main__":
    main()
