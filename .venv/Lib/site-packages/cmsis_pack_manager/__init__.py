# ARM Pack Manager
# Copyright (c) 2017 ARM Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys
import time
import collections
from os.path import join, dirname, exists
from shutil import rmtree
from json import load
from zipfile import ZipFile
from appdirs import user_data_dir
from ._native import ffi, lib


class _RaiseRust(object):
    def __enter__(self):
        pass

    def __exit__(self, exc_type, exc_val, exe_tb):
        maybe_err = ffi.gc(lib.err_get_last_message(),
                           lib.err_last_message_free)
        if maybe_err:
            raise Exception(ffi.string(maybe_err))


class CmsisPackRef(collections.namedtuple(
        "CmsisPackRef",
        "vendor, pack, version"
)):
    def get_pdsc_name(self):
        return "{}.{}.{}.pdsc".format(self.vendor, self.pack, self.version)

    def get_pack_name(self):
        return join(self.vendor, self.pack, "{}.pack".format(self.version))

    def __str__(self):
        return "{}::{}::{}".format(self.vendor, self.pack, self.version)


class Cache (object):
    """ The Cache object is the only relevant API object at the moment

    Constructing the Cache object does not imply any caching.
    A user of the API must explicitly call caching functions.

    :param silent: A boolean that, when True, significantly reduces the
                   printing of this Object
    :type silent: bool
    :param no_timeouts: A boolean that, when True, disables the default
                        connection timeout and low speed timeout for
                        downloading things.
    :type no_timeouts: bool
    """
    def __init__(
            self,
            silent,
            __,
            json_path=None,
            data_path=None,
            vidx_list=None
    ):
        default_path = user_data_dir('cmsis-pack-manager')
        json_path = default_path if not json_path else json_path
        self._index = {}
        self._aliases = {}
        self.index_path = join(json_path, "index.json")
        self.aliases_path = join(json_path, "aliases.json")
        self.data_path = default_path if not data_path else data_path
        self.vidx_list = vidx_list
        self.silent = silent

    def get_flash_algorithm_binary(self, device_name, all=False):
        """Retrieve the flash algorithm file for a particular part.

        Assumes that both the PDSC and the PACK file associated with that part
        are in the cache.

        :param device_name: The exact name of a device
        :param all: Return an iterator of all flash algos for this device
        :type device_name: str
        :return: A file-like object that, when read, is the ELF file that
                 describes the flashing algorithm
        :return: A file-like object that, when read, is the ELF file that
                 describes the flashing algorithm.  When "all" is set to
                 True then an iterator for file-like objects is returned
        :rtype: ZipExtFile or ZipExtFile iterator if all is True
        """
        device = self.index[device_name]
        pack = self.pack_from_cache(device)
        algo_itr = (pack.open(algo['file_name'].replace(u'\\', '/')) for algo
                    in device['algorithms'])
        return algo_itr if all else algo_itr.next()

    @property
    def index(self):
        """An index of most of the important data in all cached PDSC files.

        :Example:

        >>> from ArmPackManager import Cache
        >>> a = Cache()
        >>> a.index["LPC1768"]
        {u'algorithms': [{u'default': False,
                          u'file_name': u'Flash/LPC_IAP_512.FLM',
                          u'size': 524288,
                          u'start': 0}],
         u'from_pack': {u'pack': u'LPC1700_DFP',
                        u'vendor': u'Keil',
                        u'version': u'2.4.0'},
         u'memories': {u'IRAM1': {u'access': {u'execute': False,
                                              u'read': True,
                                              u'write': True},
                                  u'size': 32768,
                                  u'start': 268435456,
                                  u'startup': False},
                       u'IRAM2': {u'access': {u'execute': False,
                                              u'read': True,
                                              u'write': True},
                                  u'size': 32768,
                                  u'start': 537378816,
                                  u'startup': False},
                       u'IROM1': {u'access': {u'execute': True,
                                              u'read': True,
                                              u'write': False},
                                  u'size': 524288,
                                  u'start': 0,
                                  u'startup': False}},
         u'name': u'LPC1768'}

        """
        if not self._index:
            try:
                with open(self.index_path) as i:
                    self._index = load(i)
            except IOError:
                return {}
        return self._index

    @property
    def aliases(self):
        """An index of the boards in all CMSIS Pack Descriptions.

        :Example:

        >>> from cmsis_pack_manager import Cache
        >>> a = Cache()
        >>> a.aliases["LPC1788-32 Developers Kit"]
        {"name": "LPC1788-32 Developers Kit",
         "mounted_devices": ["LPC1788"]}
        """

        if not self._aliases:
            try:
                with open(self.aliases_path) as i:
                    self._aliases = load(i)
            except IOError:
                return {}
        return self._aliases

    def cache_everything(self):
        """Cache every CMSIS Pack and generate an index.

        .. note:: This process may use 5GB of drive space and take upwards of
        2 minutes to complete.
        """
        parsed_packs = self.cache_descriptors()
        progress_fn = None if self.silent else self._verbose_on_tick_fn
        self._call_rust_update_packs(parsed_packs, progress_fn)

    def packs_for_devices(self, devices):
        """Create a list of all CMSIS Packs required to support the
        list of devices

        :return:
        A list of CmsisPackRef objects
        """
        to_ret = {}
        for dev in devices:
            pack_ref = CmsisPackRef(
                dev["from_pack"]["vendor"],
                dev["from_pack"]["pack"],
                dev["from_pack"]["version"],
            )
            to_ret[pack_ref] = True
        return list(to_ret.keys())

    def download_pack_list(self, pack_list):
        pdsc_index = ffi.gc(
            lib.update_pdsc_index_new(),
            lib.update_pdsc_index_free
        )
        for pack_ref in pack_list:
            pdsc_path = join(self.data_path, pack_ref.get_pdsc_name())
            cpdsc_path = ffi.new("char[]", pdsc_path.encode("utf-8"))
            with _RaiseRust():
                lib.update_pdsc_index_push(pdsc_index, cpdsc_path)
        parsed_packs = self._call_rust_parse(pdsc_index)
        progress_fn = None if self.silent else self._verbose_on_tick_fn
        self._call_rust_update_packs(parsed_packs, progress_fn)

    def _call_rust_update_packs(self, parsed_packs, on_tick_fn):
        if self.data_path:
            cdata_path = ffi.new("char[]", self.data_path.encode("utf-8"))
        else:
            cdata_path = ffi.NULL
        with _RaiseRust():
            poll_obj = lib.update_packs(cdata_path, parsed_packs)
        return self._poll_rust_update(poll_obj, on_tick_fn)

    def _verbose_on_tick_fn(self, total, current):
        if total:
            total = "{:03}".format(total)
        else:
            total = "???"
        sys.stdout.write(
            "Downloading descriptors ({:03}/{})\r".format(current, total)
        )
        sys.stdout.flush()

    def _call_rust_update(self, on_tick_fn):
        if self.data_path:
            cdata_path = ffi.new("char[]", self.data_path.encode("utf-8"))
        else:
            cdata_path = ffi.NULL
        if self.vidx_list:
            cvidx_path = ffi.new("char[]", self.vidx_list.encode("utf-8"))
        else:
            cvidx_path = ffi.NULL
        with _RaiseRust():
            poll_obj = lib.update_pdsc_index(cdata_path, cvidx_path)
        return self._poll_rust_update(poll_obj, on_tick_fn)

    def _poll_rust_update(self, poll_obj, on_tick_fn):
        with _RaiseRust():
            total_downloads = None
            current_downloads = 0
            while not lib.update_pdsc_poll(poll_obj):
                prev_downloads = current_downloads
                time.sleep(1/2)
                message = ffi.gc(
                    lib.update_pdsc_get_status(poll_obj),
                    lib.update_pdsc_status_free
                )
                while message:
                    if message.is_size:
                        total_downloads = message.size
                    else:
                        current_downloads += 1
                    message = ffi.gc(
                        lib.update_pdsc_get_status(poll_obj),
                        lib.update_pdsc_status_free
                    )
                if on_tick_fn and current_downloads != prev_downloads:
                    on_tick_fn(total_downloads, current_downloads)
            pdsc_index = ffi.gc(
                lib.update_pdsc_result(poll_obj), lib.update_pdsc_index_free
            )
        return pdsc_index

    def _call_rust_parse(self, pdsc_index):
        with _RaiseRust():
            parsed_packs = ffi.gc(lib.parse_packs(pdsc_index),
                                  lib.parse_packs_free)
        return parsed_packs

    def _call_rust_dump(self, parsed_packs):
        if self.index_path:
            cindex_path = ffi.new("char[]", self.index_path.encode("utf-8"))
        else:
            cindex_path = ffi.NULL
        if self.aliases_path:
            calias_path = ffi.new("char[]", self.aliases_path.encode("utf-8"))
        else:
            calias_path = ffi.NULL
        with _RaiseRust():
            lib.dump_pdsc_json(parsed_packs, cindex_path, calias_path)
        return parsed_packs

    def cache_descriptors(self):
        """Cache all Pack Descriptions and generate an index of them.

        .. note:: This process may use 14MB of drive space and take upwards of
        10 seconds.
        """
        progress_fn = None if self.silent else self._verbose_on_tick_fn
        pdsc_index = self._call_rust_update(progress_fn)
        parsed_packs = self._call_rust_parse(pdsc_index)
        self._call_rust_dump(parsed_packs)
        return parsed_packs

    def cache_clean(self):
        """Clean the entire cache."""
        if exists(self.data_path):
            rmtree(self.data_path)
        json_path = dirname(self.index_path)
        if exists(json_path):
            rmtree(json_path)

    def pdsc_from_cache(self, device):
        """Low level inteface for extracting a PDSC file from the cache.

        Assumes that the file specified is a PDSC file and is in the cache.

        :param url: The URL of a PDSC file.
        :type url: str
        :return: A parsed representation of the PDSC file.
        :rtype: Open file
        """
        from_pack = device['from_pack']
        dest = join(self.data_path, "{}.{}.{}.pdsc".format(
            from_pack['vendor'], from_pack['pack'], from_pack['version']))
        return open(dest, "r")

    def pack_from_cache(self, device):
        """Low level inteface for extracting a PACK file from the cache.

        Assumes that the file specified is a PACK file and is in the cache.

        :param url: The URL of a PACK file.
        :type url: str
        :return: A parsed representation of the PACK file.
        :rtype: ZipFile
        """
        from_pack = device['from_pack']
        return ZipFile(join(self.data_path,
                            from_pack['vendor'],
                            from_pack['pack'],
                            from_pack['version'] + ".pack"))

    @staticmethod
    def find_pdsc(zipfile):
        """Find the PDSC file within a PACK file

        :param zipfile: The PACK to scan
        :type zipfile: ZipFile
        :return: The location of the PDSC file within the PACK file
        :rtype: str
        """
        for zipinfo in zipfile.infolist():
            if (zipinfo.filename.upper().endswith(".PDSC")):
                return zipinfo.filename
        return None

    def add_pack_from_path(self, path):
        if path:
            cpack_path = ffi.new("char[]", path.encode("utf-8"))
        else:
            cpack_path = ffi.NULL
        with _RaiseRust():
            pack_files = ffi.gc(lib.pack_from_path(cpack_path),
                                lib.update_pdsc_index_free)
        parsed_packs = self._call_rust_parse(pack_files)
        self._call_rust_dump(parsed_packs)
        return parsed_packs
