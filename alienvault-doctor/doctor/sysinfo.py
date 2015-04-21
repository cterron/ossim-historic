# -*- coding: utf-8 -*-
#
#  License:
#
#  Copyright (c) 2014 AlienVault
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

import sys
import platform
import re
import subprocess
import os
from datetime import timedelta

import MySQLdb, MySQLdb.cursors
import psutil
from netaddr import IPNetwork, IPAddress

from output import *
from singleton import Singleton
import default

class Sysinfo (object):
    __metaclass__ = Singleton

    def __init__ (self):
        self.__alienvault_config = {
            'version': '', \
            'versiontype': '', \
            'admin_dns': [], \
            'admin_gateway': '', \
            'admin_ip': '', \
            'admin_netmask': '', \
            'admin_network': '', \
            'hostname': '', \
            'domain': '', \
            'profiles': [], \
            'detectors': [], \
            'dbhost': '', \
            'dbuser': '', \
            'dbpass': '', \
            'connected_servers': [], \
            'connected_sensors': [], \
            'connected_systems': [], \
            'detectors': [], \
            'has_ha': False, \
            'network_interfaces': [], \
        }

        self.__system_config = {
            'os': '', \
            'node': '', \
            'kernel': '', \
            'arch': '', \
        }

        self.__hardware_config = {
            'is_vm': False, \
            'cpu': '', \
            'cores': 0, \
            'mem': 0, \
        }

        self.__system_status = {
            'uptime': '', \
            'load': '',
        }

        self.__parse_alienvault_config__()
        self.__parse_system_config__()
        self.__parse_hardware_config__()
        self.__parse_system_status__()

    # Parse alienvault configuration files and stuff.
    def __parse_alienvault_config__ (self):

        # Find software profiles.
        setup_file = open(default.ossim_setup_file, 'r').read()

        line = setup_file[(setup_file.find('\nprofile=') + 9):]
        profiles = line[:line.find('\n')].split(',')
        self.__alienvault_config['profiles'] = profiles

        if self.__alienvault_config['profiles'] == []:
            Output.error ('There are no defined profiles in ossim_setup.conf')
            sys.exit (default.error_codes['undef_ossim_profiles'])

        # Find the version.
        cmd = ['dpkg', '-l']

        for profile in self.__alienvault_config['profiles']:
            profile = profile.replace(' ', '')
            if profile == 'Server':
                cmd.append('ossim-server')
            elif profile == 'Sensor':
                cmd.append('ossim-agent')
            elif profile == 'Framework':
                cmd.append('ossim-framework')
            elif profile == 'Database':
                cmd.append('ossim-mysql')
            else:
                Output.error ('"%s" is not a valid profile' % profile)
                sys.exit (default.error_codes['invalid_ossim_profile'])

        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, err = proc.communicate()
        versions = list(set(re.findall('^ii\s+\S+\s+(?:1|10):(?P<version>\S+)-\S+\s+', output, re.MULTILINE)))

        if len(versions) != 1:
            Output.error ('Essential packages %s have different versions' % ', '.join(cmd[2:]))
