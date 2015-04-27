# -*- coding: utf-8 -*-
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

import api_log
import re
import json
from ansiblemethods.ansiblemanager import Ansible
from ansiblemethods.helper import ansible_is_valid_response

ansible = Ansible()


def get_plugin_list(system_ip):
    """
    Returns a hash table of plugin_name = plugin_id
    :param system_ip: System IP

    :return: On success, it returns a hash table with the correct values, otherwise it returns
    an empty dict
    """
    pluginhash = {}

    try:
        command = """executable=/bin/bash grep  "plugin_id="
                    /etc/ossim/agent/plugins/* | awk -F ":" '{print $1";"$2}'"""
        response = ansible.run_module(host_list=[system_ip], module="shell", args=command)
        response = response['contacted'][system_ip]['stdout'].split('\n')
        for line in response:
            plugin_name, plugin_id = line.split(';')
            not_used, pid = plugin_id.split('=')
            pluginhash[plugin_name] = pid
    except Exception, e:
        api_log.error("Ansible Error: get_plugin_list %s" % str(e))
    return pluginhash


def get_plugin_list_by_source_type(system_ip, source_type="log"):
    """
    Returns a hash table of plugin_name = plugin_id filtered by source type.
    :param system_ip: System IP
    :param source_type: Plugin source type. Allowed values are:
    ["command","wmi","log","database","snortlog","unix_socket","remote-log","session","http"]

    :return: On success, it returns a hash table with the correct values, otherwise it returns
    an empty dict
    """
    try:
        allowed_sources = ["command", "wmi", "log", "database",
                           "snortlog", "unix_socket", "remote-log", "session", "http"]
        plugin_hash = {}
        if source_type not in allowed_sources:
            return plugin_hash
        #Retrieve all the plugin list
        all_plugins = get_plugin_list(system_ip)

        command = """executable=/bin/bash grep  "source=%s"
                    /etc/ossim/agent/plugins/* | awk -F ":" '{print $1}'""" % source_type
        response = ansible.run_module(host_list=[system_ip], module="shell", args=command)
        response = response['contacted'][system_ip]['stdout'].split('\n')
        for line in response:
            plugin_hash[line.strip()] = all_plugins[line.strip()]
    except Exception, e:
        api_log.error("get_plugin_list_by_source_type: Error Retrieving the plugin list for a source:%s, %s"
                      % (source_type, str(e)))
    return plugin_hash


def get_plugin_enabled_by_sensor(system_ip):
    """
    Returns a has table of plugin_name = location
    """
    response = ""
    try:
        response = ansible.run_module(host_list=[system_ip], module="av_sensor", args={})
    except Exception, e:
        api_log.error("get_plugin_list_by_location: Error Retrieving the plugin list by location. %s" % e)
    return response


def get_plugin_list_by_location(system_ip, location=""):
    """
    Returns a hash table of location = plugin_id filtered by location
    :param system_ip: System IP
    :param location: Plugin location

    :return: On success, it returns a hash table with the correct values, otherwise it returns
    an empty dict
    """
    try:
        plugin_hash = {}

        #Retrieve all the plugin list
        all_plugins = get_plugin_list(system_ip)

        command = """grep  "location=.*%s.*" /etc/ossim/agent/plugins/* | awk -F ":" '{print $1}'""" % location
        response = ansible.run_module(host_list=[system_ip], module="shell", args=command)
        response = response['contacted'][system_ip]['stdout'].split('\n')
        for line in response:
            if line == "":
                continue
            plugin_hash[line.strip()] = all_plugins[line.strip()]
    except Exception, e:
        api_log.error("get_plugin_list_by_location: Error Retrieving the plugin list by location. %s" % e)
    return plugin_hash


def get_all_available_plugins(system_ip, only_detectors=False):
    """
    Returns JSON with all available plugins in the system
    @param system_ip: the host system ip
    """
    plugin_list = []
    host_list = []
    host_list.append(system_ip)
    response = ansible.run_module(host_list, "av_plugins", "")
    if system_ip in response['dark']:
        return (False, "get_all_available_plugins : " + response['dark'][system_ip]['msg'])
    else:
        for plugin, value in response['contacted'][system_ip]['data'].iteritems():
            try:
                if only_detectors and value['pname'].endswith('-monitor'):
                    continue

                plugin_list.append(value['pname'])
            except Exception:
                pass
    return (True, list(set(plugin_list)))


