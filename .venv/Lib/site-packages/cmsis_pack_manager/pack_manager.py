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
from __future__ import print_function, absolute_import

import argparse
import operator
import sys
from os.path import join, dirname, exists
from os import makedirs
from functools import reduce
from json import dump
import yaml
from cmsis_pack_manager import Cache

parser = argparse.ArgumentParser(
    description='A utility that keeps your cache of pack files up to date.'
)
subparsers = parser.add_subparsers(title="Commands")


def subcommand(name, *args, **kwargs):
    def subcommand(command):
        subparser = subparsers.add_parser(name, **kwargs)

        for arg in args:
            arg = dict(arg)
            opt = arg['name']
            del arg['name']

            if isinstance(opt, str):
                subparser.add_argument(opt, **arg)
            else:
                subparser.add_argument(*opt, **arg)

        subparser.add_argument(
            "-v", "--verbose",
            action="store_true",
            dest="verbose",
            help="Verbose diagnostic output",
        )
        subparser.add_argument(
            "--and",
            action="store_true",
            dest="intersection",
            help="Combine search terms as if with an `and`",
        )
        subparser.add_argument(
            "--or",
            action="store_false",
            dest="intersection",
            help="Combine search terms as if with an `or`",
        )
        subparser.add_argument(
            "--union",
            action="store_false",
            dest="intersection",
            help="Combine search terms as if with a set union",
        )
        subparser.add_argument(
            "--intersection",
            action="store_true",
            dest="intersection",
            help="Combine search terms as if with a set intersection",
        )
        subparser.add_argument("--vidx-list", dest="vidx_list")
        subparser.add_argument("--data-path", dest="data_path")
        subparser.add_argument("--json-path", dest="json_path")

        def thunk(parsed_args):
            cache = Cache(not parsed_args.verbose, False,
                          vidx_list=parsed_args.vidx_list,
                          data_path=parsed_args.data_path,
                          json_path=parsed_args.json_path)
            argv = [
                arg['dest'] if 'dest' in arg else arg['name'] for arg in args
            ]
            argv = [(arg if isinstance(arg, str) else arg[-1]).strip('-')
                    for arg in argv]
            argv = {arg: vars(parsed_args)[arg] for arg in argv
                    if vars(parsed_args)[arg] is not None}

            return command(cache, **argv)

        subparser.set_defaults(command=thunk)
        return command
    return subcommand


def fuzzy_find(matches, options, oper=operator.and_):
    return reduce(oper, (set(filter(lambda x: match in x, options))
                         for match in matches))


CACHE_CHOICES = [
    "descriptors", "desc", "pdsc",
    "everything", "every", "packs",
    "devices", "devs",
    "clean", "rm"
]

CACHE_CHOICES_HELP = (
    "Caching action; "
    "descriptors, desc, pdsc: download only pack descriptors; "
    "everything, every, packs: download packs and descriptors; "
    "devices, devs: download the packs for the listed devices; "
    "clean, rm: remove downloaded packs and descriptors"
)


@subcommand(
    'cache',
    dict(name="action", choices=CACHE_CHOICES, help=CACHE_CHOICES_HELP),
    dict(name="devices", nargs="*"),
    help="Download or remove pack and pack descriptions in the cache"
)
def command_cache(cache, action, devices, verbose=False, intersection=True):
    if action in ("everything", "every", "packs"):
        cache.cache_everything()
        print("Packs Cached")
        return True
    elif action in ("descriptors", "desc", "pdsc"):
        cache.cache_descriptors()
        print("Descriptors Cached")
        return True
    elif action in ("clean", "rm"):
        cache.cache_clean()
        print("Cache cleaned")
        return True
    elif action in ("devices", "devs"):
        cache.cache_descriptors()
        devices = [cache.index[dev] for dev in devices]
        packs = cache.packs_for_devices(devices)
        print("Downloading Packs: ")
        for pack in packs:
            print("  {}".format(pack))
        raw_input("Press Control-C to quit; Press Enter to continue")
        cache.download_pack_list(packs)
    else:
        print("No action specified nothing to do")


@subcommand('find-part',
            dict(name='matches', nargs="+",
                 help="Words to match to processors"),
            dict(name=['-l', "--long"], action="store_true",
                 help="Print out part details with part"),
            dict(name=['-p', '--parts-only'], action="store_false",
                 dest="print_aliases"),
            dict(name=['-a', '--aliases-only'], action="store_false",
                 dest="print_parts"),
            help="Find a part and its description within the cache")
def command_find_part(cache, matches, long=False, intersection=True,
                      print_aliases=True, print_parts=True):
    op = operator.and_ if intersection else operator.or_
    to_dump = {} if long else []
    if print_parts:
        for part in fuzzy_find(matches, cache.index.keys(), op):
            if long:
                to_dump.update({part: cache.index[part]})
            else:
                to_dump.append(part)
    if print_aliases:
        for alias in fuzzy_find(matches, cache.aliases.keys(), op):
            if long:
                if cache.aliases[alias]["mounted_devices"]:
                    part = cache.aliases[alias]["mounted_devices"][0]
                    try:
                        to_dump.update({alias: cache.index[part]})
                    except KeyError:
                        to_dump.update({
                            alias: "Could not find part: %s" % part
                        })
            else:
                to_dump.append(alias)
    print(yaml.safe_dump(to_dump, default_flow_style=None if long else False))


@subcommand('dump-parts',
            dict(name='out', help='Directory to dump to'),
            dict(name='parts', nargs='+', help='Parts to dump'),
            help='Create a directory with an `index.json` describing the part '
            'and all of the associated flashing algorithms.')
def command_dump_parts(cache, out, parts, intersection=False):
    op = operator.and_ if intersection else operator.or_
    index = {part: cache.index[part] for part
             in fuzzy_find(parts, cache.index, op)}
    if not exists(out):
        makedirs(out)
    for n, p in index.items():
        try:
            for algo in p['algorithms']:
                if not exists(join(out, dirname(algo['file_name']))):
                    makedirs(join(out, dirname(algo['file_name'])))
                with open(join(out, algo['file_name']), "wb") as fd:
                    fd.write(cache.pack_from_cache(p)
                             .open(algo['file_name'])
                             .read())
        except KeyError:
            print("[Warning] {} does not have an associated flashing algorithm"
                  .format(n))
    with open(join(out, "index.json"), "w") as fd:
        dump(index, fd)


@subcommand('add-packs',
            dict(name='path', nargs='+',
                 help='path to pdsc to add into the index'),
            help='add contents of pdsc files into the index')
def command_add_packs(cache, path, intersection=False):
    for p in path:
        cache.add_pack_from_path(p)


def get_argparse():
    return parser


def main():
    if (len(sys.argv) < 2):
        parser.print_help()
    else:
        args = parser.parse_args()
        args.command(args)