#            sys.exit (default.error_codes['diff_versions_essential_packages'])

        self.__alienvault_config['version'] = versions[0]

        # Find version type (Free, Trial, Pro)
        if os.path.isfile(default.ossim_license_file):
            with open(default.ossim_license_file, 'r') as f:
                self.__alienvault_config['versiontype'] = 'TRIAL' if re.findall(r'^expire\=9999-12-31$', f.read(), re.MULTILINE) == [] else 'PRO'
        else:
            self.__alienvault_config['versiontype'] = 'FREE'

        # Find ip address, hostname and domain configured in ossim_setup.conf
        try:
            line = setup_file[(setup_file.index('admin_dns=') + 10):]
            self.__alienvault_config['admin_dns'] = line[:line.index('\n')].split(',')
            line = setup_file[(setup_file.index('admin_gateway=') + 14):]
            self.__alienvault_config['admin_gateway'] = line[:line.index('\n')]
            line = setup_file[(setup_file.index('admin_ip=') + 9):]
            self.__alienvault_config['admin_ip'] = line[:line.index('\n')]
            line = setup_file[(setup_file.index('admin_netmask=') + 14):]
            self.__alienvault_config['admin_netmask'] = line[:line.index('\n')]
            line = setup_file[(setup_file.index('hostname=') + 9):]
            self.__alienvault_config['hostname'] = line[:line.index('\n')]
            line = setup_file[(setup_file.index('domain=') + 7):]
            self.__alienvault_config['domain'] = line[:line.index('\n')]
        except ValueError:
            Output.error ('Missing network configuration bits, check your ossim_setup.conf file')
            sys.exit (default.error_codes['missing_network_config'])

        try:
            self.__alienvault_config['admin_dns'] = map(lambda x: str(IPAddress(x)), self.__alienvault_config['admin_dns'])
            self.__alienvault_config['admin_gateway'] = str(IPAddress(self.__alienvault_config['admin_gateway']))
            self.__alienvault_config['admin_ip'] = str(IPAddress(self.__alienvault_config['admin_ip']))
            self.__alienvault_config['admin_netmask'] = str(IPAddress(self.__alienvault_config['admin_netmask']))
            self.__alienvault_config['admin_network'] = str(IPNetwork('%s/%s' % (self.__alienvault_config['admin_ip'], self.__alienvault_config['admin_netmask'])))
        except Exception, msg:
            Output.error ('Invalid network configuration info, check your ossim_setup.conf file: %s' % str(msg))
            sys.exit (default.error_codes['invalid_network_config'])

        # Find MySQL properties.
        try:
            line = setup_file[(setup_file.index('\npass=') + 6):]
            self.__alienvault_config['dbpass'] = line[:line.index('\n')]
            line = setup_file[(setup_file.index('\nuser=') + 6):]
            self.__alienvault_config['dbuser'] = line[:line.index('\n')]
            line = setup_file[(setup_file.index('\ndb_ip=') + 7):]
            self.__alienvault_config['dbhost'] = line[:line.index('\n')]
        except ValueError:
            Output.error ('Missing MySQL configuration field, check your ossim_setup.conf file')
            sys.exit (default.error_codes['missing_mysql_config'])

        # Server configuration.
        if 'Server' in self.__alienvault_config['profiles']:
            try:
                conn = MySQLdb.connect(host=self.__alienvault_config['dbhost'], user=self.__alienvault_config['dbuser'], passwd=self.__alienvault_config['dbpass'], db='alienvault')
                conn.autocommit(True)
            except Exception, msg:
                Output.error ("Cannot connect to database: %s" % str(msg))
                sys.exit(default.error_codes['cannot_connect_db'])

            try:
                cursor = conn.cursor()
                cursor.execute("select inet6_ntop(IFNULL(vpn_ip, admin_ip)) as remote_server from server_forward_role inner join system on server_src_id = server_id where server_dst_id = (select unhex(replace(value, '-', '')) from config where conf = 'server_id');")
                self.__alienvault_config['connected_servers'] = list(set(x for x, in cursor.fetchall()))

                cursor.execute("select inet6_ntop(IFNULL(vpn_ip, admin_ip)) as remote_sensor from system inner join sensor on system.sensor_id = sensor.id inner join sensor_properties on sensor.id = sensor_properties.sensor_id where sensor_properties.version is not NULL;")
                self.__alienvault_config['connected_sensors'] = list(set(x for x, in cursor.fetchall()))

                self.__alienvault_config['connected_systems'] = self.__alienvault_config['connected_servers'] + self.__alienvault_config['connected_sensors']
            except:
                pass

        # Sensor configuration.
        if 'Sensor' in self.__alienvault_config['profiles']:
            try:
                line = setup_file[(setup_file.index('\ndetectors=') + 11):]
                self.__alienvault_config['detectors'] = line[:line.index('\n')].split(',')
            except ValueError:
                Output.error ('Missing Sensor configuration field, check your ossim_setup.conf file')
                sys.exit (default.error_codes['missing_sensor_config'])

        # HA configuration.
        self.__alienvault_config['has_ha'] = (re.findall(r'^ha_other_node_ip\=unconfigured$', setup_file, re.MULTILINE) == [])

        # Configured network interfaces.
        interfaces = re.findall(r'^(?:interface\=|interfaces\=)(\S+)*$', setup_file, re.MULTILINE)
        self.__alienvault_config['network_interfaces'] = list(\
                                                              reduce(\
                                                                     lambda x, y: x | y,\
                                                                     map(\
                                                                         lambda x: set(x.split(',')),\
                                                                         interfaces))) + ['lo']

    # Parse system configuration and stuff.
    def __parse_system_config__ (self):
        (self.__system_config['os'], \
         self.__system_config['node'], \
         self.__system_config['kernel'], \
         _, \
         self.__system_config['arch'], \
         _) = platform.uname()

    # Parse some hw related configuration data.
    def __parse_hardware_config__ (self):
        with open('/proc/cpuinfo', 'r') as f:
            cpuinfo = f.read()
            self.__hardware_config['is_vm'] = (re.findall(r'hypervisor', cpuinfo) != [])

        self.__hardware_config['cpu'] = platform.processor() or 'Unknown'
        self.__hardware_config['cores'] = psutil.NUM_CPUS
        self.__hardware_config['mem'] = psutil.TOTAL_PHYMEM / 1073741824

    # Parse some system status variables.
    def __parse_system_status__ (self):

        # Get uptime.
        with open('/proc/uptime', 'r') as f:
            uptime_secs = f.read().split()[0]
        uptime_td = timedelta(0, float(uptime_secs))
        self.__system_status['uptime'] = '%d day(s), %d:%d' % (uptime_td.days, uptime_td.seconds//3600, (uptime_td.seconds//60) % 60)

        # Get load average.
        try:
            loadavg = os.getloadavg()
            self.__system_status['load'] = ', '.join(map(lambda x: "%.2f" % x, loadavg))
        except OSError:
            self.__system_status['load'] = 'Unknown'

    def get_alienvault_config (self):
        return (self.__alienvault_config)

    def get_system_config (self):
        return (self.__system_config)

    def get_hardware_config (self):
        return (self.__hardware_config)

    def get_system_status (self):
        return (self.__system_status)

    def get_value (self, string):
        if string in self.__alienvault_config.keys():
            return self.__alienvault_config[string]

        if string in self.__system_config.keys():
            return self.__system_config[string]

        if string in self.__hardware_config.keys():
            return self.__hardware_config[string]

        if string in self.__system_status.keys():
            return self.__system_status[string]

        Output.warning ('Variable "%s" does not exist as a system information value' % str(string))
        return ''

    def show_platform_info (self, extended = False):
        platform_info = [
            ('AlienVault version', self.__alienvault_config['version'] + '-' + self.__alienvault_config['versiontype']), \
            ('Software profiles', ','.join(self.__alienvault_config['profiles'])), \
        ]

        platform_info_extended = [
            ('Operating system', self.__system_config ['os']), \
            ('Hostname', self.__system_config ['node']), \
            ('Kernel version', self.__system_config ['kernel']), \
            ('Architecture', self.__system_config ['arch']), \
            ('Appliance type', 'virtual' if self.__hardware_config ['is_vm'] else 'physical'), \
            ('CPU type', self.__hardware_config ['cpu']), \
            ('Number of cores', str(self.__hardware_config ['cores'])), \
            ('Installed memory', str(self.__hardware_config ['mem'])), \
            ('Uptime', self.__system_status ['uptime']), \
            ('Load', self.__system_status ['load']), \
        ]

        platform_info_server = [
            ('Connected servers', str(len(self.__alienvault_config ['connected_servers']))), \
            ('Connected sensors', str(len(self.__alienvault_config ['connected_sensors']))), \
        ]

        platform_info_sensor = [
            ('Sensor detectors', ','.join(self.__alienvault_config ['detectors'])), \
        ]

        if extended:
            platform_info += platform_info_extended

            if 'Server' in self.__alienvault_config['profiles']:
                platform_info += platform_info_server

            if 'Sensor' in self.__alienvault_config['profiles']:
                platform_info += platform_info_sensor

        for (field, value) in platform_info:
            rjustify = 60 - len (field)
            Output.emphasized ('     %s: %s' % (field, value.rjust(rjustify, ' ')), [value])
