#!/usr/bin/env python3
"""
Install the FTDI LibMPSSE and D2XX libraries into libraries/ftd2xx/.

Two packages are required:
  1. LibMPSSE  — the MPSSE protocol layer (SPI, I2C)
  2. libftd2xx — the D2XX USB driver layer (GPIO, and runtime dependency of LibMPSSE)

The installed tree is git-ignored (like all other no-OS fetched libraries).
CMake finds it automatically at the default location — no FTD2XX_LIBRARY
environment variable needed unless you install somewhere else.

FTDI's website blocks automated downloads (Cloudflare). Download both
archives manually from the links below, then pass them with --local-mpsse
and --local-d2xx.

LibMPSSE download links:
  Linux:   https://ftdichip.com/wp-content/uploads/2025/05/D2XX_MPSSE_1.0.7_Linux.zip
  macOS:   https://ftdichip.com/wp-content/uploads/2025/05/D2XX_MPSSE_1.0.7_MacOS.zip
  Windows: https://ftdichip.com/wp-content/uploads/2025/02/LibMPSSE_1.0.7.zip

D2XX (libftd2xx) download links:
  Linux x86_64:        https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-x86_64-1.4.35.tgz
  Linux x86_32:        https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-x86_32-1.4.35.tgz
  Linux arm-v8:        https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-arm-v8-1.4.35.tgz
  Linux arm-v7-hf:     https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-arm-v7-hf-1.4.35.tgz
  Linux arm-v7-hf-uclibc: https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-arm-v7-hf-uclibc-1.4.35.tgz
  Linux arm-v7-sf:     https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-arm-v7-sf-1.4.35.tgz
  Linux arm-v6-hf:     https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-arm-v6-hf-1.4.35.tgz
  macOS:               https://ftdichip.com/wp-content/uploads/2026/06/D2XX1.4.35.dmg
  Windows:             https://ftdichip.com/wp-content/uploads/2025/03/CDM-v2.12.36.20-WHQL-Certified.zip

Usage:
  python3 config_ftd2xx.py \\
      --local-mpsse D2XX_MPSSE_1.0.7_Linux.zip \\
      --local-d2xx  libftd2xx-linux-x86_64-1.4.35.tgz
"""

import argparse
import os
import platform
import shutil
import subprocess
import sys
import tarfile
import urllib.request
import zipfile

# ---------------------------------------------------------------------------
SCRIPT_DIR  = os.path.dirname(os.path.abspath(__file__))
NO_OS_ROOT  = os.path.abspath(os.path.join(SCRIPT_DIR, '..', '..'))
DEFAULT_DIR = os.path.join(NO_OS_ROOT, 'libraries', 'ftd2xx')

# ---------------------------------------------------------------------------
# LibMPSSE download URLs
MPSSE_URLS = {
	'linux':  'https://ftdichip.com/wp-content/uploads/2025/05/D2XX_MPSSE_1.0.7_Linux.zip',
	'darwin': 'https://ftdichip.com/wp-content/uploads/2025/05/D2XX_MPSSE_1.0.7_MacOS.zip',
	'win32':  'https://ftdichip.com/wp-content/uploads/2025/02/LibMPSSE_1.0.7.zip',
}

# D2XX (libftd2xx) download URLs — Linux has per-arch tarballs
D2XX_LINUX_URLS = {
	'x86_64':          'https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-x86_64-1.4.35.tgz',
	'x86_32':          'https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-x86_32-1.4.35.tgz',
	'arm-v8':          'https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-arm-v8-1.4.35.tgz',
	'arm-v7-hf':       'https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-arm-v7-hf-1.4.35.tgz',
	'arm-v7-hf-uclibc':'https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-arm-v7-hf-uclibc-1.4.35.tgz',
	'arm-v7-sf':       'https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-arm-v7-sf-1.4.35.tgz',
	'arm-v6-hf':       'https://ftdichip.com/wp-content/uploads/2026/06/libftd2xx-linux-arm-v6-hf-1.4.35.tgz',
}

D2XX_URLS = {
	'darwin': 'https://ftdichip.com/wp-content/uploads/2026/06/D2XX1.4.35.dmg',
	'win32':  'https://ftdichip.com/wp-content/uploads/2025/03/CDM-v2.12.36.20-WHQL-Certified.zip',
}

