*********************
Contributing to no-OS
*********************

Fork and pull request workflow
==============================

When you don't have write permissions to the **no-OS** repository, the
**fork and pull request** workflow can be used.

More details: `GitHub Quickstart: Contributing to projects
<https://docs.github.com/en/get-started/quickstart/contributing-to-projects>`_.

Once a pull request is opened, select the **analogdevicesinc/noos** team as a
reviewer. The changes should be approved by at least one no-OS maintainer
before they can be merged to the base brach.

Submission Checklist
====================

Before being reviewed by the no-OS maintainers and then accepted to the GitHub
repository, each commit should respect a few rules:

- The commit contains a **Signed-off-by** trailer by the committer at the end
  of the commit log message - it certifies that the committer has the rights to
  submit the work under the project's license.

- If built using **gcc**, options such as **-Wall** and **-Wextra** don't
  detect any issues with the commit. When the pull request is created, drivers
  and project build are automatically triggered.

- **astyle --style=linux --indent=force-tab=8 --max-code-length=80** doesn't
  detect any issue with the commit.

- A detailed description of how the new changes were tested is provided. This
  information can be written in the pull request description. If the driver
  was tested on one of the supported no-OS platforms, it is recommended to also
  add a project example which uses the newly added driver.
