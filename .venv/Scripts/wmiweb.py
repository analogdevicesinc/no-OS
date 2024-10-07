from cgi import escape
import threading
try:
  from urllib import quote
except ImportError:
  from urllib.parse import quote
from wsgiref.simple_server import make_server
from wsgiref.util import request_uri, application_uri, shift_path_info

import wmi

try:
  unicode
except NameError:
  unicode = str

doc = []

def link (text, computer, namespace=None, wmi_class=None):
  link = '<a href="/' + quote (computer)
  if namespace:
    link += "/" + quote (namespace)
  if wmi_class:
    link += "/" + quote (wmi_class)
  return link + '">' + escape (text) + '</a>'

def start_doc (title):
  doc[:] = []
  doc.append ("""
  <html>
  <head>
  <title>%(title)s</title>
  <style>
  body {font-family : Verdana, sans-serif; font-size : 84%%; margin : 3em;}
  table.items {padding-left : 30px;}
  li, td {font-family : "Courier New", monospace;}
  td {padding-right : 1em; font-size : 84%%;}
  h1, h2, h3 {font-family : Tahoma, sans-serif;}
  h2 a {text-decoration : none;}
  </style>
  </head>
  <body>
  """ % locals ())

def finish_doc ():
  doc.append ("""
  </body>
  </html>
  """ % locals ())

def doc_table (items, n_cols=3):
  n_rows, n_spare_cols = divmod (len (items), n_cols)
  doc.append ('<table cellspacing=0 class="items">')

  for n_row in range (n_rows):
    doc.append ("<tr>")
    for n_col in range (n_cols):
      doc.append ("<td><li>%s</li></td>" % items[n_cols * n_col + n_row])
    doc.append ("</tr>")

  if n_spare_cols:
    doc.append ("<tr>")
    for n_col in reversed (range (n_spare_cols)):
      doc.append ("<td><li>%s</li></td>" % items[len (items) - 1 - n_col])
    doc.append ("</tr>")

  doc.append ("</table>")

def doc_breadcrumbs (computer, namespace, wmi_class=None):
  doc.append ('<p class="breadcrumbs">')
  doc.append ('%s &rarr; %s' % (link (computer, computer), link (namespace, computer, namespace)))
  if wmi_class:
    doc.append (' &rarr; %s' % (link (wmi_class, computer, namespace, wmi_class)))
  doc.append ('</p>')

def doc_wmi_class (computer, namespace, wmi_class, wmi_connection):
  start_doc ("WMI: Class %(wmi_class)s in namespace %(namespace)s on %(computer)s" % locals ())
  doc_breadcrumbs (computer, namespace, wmi_class)
  doc.append ("<h2>%(wmi_class)s</h2>" % locals ())
  klass = getattr (wmi_connection, wmi_class)

  doc.append ("<hr>")
  doc.append ("<h3>Ancestors</h3>")
  ancestors = klass.derivation ()
  if ancestors:
    doc.append (" <b>:</b> ".join (link (ancestor, computer, namespace, ancestor) for ancestor in reversed (ancestors)))
  else:
    doc.append ("<p>No ancestors</p>")

  doc.append ("<hr>")
  doc.append ("<h3>Children</h3>")
  children = sorted (c.Path_.Class for c in klass._namespace.SubclassesOf (wmi_class))
  if children:
    doc.append ('<ul>')
    for child in children:
      doc.append ('<li>%s</li>' % link (child, computer, namespace, child))
    doc.append ('</ul>')
  else:
    doc.append ('<p>No children</p>')

  doc.append ("<hr>")
  doc.append ("<h3>Associated classes</h3>")
  associations = sorted (klass.associated_classes)
  if associations:
    doc.append ("<ul>")
    for association in associations:
      doc.append ('<li>%s</li>' % link (association, computer, namespace, association))
    doc.append ("</ul>")
  else:
    doc.append ("<p>No associated classes</p>")

  doc.append ("<hr>")
  doc.append ("<h3>Properties</h3>")
  properties = sorted (klass._properties)
  n_properties = len (properties)
  if n_properties == 0:
    doc.append ("<p>No properties</p>")
  if 1 <= n_properties <= 10:
    doc_table (properties, 1)
  elif 10 < n_properties <= 20:
    doc_table (properties, 2)
  elif 20 < n_properties <= 30:
    doc_table (properties, 3)
  else:
    doc_table (properties, 4)

  doc.append ("<hr>")
  doc.append ("<h3>Keys</h3>")
  keys = sorted (klass.keys)
  if keys:
    doc.append ("<ul>")
    for key in keys:
      doc.append ("<li>%s</li>" % key)
    doc.append ("</ul>")
  else:
    doc.append ("<p>No keys</p>")

  doc.append ("<hr>")
  doc.append ("<h3>Methods</h3>")
  methods = sorted (klass._methods)
  if methods:
    doc.append ("<ul>")
    for m in methods:
      doc.append ('<li>%s<br>%s</li>' % (m, escape (str (getattr (klass, m)))))
    doc.append ("</ul>")
  else:
    doc.append ("<p>No methods</p>")
    
  doc.append ("<hr>")
  doc.append ("<h3>Qualifiers</h3>")
  qualifiers = sorted (klass.qualifiers.items ())
  if qualifiers:
    doc.append ("<ul>")
    for q in qualifiers:
      doc.append ("<li><b>%s</b>: %s</li>" % q)
    doc.append ("</ul>")
  else:
    doc.append ("<p>No qualifiers</p>")

  finish_doc ()

