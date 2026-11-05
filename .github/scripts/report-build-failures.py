#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 Analog Devices, Inc. All rights reserved.
"""Report the project builds that failed in a build-*-platform workflow run.

Reads <log_dir>/build_failures.json (written by tools/scripts/build_projects.py)
and the per-combination logs next to it, then writes one or both of:

  --html FILE      A self-contained page holding the full log of every failed
                   combination, one anchor per combination. Uploaded with
                   actions/upload-artifact `archive: false`, so it opens
                   straight in the browser instead of as a zip download.
  --markdown FILE  The PR comment body: the affected projects, plus an error
                   excerpt (+/- CONTEXT lines around each error line) per
                   failed combination. Empty when nothing failed, so the
                   shared-actions pr-comment step deletes a stale comment.
                   Not written when build_failures.json is missing (the
                   build stopped early, e.g. cancelled by a newer push), so
                   the existing comment is left as it is.

Standard library only: it runs inside the platform build containers too.
"""

import argparse
import html
import json
import os
import re
import sys

CONTEXT = 5
MAX_EXCERPT_WINDOWS = 3
MAX_LINE_LEN = 300
TAIL_LINES = 15
MAX_HTML_LOG_LINES = 20000
# GitHub rejects comment bodies over 65536 characters; leave headroom.
MAX_COMMENT_LEN = 60000

ANSI_RE = re.compile(r'\x1b\[[0-9;?]*[ -/]*[@-~]')
# Lines that name the actual failure. Tried first, so a stray "failed" in the
# configure output does not push the real compiler/linker error out of view.
STRONG_RE = re.compile(
	r'error:|fatal error|undefined reference|multiple definition|'
	r'^FAILED:|CMake Error|ninja: build stopped|make(\[\d+\])?: \*\*\*|'
	r'Error \d+|Traceback \(most recent call last\)', re.IGNORECASE)
WEAK_RE = re.compile(r'\b(errors?|fatal|failed|failure)\b', re.IGNORECASE)


def parse_args():
	parser = argparse.ArgumentParser(description=__doc__,
			formatter_class=argparse.RawDescriptionHelpFormatter)
	parser.add_argument('--log-dir', required=True,
			help="Directory holding build_failures.json and the per-combo logs")
	parser.add_argument('--platform', required=True,
			help="Platform name shown in the report (e.g. stm32)")
	parser.add_argument('--html', help="Write the failed-logs page here")
	parser.add_argument('--markdown', help="Write the PR comment body here")
	parser.add_argument('--failed-logs-url', default='',
			help="URL of the uploaded --html artifact; per-combo links get an #anchor")
	parser.add_argument('--logs-url', default='',
			help="URL of the full (zipped) logs artifact")
	parser.add_argument('--commit', default='',
			help="SHA of the commit the report was created for, shown in the footer")
	parser.add_argument('--commit-url', default='',
			help="URL the footer commit SHA links to")
	return parser.parse_args()


def load_failures(log_dir):
	"""Return the recorded failures, or None when the build did not complete.

	build_projects.py writes build_failures.json (possibly empty) only at the
	end of its run, so a missing or unreadable file means the build stopped
	early: a setup error, a crash, a cancellation or a timeout.
	"""
	path = os.path.join(log_dir, 'build_failures.json')
	try:
		with open(path) as f:
			failures = json.load(f)
	except FileNotFoundError:
		return None
	except (OSError, ValueError) as e:
		print("warning: cannot read %s: %s" % (path, e), file=sys.stderr)
		return None
	if not isinstance(failures, list):
		print("warning: unexpected content in %s" % path, file=sys.stderr)
		return None
	return failures


def read_log(log_dir, failure):
	if not failure.get('log'):
		return None
	path = os.path.join(log_dir, failure['log'])
	try:
		with open(path, errors='replace') as f:
			return [ANSI_RE.sub('', l.rstrip('\r\n')) for l in f]
	except OSError:
		return None


