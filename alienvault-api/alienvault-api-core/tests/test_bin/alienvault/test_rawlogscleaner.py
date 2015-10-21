#!/usr/bin/env python

from unittest import TestCase



# 
# I need to import the 
# ../../../api_core/bin/alienvault/rawlogscleaner 
# Module not easy, without breaking sime PEP
#
import os
import sys
import nose
import mock
from optparse import Values

execfile ("../../../src/bin/alienvault/rawlogscleaner")


class test_rawlogscleaner(TestCase):
    @mock.patch('ansiblemgr.Ansible.delete_raw_logs')
    @mock.patch('optparse.OptionParser.parse_args')
    def test_start(self,mock_parse_args,mock_delete_raw_logs):
        a = Values()
        a.start = "2001/11/12"
        a.end = None
        a.debug = None
        a.path= None
        mock_parse_args.return_value = (a,'')
        mock_delete_raw_logs.return_value  = (True,{'dirsdeleted':range(0,10)})
        assert logclean() == 0, "Bad return value"
        mock_delete_raw_logs.return_value = (False,{'dirsdeleted':range(0,10), 'dirserrors':range(0,20)})
        assert logclean() == -1 ,"Bad return value"

