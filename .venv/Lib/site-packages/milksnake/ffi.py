import os
import re
import cffi

from ._compat import PY2


_directive_re = re.compile(r'^\s*#.*?$', re.MULTILINE)


def make_ffi(module_path, header, strip_directives=False):
    """Creates a FFI instance for the given configuration."""
    if not PY2 and isinstance(header, bytes):
        header = header.decode('utf-8')
    if strip_directives:
        header = _directive_re.sub('', header)

    ffi = cffi.FFI()
    ffi.cdef(header)
    ffi.set_source(module_path, None)
    return ffi
