#!/usr/bin/env python3
# Copyright 2026(c) Analog Devices, Inc.
#
# SPDX short identifier: BSD-1-Clause
"""Tests for ci_select_builds.py, run against the real repository tree.

These are integration tests: they build the actual conf/path/select indices
from the checked-out source and assert the selector's classification of
representative change sets. Run with:

    python3 tools/scripts/test_ci_select_builds.py
"""
import unittest

import ci_select_builds as sel
from no_os_build import find_repo_root, load_presets, discover_all_combinations


class SelectorTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.repo = find_repo_root()
        presets = load_presets(cls.repo)
        cls.combos = discover_all_combinations(cls.repo, presets)
        cls.conf_index = sel.build_conf_symbol_index(cls.repo, cls.combos)
        cls.path_index = sel.build_path_symbol_index(cls.repo)
        cls.select_closure = sel.build_select_closure(cls.repo)
        cls.all_projects = {c["project"] for c in cls.combos}

    def classify(self, changed):
        return sel.classify(self.repo, changed, self.combos, self.conf_index,
                            self.path_index, self.select_closure)

    def test_doc_only_no_builds(self):
        ba, docs, tests, keys = self.classify(["doc/sphinx/source/x.rst", "README.md"])
        self.assertFalse(ba)
        self.assertTrue(docs)
        self.assertFalse(tests)
        self.assertEqual(keys, set())

    def test_tests_only(self):
        ba, docs, tests, keys = self.classify(["tests/drivers/foo/test_foo.c"])
        self.assertFalse(ba)
        self.assertTrue(tests)
        self.assertEqual(keys, set())

    def test_project_scopes_to_that_project(self):
        ba, _, _, keys = self.classify(["projects/admt4000/src/common/common_data.c"])
        self.assertFalse(ba)
        self.assertTrue(keys)
        self.assertEqual({p for (p, _v, _b) in keys}, {"admt4000"})

    def test_include_builds_all(self):
        ba, _, _, _ = self.classify(["include/no_os_spi.h"])
        self.assertTrue(ba)

    def test_build_system_builds_all(self):
        for path in ("CMakeLists.txt", "CMakePresets.json", "Kconfig",
                     "tools/scripts/no_os_build.py", "cmake/source_util.cmake",
                     "capi/foo.c"):
            ba, _, _, _ = self.classify([path])
            self.assertTrue(ba, f"{path} should build all")

    def test_platform_scopes_to_platform(self):
        ba, _, _, keys = self.classify(["drivers/platform/stm32/stm32_spi.c"])
        self.assertFalse(ba)
        self.assertTrue(keys)
        platforms = {c["platform"] for c in self.combos
                     if (c["project"], c["variant"], c["board"]) in keys}
        self.assertEqual(platforms, {"stm32"})

    def test_select_closure_narrows_gated_driver(self):
        # tmc5240.c is gated on CONFIG_MOTOR_TMC5240, which is select-ed by
        # CONFIG_MOTOR_IIO_TMC5240. The only conf enabling it is admt4000's
        # iio_trigger_tmc5240 variant, so exactly that combo must be selected.
        ba, _, _, keys = self.classify(["drivers/motor/tmc5240/tmc5240.c"])
        self.assertFalse(ba)
        self.assertTrue(keys, "expected at least the tmc5240 variant")
        for (proj, variant, _b) in keys:
            self.assertEqual(proj, "admt4000")
            self.assertIn("tmc5240", variant)

    def test_library_scopes_to_symbol(self):
        ba, _, _, keys = self.classify(["libraries/mbedtls/library/ssl_tls.c"])
        self.assertFalse(ba)
        # Every selected combo must actually enable mbedtls.
        for key in keys:
            proj, variant, _b = key
            conf = self.repo / "projects" / proj / f"{variant}.conf"
            self.assertIn("CONFIG_MBEDTLS", sel._read_enabled_symbols(conf))

    def test_unknown_gated_file_builds_all(self):
        # A file in a gated tree that no CMakeLists attributes is too risky to
        # drop -> build all.
        ba, _, _, _ = self.classify(["drivers/some_new_dir/mystery.c"])
        self.assertTrue(ba)

    def test_mixed_doc_and_project(self):
        ba, docs, _, keys = self.classify(
            ["doc/x.rst", "projects/admt4000/src/common/common_data.c"])
        self.assertFalse(ba)
        self.assertTrue(docs)
        self.assertEqual({p for (p, _v, _b) in keys}, {"admt4000"})

    def test_any_build_all_path_short_circuits(self):
        # A build-all path anywhere in the set forces build_all regardless of
        # the other (narrower) paths.
        ba, _, _, _ = self.classify(
            ["projects/admt4000/x.c", "include/no_os_spi.h"])
        self.assertTrue(ba)


if __name__ == "__main__":
    unittest.main(verbosity=2)