def anchor(failure):
	name = "%s_%s_%s_%s" % (failure['project'], failure['platform'],
				failure['variant'], failure['board'])
	return re.sub(r'[^A-Za-z0-9_.-]', '-', name)


def clip(line):
	if len(line) <= MAX_LINE_LEN:
		return line
	return line[:MAX_LINE_LEN] + ' [...]'


def excerpt(lines):
	"""Return (excerpt_lines, hidden_matches) for a failed build log.

	Every line matching STRONG_RE (falling back to WEAK_RE) opens a window of
	CONTEXT lines on each side; overlapping windows merge. Only the first
	MAX_EXCERPT_WINDOWS windows are kept. With no match at all (e.g. a
	timeout), the last TAIL_LINES lines are returned instead.
	"""
	hits = [i for i, l in enumerate(lines) if STRONG_RE.search(l)]
	if not hits:
		hits = [i for i, l in enumerate(lines) if WEAK_RE.search(l)]
	if not hits:
		return [clip(l) for l in lines[-TAIL_LINES:]], 0

	windows = []
	for i in hits:
		start, end = max(0, i - CONTEXT), min(len(lines), i + CONTEXT + 1)
		if windows and start <= windows[-1][1]:
			windows[-1][1] = max(windows[-1][1], end)
		else:
			windows.append([start, end])

	out = []
	for n, (start, end) in enumerate(windows[:MAX_EXCERPT_WINDOWS]):
		if n:
			out.append('[...]')
		out.extend(clip(l) for l in lines[start:end])
	hidden = sum(1 for i in hits if i >= windows[MAX_EXCERPT_WINDOWS - 1][1]) \
		if len(windows) > MAX_EXCERPT_WINDOWS else 0
	return out, hidden


def combo_label(failure):
	return "`%s` / `%s`" % (failure['variant'], failure['board'])


def footer_note(platform, commit, commit_url):
	note = "_Posted automatically by the %s build workflow" % platform
	if commit:
		sha = "`%s`" % commit[:7]
		note += " for commit %s" % ("[%s](%s)" % (sha, commit_url) if commit_url else sha)
	return note + "._"


def render_markdown(failures, log_dir, platform, failed_logs_url, logs_url,
		commit='', commit_url=''):
	if not failures:
		return ""

	by_project = {}
	for f in failures:
		by_project.setdefault(f['project'], []).append(f)

	head = ["### :x: Platform %s has build errors for the following projects:" % platform, ""]
	for project in sorted(by_project):
		combos = []
		for f in by_project[project]:
			label = combo_label(f)
			if failed_logs_url and f.get('log'):
				label = "[%s](%s#%s)" % (label, failed_logs_url, anchor(f))
			combos.append(label)
		head.append("- **%s**: %s" % (project, ", ".join(combos)))
	head.append("")

	links = []
	if failed_logs_url:
		links.append("[failed build logs](%s)" % failed_logs_url)
	if logs_url:
		links.append("[all build logs (zip)](%s)" % logs_url)
	foot = [""]
	if links:
		foot.append("Full logs: %s." % " | ".join(links))
	foot.append(footer_note(platform, commit, commit_url))

	body = "\n".join(head)
	budget = MAX_COMMENT_LEN - len(body) - len("\n".join(foot)) - 200
	details = []
	omitted = 0
	for f in sorted(failures, key=lambda f: (f['project'], f['variant'], f['board'])):
		lines = read_log(log_dir, f)
		block = ["<details><summary><b>%s</b> (%s) &mdash; %s</summary>" % (
			html.escape(f['project']), html.escape("%s / %s" % (f['variant'], f['board'])),
			html.escape(f['reason'])), ""]
		if lines is None:
			block.append("_No build log: the build was not started._")
		elif not lines:
			block.append("_The build log is empty._")
		else:
			ex, hidden = excerpt(lines)
			# A fence longer than any backtick run in the log cannot be closed early.
			longest = max((len(m) for l in ex for m in re.findall(r'`+', l)), default=0)
			fence = '`' * max(3, longest + 1)
			block += [fence] + ex + [fence]
			if hidden:
				block.append("_%d more error line(s) further down in the full log._" % hidden)
		block += ["", "</details>"]
		text = "\n".join(block)
		if len(text) + 2 > budget:
			omitted += 1
			continue
		budget -= len(text) + 2
		details.append(text)

	if omitted:
		details.append("_%d error excerpt(s) omitted to fit the comment size limit; "
			"see the full logs._" % omitted)
	# Blank lines keep each <details> block out of the preceding list item.
	return body + "\n" + "\n\n".join(details) + "\n" + "\n".join(foot) + "\n"