def doc_namespace (computer, namespace, wmi_connection):
  start_doc ("WMI: Namespace %(namespace)s on %(computer)s" % locals ())
  doc_breadcrumbs (computer, namespace)

  namespaces = namespace.split ("\\")
  namespace_links = []
  for i, n in enumerate (namespaces):
    namespace_links.append (link (n, computer, "\\".join (namespaces[:i+1])))
  doc.append ("<h2>%s</h2>" % "\\".join (namespace_links))

  doc.append ("<hr>")
  subnamespaces = sorted (wmi_connection.__NAMESPACE ())
  doc.append ("<h3>Namespaces:</h3>")
  if subnamespaces:
    doc.append ("<ul>")
    for subnamespace in subnamespaces:
      name = subnamespace.Name
      doc.append ('<li>%s</li>' % link (name, computer, namespace + "\\" + name))
    doc.append ("</ul>")
  else:
    doc.append ("<p>No namespaces</p>")

  doc.append ("<hr>")
  subclasses = sorted (wmi_connection.classes)
  doc.append ("<h3>Classes:</h3>")
  if subclasses:
    doc.append ("<ul>")
    for subclass in subclasses:
      doc.append ('<li>%s</li>' % link (subclass, computer, namespace, subclass))
    doc.append ("</ul>")
  else:
    doc.append ("<p>No classes</p>")

  finish_doc ()

def handle_namespace (environ, computer, namespace):
  if not namespace:
    wmi_connection = wmi.WMI (computer, namespace="root/cimv2")
    for setting in wmi_connection.Win32_WMISetting ():
      namespace=setting.ASPScriptDefaultNamespace
      break

  wmi_connection = wmi.WMI (computer, namespace=namespace, find_classes=True)
  wmi_class = shift_path_info (environ)
  if wmi_class:
    doc_wmi_class (computer, namespace, wmi_class, wmi_connection)
  else:
    doc_namespace (computer, namespace, wmi_connection)

def handle_computer (environ, computer):
  handle_namespace (environ, computer, shift_path_info (environ))

def app (environ, start_response):
  computer = shift_path_info (environ)
  if computer == "favicon.ico":
    start_response ("404 Not Found", [("Content-Type", "text/plain")])
    return []
  elif computer:
    start_response ("200 OK", [("Content-Type", "text/html; charset=utf-8")])
    handle_computer (environ, computer)
    return (unicode (d).encode ("utf8") + unicode ("\n").encode ("utf8") for d in doc)
  else:
    start_response ("301 Moved Permanently", [("Location", "/localhost"), ("Content-Type", "text/plain")])
    return ["Redirected to /localhost"]

PORT = 8010

def run_browser ():
  import os
  os.startfile ("http://localhost:%d" % PORT)

if __name__ == '__main__':
  threading.Timer (3.0, run_browser).start ()
  httpd = make_server ('', PORT, app)
  try:
    httpd.serve_forever ()
  except KeyboardInterrupt:
    print ("Shutting down gracefully...")