# Map platform.machine() → (libmpsse_arch, libftd2xx_arch)
LINUX_ARCH_MAP = {
	'x86_64':  ('x86_64',  'x86_64'),
	'i686':    ('x86_32',  'x86_32'),
	'i386':    ('x86_32',  'x86_32'),
	'aarch64': ('arm-v8',  'arm-v8'),
	'armv7l':  ('arm-v7-hf', 'arm-v7-hf'),
	'armv6l':  ('arm-v6-hf', 'arm-v6-hf'),
}

# ---------------------------------------------------------------------------

def parse_args():
	parser = argparse.ArgumentParser(
		description=__doc__,
		formatter_class=argparse.RawDescriptionHelpFormatter)
	parser.add_argument('install_dir', nargs='?', default=DEFAULT_DIR,
		help="Install destination (default: libraries/ftd2xx/ in the no-OS tree)")
	parser.add_argument('--local-mpsse', metavar='FILE',
		help="Locally downloaded LibMPSSE archive (skips download)")
	parser.add_argument('--local-d2xx', metavar='FILE',
		help="Locally downloaded D2XX (libftd2xx) archive (skips download)")
	return parser.parse_args()


def try_download(url, dest):
	"""Try curl first (handles Cloudflare better), fall back to urllib."""
	if shutil.which('curl'):
		ret = subprocess.run(
			['curl', '-fL', '-A',
			 'Mozilla/5.0 (X11; Linux x86_64; rv:125.0) Gecko/20100101 Firefox/125.0',
			 '-o', dest, url],
			check=False)
		if ret.returncode == 0:
			return
		print("  curl failed (exit %d), retrying with urllib ..." % ret.returncode)

	req = urllib.request.Request(url, headers={
		'User-Agent': ('Mozilla/5.0 (X11; Linux x86_64; rv:125.0) '
		               'Gecko/20100101 Firefox/125.0'),
		'Referer': 'https://ftdichip.com/drivers/d2xx-drivers/',
	})
	with urllib.request.urlopen(req) as resp:
		with open(dest, 'wb') as f:
			f.write(resp.read())


def get_file(url, local_arg, tmp_path, label):
	"""Return (path, should_delete). Uses local_arg if given, else downloads."""
	if local_arg:
		path = os.path.abspath(local_arg)
		if not os.path.exists(path):
			print("ERROR: %s file not found: %s" % (label, path))
			sys.exit(1)
		print("[%s] Using local file: %s" % (label, path))
		return path, False

	print("[%s] Downloading from %s ..." % (label, url))
	try:
		try_download(url, tmp_path)
		return tmp_path, True
	except Exception as e:
		if os.path.exists(tmp_path):
			os.remove(tmp_path)
		print("\n[%s] Download failed: %s" % (label, e))
		print("FTDI's site blocks automated downloads. Download manually:")
		print("  %s" % url)
		print("Then re-run with the appropriate --local-mpsse / --local-d2xx flag.")
		sys.exit(1)


# ---------------------------------------------------------------------------
# LibMPSSE install
# ---------------------------------------------------------------------------

def install_mpsse_linux(zip_path, install_dir, mpsse_arch):
	"""Extract arch-specific tarball from the Linux LibMPSSE zip."""
	tarball_name = 'libmpsse-%s-1.0.7.tgz' % mpsse_arch
	print("[LibMPSSE] Selecting tarball: %s" % tarball_name)

	tmp = install_dir + '.mpsse_tmp'
	os.makedirs(tmp, exist_ok=True)
	try:
		with zipfile.ZipFile(zip_path, 'r') as z:
			z.extractall(tmp)
		tarball_path = os.path.join(tmp, tarball_name)
		if not os.path.exists(tarball_path):
			available = [f for f in os.listdir(tmp) if f.endswith('.tgz')]
			print("ERROR: '%s' not found. Available: %s" % (tarball_name, available))
			sys.exit(1)
		with tarfile.open(tarball_path, 'r:gz') as t:
			t.extractall(install_dir)
	finally:
		shutil.rmtree(tmp, ignore_errors=True)


