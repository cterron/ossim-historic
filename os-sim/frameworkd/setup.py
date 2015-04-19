#!/usr/bin/env python

from distutils.core import setup

from ossimframework.Const import VERSION

setup (
    name            = "ossim-framework",
    version         = VERSION,
    description     = "OSSIM framework",
    author          = "OSSIM Development Team",
    author_email    = "ossim@ossim.net",
    url             = "http://www.ossim.net",
    packages        = [ 'ossimframework' ],
    scripts         = [ 'ossim-framework' ]
#    data_files      = data
)

