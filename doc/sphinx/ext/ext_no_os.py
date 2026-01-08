from os import path, environ
from time import time
from urllib.request import urlopen

from lxml import html
from docutils import nodes
from sphinx.util.docutils import Directive
from sphinx.util import logging
from sphinx.util.osutil import SEP
from adi_doctools.directive.node import node_a

logger = logging.getLogger(__name__)

base_url_doc = "https://analogdevicesinc.github.io/no-OS/doxygen/"

def manage_no_os_doxygen_links(env):
    """
    Manages the doxygen drivers/projects inventory.
    Only parses on source changes, persistent between Sphinx runs.
    """
    pages = ["drivers", "projects"]

    if not hasattr(env, 'no_os_doxygen'):
        env.no_os_doxygen = {}
        for p in pages:
            env.no_os_doxygen[p] = {'ctime': 0, 'list': None,
                                    'exclude': [], 'ctime_exclude': 0,
                                    'try_file': True, 'try_fetch': True}

    dxy = env.no_os_doxygen

    if 'ADOC_CUSTOM_DOC' in environ:
        root_path = path.abspath(path.join(env.srcdir, "..", "no-OS"))
    else:
        root_path = path.abspath(path.join(env.srcdir, "..", "..", ".."))

    def get_exclusions(file):
        if not path.isfile(file):
            logger.warning(f"no-os-doxygen: '{file}' does not exist!")
            return

        ctime__ = path.getctime(file)
        if ctime__ <= dxy['drivers']['ctime_exclude']:
            return

        with open(file, 'r') as f:
            for line in f:
                line = line.strip()
                if line.startswith("EXCLUDE_DRV="):
                    exclude = line.split("=", 1)[1].strip().strip('"').split()
                    dxy['drivers']['exclude'] = exclude
                    dxy['drivers']['ctime_exclude'] = ctime__
                    return
        return

    get_exclusions(path.join(root_path, "ci", "gen_dox.sh"))

    base_path = path.join(root_path, "doc", "doxygen", "build",
                          "doxygen_doc", "html")

    def read_file(p, file: tuple, ctime_):
        if not dxy[p]['try_file']:
            return None, None, False

        file = path.join(base_path, SEP.join(file))
        if not path.isfile(file):
            logger.info(f"no-os-doxygen: '{file}' does not exist.")
            dxy[p]['try_file'] = False
            return None, None, False

        ctime__ = path.getctime(file)
        if ctime__ <= ctime_:
            return None, None, True

        with open(file, 'r') as f:
            content = ''.join(f.readlines())
            return content, ctime__, False

    def fetch_file(p, file: tuple, ctime_):
        if not dxy[p]['try_fetch']:
            return None, None, False

        url = base_url_doc + '/'.join(file)
        ctime__ = int(time())
        if ctime__ <= ctime_:
            return None, None, True

        try:
            logger.info(f"loading doxygen entries from {url} ...")
            with urlopen(url) as response:
                content = response.read().decode('utf-8')
                ctime__ = ctime__ + 600 # Slack future
        except Exception as e:
            logger.warning(f"no-os-doxygen: Failed to fetch '{url}', {e}")
            dxy[p]['try_fetch'] = False
            return None, None, False

        return content, ctime__, False

    for p in pages:
        file = (f"{p}_page.html",)
        content, ctime, cached = read_file(p, file, dxy[p]['ctime'])
        if cached:
            continue
        if content is None:
            content, ctime, cached = fetch_file(p, file, dxy[p]['ctime'])
        if cached or content is None:
            continue

        root = html.fromstring(content)

        dxy[p]['list'] = {}
        link_elements = root.xpath("//body//div[@class='textblock']//ul")

        if p == "drivers":
            for le in link_elements:
                prev = le.getprevious()
                if prev.tag == "h1":
                    title = ''.join(prev.itertext()).strip().lower()
                else:
                    logger.warning(f"no-os-doxygen: List in '{file}'"
                                   " missing title h1.")
                    continue

                links = le.xpath("./li/a")
                links_ = [[lk.text.lower(), lk.attrib['href']] for lk in links]

                dxy[p]['list'][title] = dict(links_)
        else:
            if len(link_elements) != 1:
                logger.warning(f"no-os-doxygen: number of lists for '{p}'"
                               f" is 1, got {len(link_elements)}.")
                continue

            links = link_elements[0].xpath("./li/a")
            links_ = [[lk.text.lower(), lk.attrib['href']] for lk in links]
            dxy[p]['list'] = dict(links_)

        dxy[p]['ctime'] = ctime