def install_mpsse_macos(archive_path, install_dir):
	"""Install the macOS LibMPSSE from either a .zip containing a .dmg or a
	bare .dmg (macOS auto-extracts .zip downloads from the browser).
	"""
	if not shutil.which('hdiutil'):
		print("ERROR: hdiutil not found — are you running on macOS?")
		sys.exit(1)

	tmp = install_dir + '.mpsse_tmp'
	os.makedirs(tmp, exist_ok=True)
	try:
		if zipfile.is_zipfile(archive_path):
			# ZIP wrapping a DMG — extract the DMG first.
			with zipfile.ZipFile(archive_path, 'r') as z:
				z.extractall(tmp)
			dmg_files = [f for f in os.listdir(tmp) if f.endswith('.dmg')]
			if not dmg_files:
				print("ERROR: no .dmg found in LibMPSSE macOS archive.")
				sys.exit(1)
			dmg_path = os.path.join(tmp, dmg_files[0])
		else:
			# Bare DMG (browser download, auto-extracted by macOS).
			dmg_path = archive_path

		mount_point = tmp + '_mnt'
		print("[LibMPSSE] Mounting %s ..." % os.path.basename(dmg_path))
		subprocess.run(['hdiutil', 'attach', dmg_path,
		                '-mountpoint', mount_point, '-nobrowse'], check=True)
		try:
			release_src = os.path.join(mount_point, 'release')
			src = release_src if os.path.isdir(release_src) else mount_point
			shutil.copytree(src, os.path.join(install_dir, 'release'))
		finally:
			subprocess.run(['hdiutil', 'detach', mount_point], check=False)
	finally:
		shutil.rmtree(tmp, ignore_errors=True)
	memcpy = os.path.join(install_dir, 'release', 'source', 'memcpy.c')
	if os.path.exists(memcpy):
		os.remove(memcpy)


def install_mpsse_windows(zip_path, install_dir):
	"""Extract the Windows LibMPSSE nested zip."""
	tmp = install_dir + '.mpsse_tmp'
	os.makedirs(tmp, exist_ok=True)
	try:
		with zipfile.ZipFile(zip_path, 'r') as z:
			z.extractall(tmp)
		nested = os.path.join(tmp, 'LibMPSSE_1.0.7', 'Windows',
		                      'libmpsse-windows-1.0.7.zip')
		if not os.path.exists(nested):
			print("ERROR: expected nested zip not found: %s" % nested)
			sys.exit(1)
		with zipfile.ZipFile(nested, 'r') as z:
			z.extractall(install_dir)
	finally:
		shutil.rmtree(tmp, ignore_errors=True)


# ---------------------------------------------------------------------------
# D2XX (libftd2xx) install
# ---------------------------------------------------------------------------

def install_d2xx_linux(archive, install_dir, d2xx_arch):
	"""
	Extract libftd2xx .so files from the arch-specific Linux tarball.
	Tarball layout: linux-<arch>/libftd2xx.so*
	"""
	build_dir = os.path.join(install_dir, 'release', 'build')
	os.makedirs(build_dir, exist_ok=True)

	subdir = 'linux-%s' % d2xx_arch
	print("[D2XX] Extracting from tarball subdir: %s/" % subdir)

	with tarfile.open(archive, 'r:gz') as t:
		for member in t.getmembers():
			name = member.name
			# Extract only .so files from the arch subdir
			if name.startswith(subdir + '/') and '.so' in os.path.basename(name):
				member.name = os.path.basename(name)
				t.extract(member, build_dir)

	so_files = [f for f in os.listdir(build_dir) if 'ftd2xx' in f and '.so' in f]
	if not so_files:
		print("ERROR: no libftd2xx .so files found in tarball subdir '%s/'" % subdir)
		print("Check that the tarball matches your architecture.")
		sys.exit(1)
	print("[D2XX] Installed: %s" % ', '.join(so_files))


def _mount_and_copy_d2xx(dmg_path, install_dir):
	"""Mount a D2XX DMG and copy the dylib/static libraries out of it.

	D2XX DMG layout:
	  release/build/  — libftd2xx.dylib, libftd2xx.<ver>.dylib, libftd2xx.a
	"""
	build_dir = os.path.join(install_dir, 'release', 'build')
	os.makedirs(build_dir, exist_ok=True)

	mount_point = install_dir + '.d2xx_mnt'
	print("[D2XX] Mounting %s ..." % os.path.basename(dmg_path))
	subprocess.run(['hdiutil', 'attach', dmg_path,
	                '-mountpoint', mount_point, '-nobrowse'], check=True)
	try:
		# Libraries live in release/build/ inside the DMG.
		src_dir = os.path.join(mount_point, 'release', 'build')
		if not os.path.isdir(src_dir):
			# Fallback: scan the DMG root (older layout)
			src_dir = mount_point
		for f in os.listdir(src_dir):
			if '.dylib' in f or f.endswith('.a'):
				shutil.copy2(os.path.join(src_dir, f), build_dir)
				print("[D2XX] Installed: %s" % f)
	finally:
		subprocess.run(['hdiutil', 'detach', mount_point], check=False)
		if os.path.exists(mount_point):
			shutil.rmtree(mount_point, ignore_errors=True)