def render_html(failures, log_dir, platform):
	esc = html.escape
	failures = sorted(failures, key=lambda f: (f['project'], f['variant'], f['board']))
	out = ["<!DOCTYPE html>", "<html><head><meta charset=\"utf-8\">",
		"<title>%s failed builds</title>" % esc(platform),
		"<style>body{font-family:sans-serif;margin:1em 2em}"
		"pre{background:#f6f8fa;padding:.8em;overflow-x:auto;font-size:12px}"
		"h2{border-bottom:1px solid #ddd;padding-top:.5em}</style>",
		"</head><body>",
		"<h1>Platform %s: %d failed build(s)</h1>" % (esc(platform), len(failures)), "<ul>"]
	for f in failures:
		out.append("<li><a href=\"#%s\">%s</a> (%s / %s) &mdash; %s</li>" % (
			anchor(f), esc(f['project']), esc(f['variant']), esc(f['board']),
			esc(f['reason'])))
	out.append("</ul>")
	for f in failures:
		out.append("<h2 id=\"%s\">%s (%s / %s)</h2>" % (
			anchor(f), esc(f['project']), esc(f['variant']), esc(f['board'])))
		out.append("<p>%s%s</p>" % (esc(f['reason']),
			(" &mdash; <code>%s</code>" % esc(f['log'])) if f.get('log') else ""))
		lines = read_log(log_dir, f)
		if lines is None:
			out.append("<p><i>No build log: the build was not started.</i></p>")
			continue
		if len(lines) > MAX_HTML_LOG_LINES:
			out.append("<p><i>Showing the last %d of %d lines; the full log is in "
				"the logs artifact.</i></p>" % (MAX_HTML_LOG_LINES, len(lines)))
			lines = lines[-MAX_HTML_LOG_LINES:]
		out.append("<pre>%s</pre>" % esc("\n".join(lines)))
	out.append("</body></html>")
	return "\n".join(out) + "\n"


def main():
	args = parse_args()
	failures = load_failures(args.log_dir)

	if args.html:
		if failures:
			with open(args.html, 'w') as f:
				f.write(render_html(failures, args.log_dir, args.platform))
		elif os.path.exists(args.html):
			# Self-hosted runners keep the workspace: never upload a stale page.
			os.remove(args.html)
	if args.markdown and failures is None:
		# No results to report: leave the PR comment alone rather than
		# deleting it or posting noise for a cancelled run.
		if os.path.exists(args.markdown):
			os.remove(args.markdown)
	elif args.markdown:
		# The log dir is missing when the build step died before creating it.
		os.makedirs(os.path.dirname(os.path.abspath(args.markdown)), exist_ok=True)
		with open(args.markdown, 'w') as f:
			f.write(render_markdown(failures, args.log_dir, args.platform,
				args.failed_logs_url, args.logs_url,
				args.commit, args.commit_url))
	if failures is None:
		print("build on %s did not complete: no usable build_failures.json"
			% args.platform)
	else:
		print("%d failed build(s) on %s" % (len(failures), args.platform))


if __name__ == '__main__':
	main()
