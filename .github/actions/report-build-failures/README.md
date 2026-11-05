# Report build failures

A composite action that reports the failed project builds of a `build-*-platform`
workflow job on the pull request. It is called once per platform job, as the last
step after the build logs have been uploaded.

## What it does

1. **Collects the failures.** Runs `.github/scripts/report-build-failures.py` on the
   log directory. The script reads `build_failures.json` (written by
   `tools/scripts/build_projects.py`) and the logs next to it.
2. **Uploads the failed logs.** Writes `<name>_failed_builds.html`, a self-contained
   page with the full log of every failed combination, and uploads it as the
   `<name>-failed-builds` artifact. The file is not zipped, so it opens directly in
   the browser. If nothing failed, no file is written and nothing is uploaded.
3. **Renders the PR comment.** Writes `build_failures.md` with the affected projects
   and an error excerpt for each failed combination. It links to the failed-logs
   artifact and to the full logs artifact. Its footer names the PR head commit the
   report was created for. The file is empty when nothing failed.
   It is not written when `build_failures.json` is missing, which means the build
   did not complete, for example because a newer push cancelled the run.
4. **Posts the PR comment.** Calls
   [`analogdevicesinc/shared-actions/pr-comment`](https://github.com/analogdevicesinc/shared-actions)
   with the marker `build-failures-<name>`. The action first deletes the previous
   comment, then posts the new report as a fresh comment, so the latest report
   always appears at the bottom of the conversation. An empty body only deletes.
   When step 3 wrote no file, both calls are skipped and any existing comment is
   left as it is.

Steps 2–4 run only on `pull_request` events, except the upload, which always runs.
The collect step runs on every event.

## Inputs

| Input      | Required | Default | Description |
|------------|----------|---------|-------------|
| `platform` | yes      |         | Platform label shown in the report (`--platform`). |
| `name`     | yes      |         | Slug for the file names and the PR comment marker. Must be unique per job of a workflow run. |
| `log-dir`  | yes      |         | Directory the build logs were written to. |
| `logs-url` | no       | `''`    | URL of the uploaded full build logs artifact, linked from the PR comment. |

## Usage

Place the call after the step that uploads the full build logs, because `logs-url`
uses that step's output:

```yaml
- name: Upload build logs
  id: upload-logs
  if: always()
  uses: actions/upload-artifact@v7
  with:
    path: ${{ env.LOGS_DIR }}
    name: ${{ github.job }}-logs

- name: Report build failures
  if: always()
  uses: ./.github/actions/report-build-failures
  with:
    platform: "xilinx"
    name: "xilinx"
    log-dir: ${{ env.LOGS_DIR }}
    logs-url: ${{ steps.upload-logs.outputs.artifact-url }}
```

For a matrix job, include the matrix value in `name` so each combination has its own
file and marker, for example `platform: "mac (${{ matrix.runs-on }})"` and
`name: "mac-${{ matrix.runs-on }}"`.

## Requirements

- The repository must be checked out first, because the action runs
  `.github/scripts/report-build-failures.py` and is referenced by a local path.
- `python3` must be on the runner's `PATH`.
- The job needs `pull-requests: write` permission for the PR comment step.
- The step runs in `bash`, on Linux, macOS, and Windows runners.

## Related files

- [`.github/scripts/report-build-failures.py`](../../scripts/report-build-failures.py): the reporter.
- [`tools/scripts/build_projects.py`](../../../tools/scripts/build_projects.py): records the failed builds.
- [`.github/workflows/`](../../workflows/): the `build-*-platform.yaml` workflows that call this action.