def install_d2xx_macos(archive_path, install_dir):
	"""Install D2XX libraries from either a .dmg or a .zip containing a .dmg.

	FTDI distributes the macOS D2XX package as a .zip. Depending on how it
	was obtained:
	  - Downloaded via browser: macOS auto-extracts the .zip → bare .dmg
	  - Downloaded by the script (curl/urllib): raw .zip → contains a .dmg

	Both cases are handled transparently by detecting the file type.
	"""
	if not shutil.which('hdiutil'):
		print("ERROR: hdiutil not found — are you running on macOS?")
		sys.exit(1)

	if zipfile.is_zipfile(archive_path):
		# ZIP wrapping a DMG — extract the DMG first.
		tmp = install_dir + '.d2xx_zip_tmp'
		os.makedirs(tmp, exist_ok=True)
		try:
			with zipfile.ZipFile(archive_path, 'r') as z:
				dmg_files = [f for f in z.namelist() if f.lower().endswith('.dmg')]
				if not dmg_files:
					print("ERROR: no .dmg found inside D2XX zip archive.")
					sys.exit(1)
				print("[D2XX] Extracting %s from zip ..." % dmg_files[0])
				z.extract(dmg_files[0], tmp)
				dmg_path = os.path.join(tmp, dmg_files[0])
			_mount_and_copy_d2xx(dmg_path, install_dir)
		finally:
			shutil.rmtree(tmp, ignore_errors=True)
	else:
		# Bare DMG (browser download, auto-extracted by macOS).
		_mount_and_copy_d2xx(archive_path, install_dir)


def codesign_macos_dylibs(install_dir):
	"""Strip quarantine flag and ad-hoc sign all installed dylibs.

	On macOS 13+ (Ventura/Sonoma/Sequoia), dylibs downloaded from the
	internet carry a com.apple.quarantine extended attribute that causes
	Gatekeeper to block them. The block decision is cached until reboot
	unless the quarantine flag is removed first.

	Steps:
	  1. xattr -cr  — remove quarantine and all other extended attributes
	  2. codesign --force --sign -  — ad-hoc sign (no paid Apple account
	     required; '-' identity is Apple's supported mechanism for local use)
	"""
	build_dir = os.path.join(install_dir, 'release', 'build')
	if not os.path.isdir(build_dir):
		return
	dylibs = [os.path.join(build_dir, f)
	          for f in os.listdir(build_dir) if f.endswith('.dylib')]
	if not dylibs:
		return

	if shutil.which('xattr'):
		print("[macOS] Removing quarantine flags from dylibs ...")
		for path in dylibs:
			subprocess.run(['xattr', '-cr', path], check=False)

	if shutil.which('codesign'):
		print("[macOS] Ad-hoc signing dylibs ...")
		for path in dylibs:
			ret = subprocess.run(
				['codesign', '--force', '--sign', '-', path],
				capture_output=True, text=True)
			name = os.path.basename(path)
			if ret.returncode == 0:
				print("[macOS] Signed: %s" % name)
			else:
				print("[macOS] WARNING: codesign failed for %s: %s"
				      % (name, ret.stderr.strip()))


