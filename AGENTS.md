# no-OS — instructions for AI agents

This file is the vendor-neutral entry point for any AI coding agent working in
this repository (read by Codex, Cursor, Aider, Gemini CLI, and others; the
nearest `AGENTS.md` in the directory tree takes precedence). The human-facing
source of truth is `doc/sphinx/source/contributing.rst`.

## Commit trailers — always required

Every commit must end with:

- **`Assisted-by: <agent and model>`** — REQUIRED whenever an AI agent authored
  or modified the commit, e.g. `Assisted-by: Claude Sonnet 4.6`. Place it
  **before** `Signed-off-by`. If you are the agent making the change, add it
  yourself — do not wait to be asked. The sign-off and human responsibility for
  the change still apply; this trailer only records that assistance was used.

- **`Signed-off-by: Name <email>`** — the committer certifies they have the
  rights to submit the work under the project's license. Produced by
  `git commit -s`.

Example trailer block:

```
Assisted-by: Claude Sonnet 4.6
Signed-off-by: Jane Dev <jane.dev@analog.com>
```

## Commit message rules

- **50/72 rule**: subject ≤ 50 chars; body wrapped at 72 chars per line.
- **Path prefix**: the subject starts with the relative path to the affected
  area, e.g. `dac: ad5460: add driver support` or
  `projects: eval-adis1655x: add implementation`.

## Code checks maintainers run

- Builds cleanly under **gcc** with `-Wall -Wextra` (CI triggers driver and
  project builds on the PR).
- Passes **astyle**:
  `astyle --style=linux --indent=force-tab=8 --max-code-length=80 --suffix=none --pad-oper --pad-header --unpad-paren --pad-comma`
- The PR description explains **how the change was tested**; if a driver was
  tested on a supported platform, add a project example that uses it.

## PR workflow

Use the fork-and-pull-request workflow when you lack write access. Request the
**analogdevicesinc/noos** team as reviewer; at least one no-OS maintainer must
approve before merge.
