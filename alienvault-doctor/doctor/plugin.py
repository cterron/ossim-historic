#
#  License:
#
#  Copyright (c) 2013 AlienVault
#  All rights reserved.
#
#  This package is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; version 2 dated June, 1991.
#  You may not use, modify or distribute this program under any other version
#  of the GNU General Public License.
#
#  This package is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this package; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
#  MA  02110-1301  USA
#
#
#  On Debian GNU/Linux systems, the complete text of the GNU General
#  Public License can be found in `/usr/share/common-licenses/GPL-2'.
#
#  Otherwise you can read it here: http://www.gnu.org/licenses/gpl-2.0.txt
#

import subprocess
import MySQLdb
import re
import json

from os import path

import ConfigParser
from ConfigParser import RawConfigParser
from distutils.version import LooseVersion

import dependency
from output import Output
from wildcard import Wildcard
from check import Check
from error import PluginError, PluginConfigParserError, CheckError

'''
Class PluginConfigParser.
Parses a plugin configuration file and checks it for inconsistencies.
'''
class PluginConfigParser (RawConfigParser):
  # Load a specific plugin file and returns it.
  def read (self, filename):
    try:
      RawConfigParser.read(self, filename)
      self.__check_dependencies__ (filename)
    except ConfigParser.Error, e:
      raise PluginConfigParserError ('Cannot read file %s: %s' % (filename, e))

  # Load a specific plugin file using a file descriptor and returns it.
  def readfp (self, fp):
    try:
      RawConfigParser.readfp(self, fp)
      self.__check_dependencies__ ()
    except ConfigParser.Error, e:
      raise PluginConfigParserError ('Cannot read file with descriptor %d: %s' % (fp, e))

  # Check dependencies on plugin configuration options.
  def __check_dependencies__ (self, filename):
    # Check for exclusive mandatory options.
    for section in self.sections():
      options = self.options (section)
      for moption in dependency.moptions:
        if sum ([int(x in moption) for x in options]) > 1:
          raise PluginConfigParserError ('Incompatible options in section %s' % section, filename)

    # Check for section dependencies.
    for key in dependency.sections.keys ():
      if self.has_section (key):
        deps = dependency.sections.values ()
        for dep in deps:
          for key in dep.iterkeys():
            if self.has_section (key) and key.startswith ('!'):
              raise PluginConfigParserError ('File "%s" does not met section dependency for section "%s"' % (filename, key), filename)
            elif not self.has_section (key):
              raise PluginConfigParserError ('File "%s" does not met section dependency for section "%s"' % (filename, key), filename)

            for value in dep[key]:
              if self.has_option (key, value) and value.startswith ('!'):
                raise PluginConfigParserError ('File "%s" does not met option dependency for section "%s"' % (filename, key), filename)
              elif not self.has_option (key, value):
                raise PluginConfigParserError ('File "%s" does not met option dependency for section "%s"' % (filename, key), filename)

    # Check for option dependencies.
    # TODO