def install_d2xx_windows(zip_path, install_dir):
	"""
	Extract ftd2xx.lib and FTD2XX64.dll from the CDM WHQL zip.
	CDM layout: amd64/ftd2xx.lib, amd64/FTD2XX64.dll, Static/amd64/FTD2XX.lib
	Target: release/build/x64/DLL/ and release/build/x64/LIB/ (where libmpsse lives).
	"""
	dll_dir = os.path.join(install_dir, 'release', 'build', 'x64', 'DLL')
	lib_dir = os.path.join(install_dir, 'release', 'build', 'x64', 'LIB')
	os.makedirs(dll_dir, exist_ok=True)
	os.makedirs(lib_dir, exist_ok=True)

	print("[D2XX] Extracting from CDM WHQL zip ...")
	with zipfile.ZipFile(zip_path, 'r') as z:
		# Dynamic import lib and runtime DLLs → DLL dir (for DLL linking)
		# amd64/ftd2xx.dll is saved as FTD2XX.dll to match the name the
		# libmpsse import library expects at runtime.
		dll_files = {
			'amd64/ftd2xx.lib':    'ftd2xx.lib',
			'amd64/FTD2XX64.dll':  'FTD2XX64.dll',
			'amd64/ftd2xx.dll':    'FTD2XX.dll',
		}
		for name, basename in dll_files.items():
			if name in z.namelist():
				data = z.read(name)
				dest = os.path.join(dll_dir, basename)
				with open(dest, 'wb') as f:
					f.write(data)
				print("[D2XX] Installed: %s" % dest)
		# Static lib → LIB dir
		if 'Static/amd64/FTD2XX.lib' in z.namelist():
			data = z.read('Static/amd64/FTD2XX.lib')
			dest = os.path.join(lib_dir, 'FTD2XX.lib')
			with open(dest, 'wb') as f:
				f.write(data)
			print("[D2XX] Installed: %s" % dest)


# ---------------------------------------------------------------------------

def main():
	args = parse_args()
	install_dir = os.path.abspath(args.install_dir)
	plat = sys.platform
	plat_key = 'linux' if plat.startswith('linux') else plat

	if plat_key not in MPSSE_URLS:
		print("Unsupported platform: %s" % plat)
		sys.exit(1)

	# Detect Linux arch
	mpsse_arch = d2xx_arch = None
	if plat_key == 'linux':
		machine = platform.machine()
		archs = LINUX_ARCH_MAP.get(machine)
		if not archs:
			print("WARNING: unknown arch '%s', defaulting to x86_64" % machine)
			archs = ('x86_64', 'x86_64')
		mpsse_arch, d2xx_arch = archs
		print("Detected arch: %s (machine=%s)" % (d2xx_arch, machine))

	already_done = os.path.isdir(os.path.join(install_dir, 'release'))
	if already_done and not args.local_mpsse and not args.local_d2xx:
		print("Already installed at: %s" % install_dir)
		print("Delete it first to re-install, or pass --local-mpsse/--local-d2xx to update.")
		sys.exit(0)

	os.makedirs(install_dir, exist_ok=True)

	# ---- Step 1: LibMPSSE ----
	if not already_done or args.local_mpsse:
		mpsse_file, mpsse_rm = get_file(
			MPSSE_URLS[plat_key],
			args.local_mpsse,
			os.path.join(install_dir, '_mpsse_download.zip'),
			'LibMPSSE')

		print("[LibMPSSE] Installing ...")
		if plat_key == 'linux':
			install_mpsse_linux(mpsse_file, install_dir, mpsse_arch)
		elif plat_key == 'darwin':
			install_mpsse_macos(mpsse_file, install_dir)
		else:
			install_mpsse_windows(mpsse_file, install_dir)

		if mpsse_rm:
			os.remove(mpsse_file)
		print("[LibMPSSE] Done.")

	# ---- Step 2: D2XX (libftd2xx) ----
	if plat_key == 'linux':
		d2xx_url = D2XX_LINUX_URLS.get(d2xx_arch,
		           D2XX_LINUX_URLS['x86_64'])
	else:
		d2xx_url = D2XX_URLS[plat_key]

	# For macOS D2XX the local arg may be a bare .dmg, not a zip
	d2xx_file, d2xx_rm = get_file(
		d2xx_url,
		args.local_d2xx,
		os.path.join(install_dir, '_d2xx_download.tmp'),
		'D2XX')

	print("[D2XX] Installing ...")
	if plat_key == 'linux':
		install_d2xx_linux(d2xx_file, install_dir, d2xx_arch)
	elif plat_key == 'darwin':
		install_d2xx_macos(d2xx_file, install_dir)
	else:
		install_d2xx_windows(d2xx_file, install_dir)

	if d2xx_rm:
		os.remove(d2xx_file)
	print("[D2XX] Done.")

	# ---- macOS: strip quarantine and ad-hoc sign dylibs ----
	if plat_key == 'darwin':
		codesign_macos_dylibs(install_dir)

	# ---- Summary ----
	release_dir = os.path.join(install_dir, 'release')
	print("\nAll libraries installed to: %s" % install_dir)
	print("FTD2XX_LIBRARY=%s" % release_dir)
	print("(CMake finds this automatically if installed to the default location.)")


if __name__ == '__main__':
	main()