def get_plugin_package_version(system_ip):
    """
        Return the current version of package alienvault-plugin-sids in the system
    """
    command = """dpkg -s alienvault-plugin-sids | grep 'Version' | awk {'print $2'}"""
    response = ansible.run_module(host_list=[system_ip], module="shell", args=command)
    if system_ip in response['contacted']:
        version = response['contacted'][system_ip]['stdout'].split('\n')[0]  # Only first line
        result = (True, version)
    else:
        result = (False, str(response['dark'][system_ip]))
    return result


def get_plugin_package_info(system_ip):
    """
        If exists, return the md5sum of INSTALLED VERSION of package alienvault-plugin-sid
        IMPORTANT NOTE: The file must be called alienvault-plugin_<version>_all.deb
    """
    (success, version) = get_plugin_package_version(system_ip)
    if success:
        command = """md5sum /var/cache/apt/archives/alienvault-plugin-sids_%s_all.deb|awk {'print $1'}""" % version
        response = ansible.run_module(host_list=[system_ip], module="shell", args=command)
        if system_ip in response['contacted']:
            if response['contacted'][system_ip]['rc'] == 0:
                md5 = response['contacted'][system_ip]['stdout'].split('\n')[0]  # Only first line
            else:
                api_log.warning("Can't obtanin md5 for alienvault-plugin-sids")
                md5 = ''
            result = (True, {'version': version, 'md5': md5})
        else:
            result = (False, str(response['dark'][system_ip]))
    else:
        result = (False, "Can't obtain package version")
    return result


def ansible_check_plugin_integrity(system_ip):
    """
    Check if installed agent plugins and agent rsyslog files have been modified or removed locally
    :param system_ip: System IP

    :return: JSON with all integrity checks

    """
    rc = True
    output = {}

    try:

        #alienvault-doctor -l agent_rsyslog_conf_integrity.plg,agent_plugins_integrity.plg --output-type=ansible
        doctor_args = {}
        doctor_args['plugin_list'] = 'agent_rsyslog_conf_integrity.plg,agent_plugins_integrity.plg'
        doctor_args['output_type'] = 'ansible'

        response = ansible.run_module(host_list=[system_ip], module="av_doctor", args=doctor_args)

        success, msg = ansible_is_valid_response(system_ip, response)

        if not success:
            return False, msg

        data = response['contacted'][system_ip]
        if data['rc'] != 0:
            return False, data['stderr']

        #Parse ouptut
        pattern = re.compile('failed for \"(?P<plugin_name>[^\s]+)\"')

        output = {
            'command': data['cmd'],
            'rsyslog_integrity_check_passed': True,
            'rsyslog_files_changed': [],
            'all_rsyslog_files_installed': True,
            'rsyslog_files_removed': [],
            'plugins_integrity_check_passed': True,
            'plugins_changed': [],
            'all_plugins_installed': True,
            'plugins_removed': []
        }

        agent_rsyslog_dict = json.loads(data['data'].strip())['Agent rsyslog configuration files integrity']
        agent_plugins_dict = json.loads(data['data'].strip())['Agent plugins integrity']
        if not agent_rsyslog_dict['checks']['Default agent rsyslog files integrity']['result']:
            output['rsyslog_integrity_check_passed'] = False
            output['rsyslog_files_changed'] = pattern.findall(agent_rsyslog_dict['checks']['Default agent rsyslog files integrity']['detail'])

        if not agent_rsyslog_dict['checks']['Default agent rsyslog files installed']['result']:
            output['all_rsyslog_files_installed'] = False
            output['rsyslog_files_removed'] = pattern.findall(agent_rsyslog_dict['checks']['Default agent rsyslog files installed']['detail'])

        if not agent_plugins_dict['checks']['Default agent plugins integrity']['result']:
            output['plugins_integrity_check_passed'] = False
            output['plugins_changed'] = pattern.findall(agent_plugins_dict['checks']['Default agent plugins integrity']['detail'])

        if not agent_plugins_dict['checks']['Default agent plugins installed']['result']:
            output['all_plugins_installed'] = False
            output['plugins_removed'] = pattern.findall(agent_plugins_dict['checks']['Default agent plugins installed']['detail'])

    except Exception, e:
        response = "Error checking agent plugins and agent rsyslog files integrity: %s" % str(e)
        rc = False

    return rc, output
