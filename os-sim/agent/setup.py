#!/usr/bin/env python

import glob, os
from distutils.core import setup

man  = [ ('share/man/man8', ['doc/ossim-agent.8.gz']) ]
doc  = [ ('share/doc/ossim-agent', ['doc/config.dtd', 'doc/config.xml.sample', 'INSTALL', 'COPYING', 'AUTHORS'] ) ]
lib  = [ ('share/ossim-agent/pyossim/', 
    glob.glob(os.path.join('pyossim', '*.py'))) ]
data = man + doc + lib

from pyossim.__init__ import VERSION

setup (
    name            = "ossim-agent",
    version         = VERSION,
    description     = "OSSIM agent",
    author          = "OSSIM Development Team",
    author_email    = "ossim@ossim.net",
    url             = "http://www.ossim.net",
#    packages        = [ 'pyossim' ],
    scripts         = [ 'ossim-agent' ],
    data_files      = data
)