'''
Plugin class.
Defines a plugin with a set of conditions/actions.
'''
class Plugin:
  def __init__ (self, filename, alienvault_config, severity_list, verbose, raw):

    # Common properties.
    self.__config_file = None
    self.__alienvault_config = {}
    self.__severity_list = []
    self.__verbose = 0
    self.__raw = False
    self.__name = ''
    self.__description = ''
    self.__type = ''
    self.__category = []
    self.__requires = []
    self.__profiles = []
    self.__cutoff = False
    self.__raw_limit = 0

    # 'file' type properties.
    self.__filename = ''

    # 'command' type properties.
    self.__command = ''

    # Shared properties for 'file' and 'command' types.
    self.__data = ''
    self.__data_len = ''

    # 'db' type properties.
    self.__host = ''
    self.__user = ''
    self.__password = ''
    self.__database = ''
    self.__db_conn = None
    self.__db_cursor = None

    # Plugin defined checks.
    self.__checks = []

    # Check for file extension.
    if not filename.endswith ('.plg'):
      raise PluginError ('File extension is not .plg', filename)

    self.__alienvault_config = alienvault_config
    self.__severity_list = severity_list
    self.__verbose = verbose
    self.__raw = raw

    try:
      # Parse the plugin configuration file.
      self.__config_file = PluginConfigParser ()
      self.__config_file.read (filename)
    except PluginConfigParserError:
      raise
    except Exception as e:
      raise PluginError ('Cannot parse plugin file "%s": %s' % (filename, str(e)), filename)

    try:
      self.__name = self.__config_file.get ('properties', 'name')
      self.__description = self.__config_file.get ('properties', 'description')
      self.__type = self.__config_file.get ('properties', 'type')
      self.__category = self.__config_file.get ('properties', 'category').split(',')

      if self.__verbose > 0:
        Output.emphasized ('\nRunning plugin "%s"...\n' % self.__name, [self.__name])

      if self.__config_file.has_option('properties', 'requires'):
        self.__requires = self.__config_file.get('properties', 'requires').split(';')
        self.__check_requirements__ ()

      # Check for the 'cutoff' option
      if self.__config_file.has_option('properties', 'cutoff'):
        self.__cutoff = eval(self.__config_file.get ('properties', 'cutoff'))

      # Check for the 'limit' option (in kbytes), used in combination with the raw data output. '0' means no limit.
      if self.__raw and self.__config_file.has_option('properties', 'raw_limit'):
        self.__raw_limit = int(self.__config_file.get ('properties', 'raw_limit'))

      # Check for profile & version where this plugin is relevant.
      if self.__config_file.has_option('properties', 'profiles'):
        profiles_versions = self.__config_file.get ('properties', 'profiles')
        self.__profiles = [(x.partition(':')[0], x.partition(':')[2]) for x in profiles_versions.split(';')]

        for (profile, version) in self.__profiles:
          if profile == '' or version == '':
            raise PluginError ('Empty profile or version in "profiles" field', self.__name)

          if not profile in self.__alienvault_config['profiles']:
            raise PluginError ('Profile "%s" does not match installed profiles' % profile, self.__name)

          if version.startswith('>'):
            ret = LooseVersion(self.__alienvault_config['version']) > LooseVersion (version[1:])
          elif version.startswith('<'):
            ret = LooseVersion(self.__alienvault_config['version']) < LooseVersion (version[1:])
          elif version.startswith('=='):
            ret = LooseVersion(self.__alienvault_config['version']) == LooseVersion (version[2:])
          elif version.startswith('!='):
            ret = LooseVersion(self.__alienvault_config['version']) != LooseVersion (version[2:])
          else:
            raise PluginError ('Profile "%s" version does not match installed profiles' % profile, self.__name)

      # Ugly...
      if self.__type == 'file':
        self.__init_file__ ()
      elif self.__type == 'command':
        self.__init_command__ ()
      elif self.__type == 'db':
        self.__init_db__ ()
      else:
        raise PluginError ('Unknown type', self.__name)

      # Parse 'check' sections.
      sections = self.__config_file.sections()
      for section in sections:
        if section != 'properties':
          check = Check (self, section)
          if check.check_severity (self.__severity_list):
            self.__checks.append (check)
          else:
            del check

    except PluginError:
      raise

    except PluginConfigParserError:
      raise

    except CheckError:
      raise

    except Exception, msg:
      raise PluginError ('Cannot initialize plugin: %s' % str(msg), filename)

  # Check for plugin requirements.
  # Currently, requirement types could be 'modules', 'files', 'dpkg', 'hardware'.
  def __check_requirements__ (self):
    for requirement in self.__requires:
      req_type, req_data = requirement.split(':')
      req_data_split = req_data.split(',')

      if req_type == '@modules':
        # Would be nice to rewrite this using python-kmod
        # https://github.com/agrover/python-kmod
        for req_module in req_data_split:
          command = 'lsmod|grep ' + req_module
          proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
          data, err = proc.communicate()
          if data == '':
            raise PluginError ('Required module "%s" is not present' % req_module, self.__name)
      elif req_type == '@files':
        for req_file in req_data_split:
          if not path.exists(req_file):
            raise PluginError ('Required file "%s" does not exist' % req_file, self.__name)
      elif req_type == '@dpkg':
        for req_pkg in req_data_split:
          command = 'dpkg -l ' + req_pkg
          proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
          data, err = proc.communicate()
          if err != '':
            raise PluginError ('Required package "%s" is not installed' % req_pkg, self.__name)
      elif req_type == '@hardware':
        self.__check_hardware_requirements__ (req_data_split)
      else:
        raise PluginError ('Unknown requirement type: %s' % req_type, self.__name)

  # Check for hardware requirements.
  # Items in the list are defined this way: hardware/attribute/value
  def __check_hardware_requirements__ (self, hw_list):
    # Check hardware requirements.
    for hw_req in hw_list:
      (hw_req_pretty, eval_str) = Wildcard.hw_config(hw_req)
      try:
        if not eval(eval_str):
          raise PluginError (hw_req_pretty, self.__name)
      except PluginError:
        raise
      except:
        raise PluginError ('Expression "%s" cannot be evaluated' % eval_str, self.__name)

  # Initialize 'file' type plugin properties.
  def __init_file__ (self):
    self.__filename = self.__config_file.get ('properties', 'filename')

    try:
      fp = open (self.__filename, 'r')
      self.__data = fp.read ()
      self.__data_len = len (self.__data)
    except Exception as e:
      raise PluginError ('Cannot parse file "%s": %s' % (self.__filename, e), self.__name)

  # Initialize 'command' type plugin properties.
  def __init_command__ (self):
    self.__command = self.__config_file.get ('properties', 'command')
    self.__command = Wildcard.av_config(self.__command)

    try:
      proc = subprocess.Popen(self.__command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      self.__data, err = proc.communicate()
      self.__data_len = len (self.__data)
    except Exception as e:
      raise PluginError ('Cannot run command "%s": %s' % (self.__command, e), self.__name)

  # Initialize 'db' type plugin properties.
  def __init_db__ (self):
    self.__host = Wildcard.av_config(self.__config_file.get ('properties', 'host'))
    self.__user = Wildcard.av_config(self.__config_file.get ('properties', 'user'))
    self.__password = Wildcard.av_config(self.__config_file.get('properties', 'password'))
    self.__database = self.__config_file.get ('properties', 'database')

    try:
      self.__db_conn = MySQLdb.connect (host=self.__host, user=self.__user, passwd=self.__password, db=self.__database)
      self.__db_cursor = self.__db_conn.cursor()
    except Exception as e:
      raise PluginError ('Cannot connect to database: %s' % e, self.__name)

  # Getter/setter methods.
  def get_name (self):
    return self.__name

  def get_description (self):
    return self.__description

  def get_category (self):
    return self.__category

  def get_config_file (self):
    return self.__config_file

  def get_data (self):
    return self.__data

  def get_checks_len (self):
    return len(self.__checks)

  def get_filename (self):
    return self.__filename

  # Check if any of the categories match with the plugin ones.
  def check_category (self, categories):
    # Treat the empty list as 'all'
    if categories == []:
      return True

    # Search for the 'all' wildcard.
    if 'all' in categories:
      return True

    for category in self.__category:
      if category in categories:
        return True
    return False

  # Run a query with the userdata placed in the plugin.
  def run_query (self, query, result = False):
    try:
      self.__db_cursor.execute (query)
    except Exception as e:
      Output.warning ('Cannot run query for plugin "%s": %s' % (self.__name, e))
      return []

    if result:
      try:
        rows = self.__db_cursor.fetchall ()
      except Exception as e:
        Output.warning ('Cannot run query for plugin "%s": %s' % (self.__name, e))
        return []

      self.__data += query + '\n' + str(rows) + '\n\n'
      return list(rows)

    return []

  # Run checks.
  def run (self):
    total = 0
    passed = 0

    json_msg = {'plugin': self.__name, 'description': self.__description, 'checks': {}}

    if self.__raw:
      json_msg['source'] = unicode(self.__data[-(self.__raw_limit * 1024):], errors='replace')

    for check in self.__checks:
      total += 1
      result, msg = check.run()

      if not result:
        if self.__verbose == 2:
          Output.error ('Check "%s" failed: %s' % (check.get_name(), msg))
        elif self.__verbose == 1:
          Output.error ('Check "%s" failed' % check.get_name())

        json_msg['checks'][check.get_name()] = {'result': False, 'severity': check.get_severity(), 'warning': check.get_warning(), 'advice': check.get_advice(), 'detail': msg.lstrip().replace('\n\t', ';')}

        # Exit the loop if one check has failed.
        if self.__cutoff:
          break
      else:
        if self.__verbose > 0:
          Output.info ('Check "%s" passed' % check.get_name())
        json_msg['checks'][check.get_name()] = {'result': True}
        passed += 1

    if self.__verbose > 0:
      if passed == total:
        Output.emphasized ('\nAll checks passed for plugin "%s".' % self.__name, [self.__name])
      else:
        Output.emphasized ('\n%d out of %d checks passed for plugin "%s".' % (passed, total, self.__name), [self.__name])

    return (json_msg)
