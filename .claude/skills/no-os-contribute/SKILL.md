---
name: no-os-contribute
description: >
  Contribution rules for the no-OS repository. Read and apply this BEFORE
  writing a commit, opening a pull request, or finalizing changes to no-OS.
  Covers required commit trailers (Assisted-by before Signed-off-by), the
  50/72 commit-message rule, the path-prefix subject convention, and the
  astyle/gcc checks maintainers enforce. Triggers whenever you are about to
  commit or contribute code to no-OS. Also invocable on demand via
  /no-os-contribute.
---

# Contributing to no-OS (agent guide)

The authoritative source is `doc/sphinx/source/contributing.rst`. Read it when
you need the full detail. This skill summarizes what an AI agent must do so a
contribution passes maintainer review on the first try.

## Commit trailers — always required

Every commit must end with, in this order:

- **`Assisted-by: <agent and model>`** — REQUIRED whenever an AI agent authored
  or modified the commit, e.g. `Assisted-by: Claude Sonnet 4.6`. Place it
  **before** `Signed-off-by`. If you are the agent making the change, add it
  yourself — do not wait to be asked.

- **`Signed-off-by: Name <email>`** — the committer certifies they have the
  rights to submit the work under the project's license. If the commit does not
  already have this trailer, add it yourself from the committer's git
  `user.name` and `user.email` (equivalently, commit with `git commit -s`).
  Use exactly what git config reports — never invent or change the identity.

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
