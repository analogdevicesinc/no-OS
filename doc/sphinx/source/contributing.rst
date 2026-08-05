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

- The commit must contain a **Signed-off-by** trailer by the committer at the end
  of the commit log message. Produce it with ``git commit -s``.
- If an **AI agent** was used to author or modify the commit, add an
  **Assisted-by** trailer naming the agent and model, for example
  ``Assisted-by: Claude Sonnet 4.6``. Place it before the
  **Signed-off-by** trailer.
- The commit message should follow the **50/72 rule** :

  - The first line (the subject) should be **at most 50 characters** long.
  - The body of the commit message should wrap at **72 characters** per line.

- The commit subject should start with a **prefix** indicating the relative
  path to the file that was added or changed. This improves clarity and makes
  it easier to identify affected areas in the repository.

  Example:

  - ``dac: ad5460: add driver support``
  - ``projects: eval-adis1655x: add implementation``

- If built using **gcc**, options such as **-Wall** and **-Wextra** don't
  detect any issues with the commit. When the pull request is created, drivers
  and project build are automatically triggered.

- **astyle --style=linux --indent=force-tab=8 --max-code-length=80 --suffix=none
  --pad-oper --pad-header --unpad-paren --pad-comma** doesn't detect any issue
  with the commit.

- A detailed description of how the new changes were tested is provided. This
  information can be written in the pull request description. If the driver
  was tested on one of the supported no-OS platforms, it is recommended to also
  add a project example which uses the newly added driver.

- When adding a new project, base its ``README.rst`` on the template at
  ``doc/project_readme_template.rst`` so it follows the standard structure and
  the current CMake/no_os_build.py build conventions.

- When adding a new driver, base its ``README.rst`` on the template at
  ``doc/driver_readme_template.rst`` so it follows the standard driver
  documentation structure, and add the matching Sphinx include stub under
  ``doc/sphinx/source/drivers/<category>/``.

Licensing of contributions
==========================

To keep the no-OS core permissively licensed, new contributions must be
released under the repository's default **BSD 3-Clause** license
(``LICENSE_BSD``). Declare it by adding an
``SPDX-License-Identifier: BSD-3-Clause`` tag to every new source file;
this is the form used throughout no-OS.

- **Do not** introduce copyleft (GPL, LGPL) or proprietary code into the
  linkable library core (``util/``, ``include/``, ``iio/``, ``network/``,
  ``jesd204/``) or into general-purpose drivers.
- The only exception is **vendor-supplied device APIs** that cannot be
  relicensed (for example the ADI RF-transceiver API code). Such code
  must be confined to its own ``drivers/<category>/<part>/`` subtree, must
  carry an accurate ``SPDX-License-Identifier`` (or license header) in
  every file, and its addition must be raised with a no-OS maintainer
  before the pull request is opened.
- When adding non-permissive code under an allowed exception, update the
  top-level ``LICENSE`` file so its tables continue to list every
  non-permissive and non-BSD tree.

See :doc:`license_rules` for how licenses are declared with SPDX tags, the
meaning of the ``LICENSES/`` buckets, and the repository-local
``LicenseRef-`` identifier.