class directive_no_os_doxygen(Directive):
    """
    Adds a link to the Doxygen documentation using the format:

      See <path> for the Doxygen documentation.

    If no argument is provided, infer the project/driver path from the docname.
    If an argument is supplied, use it instead, for example:

       .. no-os-doxygen:: projects/ad7091r8-sdz

    """
    has_content = False
    add_index = True
    current_doc = ''
    final_argument_whitespace = True
    required_arguments = 0
    optional_arguments = 1

    def run(self):
        env = self.state.document.settings.env
        self.current_doc = env.doc2path(env.docname)

        manage_no_os_doxygen_links(env)

        if len(self.arguments) == 1:
            lf = self.arguments[0].strip().split('/')
        else:
            # Must recover category
            k = env.docname.split('/')[0]
            with open(self.current_doc, 'r') as file:
                for lk in file:
                    lk = lk.strip()
                    lk = lk[lk.find(k+'')+len(k+'/'):]
                    lk = lk.replace("/README.rst", "").split('/')
                    if len(lk) == 1 and k == "drivers":
                        lk = [lk[0], lk[0]]
                    break
            lf = [k, *lk]

        if 'ADOC_CUSTOM_DOC' in environ:
            lf.pop(0)
        if lf[0] not in env.no_os_doxygen:
            logger.warning(f"no-os-doxygen: Entry '{lf[0]}' from doc"
                           f" '{env.docname}' doesn't exist.")
            return []

        if env.no_os_doxygen[lf[0]]['list'] is None:
            return []

        if lf[1] not in env.no_os_doxygen[lf[0]]['list']:
            if lf[1] not in env.no_os_doxygen[lf[0]]['exclude']:
                logger.warning(f"no-os-doxygen: Entry '{lf[1]}' doesn't exist"
                               f" in '{lf[0]}'")
            return []

        elif (len(lf) == 3 and
              lf[2] not in env.no_os_doxygen[lf[0]]['list'][lf[1]]):
            if lf[2] not in env.no_os_doxygen[lf[0]]['exclude']:
                logger.warning(f"no-os-doxygen: Entry '{lf[2]}' doesn't exist"
                               f" in '{lf[0]}/{lf[1]}'")
            return []

        if lf[0] == "drivers":
            dir_ = env.no_os_doxygen[lf[0]]['list'][lf[1]][lf[2]]
        else:
            dir_ = env.no_os_doxygen[lf[0]]['list'][lf[1]]

        node = nodes.paragraph()

        content_root = (f'..{SEP}' * env.docname.count(SEP)) or f'.{SEP}'
        if 'ADOC_CUSTOM_DOC' in environ:
            path_ = base_url_doc + dir_
        else:
            path_ = path.join(content_root, 'doxygen', dir_)
        url = node_a(href=path_,
                     classes=["reference", "external"])
        url += nodes.inline(text='/'.join(lf)+" (doxygen)")
        node += nodes.inline(text="See ")
        node += url
        node += nodes.inline(text=" for the Doxygen documentation.")

        return [node]


def setup(app):
    app.add_directive('no-os-doxygen', directive_no_os_doxygen)

    return {
        "parallel_read_safe": True,
        "parallel_write_safe": True,
        "version": "0.1",
    }
