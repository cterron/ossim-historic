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
"""
    Implementation of system monitors
        MonitorSystemCPULoad
        MonitorDiskUsage
        MonitorSystemDNS
        MonitorRemoteCertificates
        MonitorRetrievesRemoteInfo
        MonitorPendingUpdates
"""

import traceback
import celery.utils.log
import time
import datetime
import os
from api.lib.monitors.monitor import Monitor, MonitorTypes, ComponentTypes
from ansiblemethods.system.system import (
    get_root_disk_usage,
    get_system_load,
    get_av_config,
    ping_system,
    ansible_download_release_info
)
from ansiblemethods.system.maintenance import system_reboot_needed
from ansiblemethods.system.about import get_is_professional
from ansiblemethods.system.support import check_support_tunnels
from apimethods.system.network import dns_is_external
from db.methods.data import get_asset_list
from db.methods.system import (
    get_systems,
    set_system_vpn_ip,
    set_system_ha_ip,
    set_system_ha_role,
    get_system_ip_from_local,
    get_system_id_from_local,
    db_system_update_hostname,
    db_get_hostname,
    set_system_ha_name,
    fix_system_references,
    check_any_innodb_tables,
    get_config_otx_enabled,
    get_wizard_data,
    get_trial_expiration_date,
    check_backup_process_running
)
from db.methods.sensor import (
    get_sensor_id_from_system_id,
    set_sensor_properties_active_inventory,
    set_sensor_properties_passive_inventory,
    set_sensor_properties_netflow,
    check_any_orphan_sensor
)
from apimethods.sensor.sensor import get_plugins_from_yaml
from apimethods.system.config import (
    get_system_config_general,
    get_system_config_alienvault
)
from apimethods.system.network import get_interfaces
from apimethods.system.system import (
    get as system_get,
    apimethod_get_update_info,
    system_is_trial,
    system_is_professional,
    get_license_devices
)
from apimethods.system.support import status_tunnel
from apimethods.system.backup import get_backup_list

from apimethods.system.cache import flush_cache
from apimethods.utils import is_valid_ipv4
from apimethods.system.status import system_all_info, network_status, alienvault_status
import api_log
from api.lib.mcenter import get_message_center_messages
from apimethods.data.status import load_external_messages_on_db
from apimethods.data.status import insert_current_status_message
from api.lib.monitors.messages import MessageReader

messages = MessageReader()
logger = celery.utils.log.get_logger("celery")


class MonitorSystemCPULoad(Monitor):
    def __init__(self):
        Monitor.__init__(self, MonitorTypes.MONITOR_CPU_LOAD)
        self.message = 'System CPU Load monitor started'

    def start(self):
        """
            Starts the monitor activity
        """
        rt = True
        self.remove_monitor_data()
        # Load all system from current_local
        logger.info("Checking systems cpu load")
        result, systems = get_systems()
        if not result:
            logger.error("Can't retrieve the system info: %s" % str(systems))
            return False
        for (system_id, system_ip) in systems:
            (result, load) = get_system_load(system_ip)
            if result:
                try:
                    logger.info("CPU Load: %s %f" % (system_ip, load))
                    monitor_data = {"cpu_load": load}
                    self.save_data(system_id, ComponentTypes.SYSTEM, self.get_json_message(monitor_data))
                except Exception:
                    logger.error("Error==>> " + traceback.format_exc())
                    rt = False
                    break
            else:
                logger.error("MonitorSystemCPULoad: %s" % load)
                rt = False
                break
        return rt


class MonitorDiskUsage(Monitor):
    """
    Monitor disk usage in the local server.
    """

    def __init__(self):
        Monitor.__init__(self, MonitorTypes.MONITOR_DISK_SPACE)
        self.message = 'Disk Usage Monitor Enabled'

    def start(self):
        """
        Starts the monitor activity

        :return: True on success, False otherwise
        """
        self.remove_monitor_data()
        # Find the local server.
        rc, system_list = get_systems()
        if not rc:
            logger.error("Can't retrieve systems..%s" % str(system_list))
            return False
        args = {}
        args['plugin_list'] = 'disk_usage.plg'
        args['output_type'] = 'ansible'
        for (system_id, system_ip) in system_list:
            dummy_result, ansible_output = get_root_disk_usage(system_ip)

            if not self.save_data(system_id, ComponentTypes.SYSTEM,
                                  self.get_json_message(
                                      {'disk_usage': ansible_output})):
                logger.error("Can't save monitor info")
        return True


class MonitorSystemDNS(Monitor):
    """
        Monitor the current system DNS.
    """
    def __init__(self):
        Monitor.__init__(self, MonitorTypes.MONITOR_SYSTEM_DNS)
        self.message = "Monitor the current system DNS"

    def start(self):
        self.remove_monitor_data()
        rc, system_list = get_systems()
        if not rc:
            logger.error("Can't retrieve systems..%s" % str(system_list))
            return False

        for (system_id, system_ip) in system_list:
            # Use ansible to get the DNS config.
            result, ansible_output = get_av_config(system_ip, {'general_admin_dns': ''})
            logger.info("DNS returned from ossim_setup.conf %s" % str(ansible_output))
            if result:
                dnslist = []
                if 'general_admin_dns' in ansible_output:
                    dnslist = ansible_output['general_admin_dns'].split(',')
                count = 0
                for ip in dnslist:
                    r = dns_is_external(ip)
                    if r == -2:
                        count += 1
                    elif r == -1:
                        logger.error("Bad data in admin_dns field of ossim_setup.conf: " + str(ip))
                # logger.info("DNS IP count = " + str(count))
                if count == len(dnslist):
                    admin_dns_msg = "Warning: All DNS configured are externals"
                    self.save_data(system_id, ComponentTypes.SYSTEM,
                                   self.get_json_message(
                                       {'admin_dns': admin_dns_msg,
                                        'internal_dns': False}))
                else:
                    self.save_data(system_id, ComponentTypes.SYSTEM,
                                   self.get_json_message({'admin_dns': 'DNS ok. You have at least one internal DNS',
                                                          'internal_dns': True}))

            else:
                if not self.save_data(system_id, ComponentTypes.SYSTEM,
                                      self.get_json_message({'admin_dns': 'Error: %s' % str(ansible_output),
                                                             'internal_dns': True})):
                    logger.error("Can't save monitor info")
        return True


class MonitorRemoteCertificates(Monitor):
    """
        Monitor the remote certificates.
    """
    def __init__(self):
        Monitor.__init__(self, MonitorTypes.MONITOR_REMOTE_CERTIFICATES)
        self.message = "Monitor the remote certificates"

    def start(self):
        self.remove_monitor_data()
        rc, system_list = get_systems()
        if not rc:
            logger.error("Can't retrieve systems..%s" % str(system_list))
            return False
        for (system_id, system_ip) in system_list:
            result, ansible_output = ping_system(system_ip)
            if not result:
                # Check whether is sensor or not
                sensor, sensor_id = get_sensor_id_from_system_id(system_id)
                if not self.save_data(system_id, ComponentTypes.SYSTEM,
                                      self.get_json_message({'remote_certificates': 'Error: %s' % str(ansible_output),
                                                             'contacted': False,
                                                             'is_sensor': sensor})):
                    logger.error("Can't save monitor info")
            else:
                self.save_data(system_id, ComponentTypes.SYSTEM,
                               self.get_json_message({'remote_certificates': 'Ping OK',
                                                      'contacted': True}))
        return True


class MonitorRetrievesRemoteInfo(Monitor):
    """
        Monitor the remote certificates.
    """
    def __init__(self):
        Monitor.__init__(self, MonitorTypes.MONITOR_GET_REMOTE_SYSTEM_INFO)
        self.message = "Monitor: Get remote system information"

    def start(self):
        try:
            self.remove_monitor_data()
            rc, system_list = get_systems(directly_connected=False)
            if not rc:
                logger.error("Can't retrieve systems..%s" % str(system_list))
                return False

            for (system_id, system_ip) in system_list:
                success, sensor_id = get_sensor_id_from_system_id(system_id)
                if not success:
                    logger.warning("[MonitorRetrievesRemoteInfo] get_sensor_id_from_system_id failed for system %s (%s)" % (system_ip, system_id))
                    continue

                if sensor_id is not None and sensor_id != '':
                    success, result = get_plugins_from_yaml(sensor_id, no_cache=True)
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] get_plugins_from_yaml failed for system %s (%s)" % (system_ip, system_id))
                        continue
                ha_name = None
                success, result = system_all_info(system_id, no_cache=True)
                if not success:
                    logger.warning("[MonitorRetrievesRemoteInfo] system_all_info failed for system %s (%s)" % (system_ip, system_id))
                    continue
                if 'ha_status' in result:
                    ha_name = 'active' if result['ha_status'] == 'up' else 'passive'
                success, result = network_status(system_id, no_cache=True)
                if not success:
                    logger.warning("[MonitorRetrievesRemoteInfo] network_status failed for system %s (%s)" % (system_ip, system_id))
                    continue
                success, result = alienvault_status(system_id, no_cache=True)
                if not success:
                    logger.warning("[MonitorRetrievesRemoteInfo] alienvault_status failed for system %s (%s)" % (system_ip, system_id))
                    continue
                success, result = status_tunnel(system_id, no_cache=True)
                if not success:
                    logger.warning("[MonitorRetrievesRemoreInfo] status_tunnel failed for system %s (%s)" % (system_ip, system_id))
                    continue
                success, result = get_system_config_general(system_id, no_cache=True)
                if not success:
                    logger.warning("[MonitorRetrievesRemoteInfo] get_system_config_general failed for system %s (%s)" % (system_ip, system_id))
                    continue
                                # Here we have the hostname
                hostname = result.get('general_hostname', None)
                if hostname is not None:
                    success, hostname_old = db_get_hostname(system_id)
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] db_get_hostname failed for system %s (%s)" % (system_ip, system_id))
                        continue
                    if hostname == hostname_old:
                        hostname = None

                # Getting config params from the system,
                # we do use this result var so do not change the order of the calls!
                success, result = get_system_config_alienvault(system_id, no_cache=True)
                if not success:
                    logger.warning("[MonitorRetrievesRemoteInfo] get_system_config_alienvault failed for system %s (%s)" % (system_ip, system_id))
                    continue

                prads_enabled = False
                suricata_snort_enabled = False
                netflow_enabled = False
                ha_ip = None
                ha_role = None

                if 'sensor_detectors' in result:
                    prads_enabled = True if 'prads' in result['sensor_detectors'] else False
                    suricata_snort_enabled = True if 'snort' in result['sensor_detectors'] or 'suricata' in result['sensor_detectors'] else False
                if 'sensor_netflow' in result:
                    netflow_enabled = True if result['sensor_netflow'] == 'yes' else False

                if 'ha_ha_virtual_ip' in result:
                    ha_ip = result['ha_ha_virtual_ip']
                    if not is_valid_ipv4(ha_ip):
                        ha_ip = None

                if 'ha_ha_role' in result:
                    ha_role = result['ha_ha_role']
                    if ha_role not in ['master', 'slave']:
                        ha_role = None

                success, result = get_interfaces(system_id, no_cache=True)
                if not success:
                    continue
                success, result = system_get(system_id, no_cache=True)
                if not success:
                    continue

                vpn_ip = None
                if "ansible_tun0" in result:
                    try:
                        vpn_ip = result['ansible_tun0']['ipv4']['address']
                    except Exception:
                        vpn_ip = None

                # TO DB; vpn_ip, netflow, active inventory, passive inventory
                # ha_ip
                if sensor_id is not None and sensor_id != '':
                    success, message = set_sensor_properties_active_inventory(sensor_id, suricata_snort_enabled)
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] set_sensor_properties_active_inventory failed: %s" % message)
                    success, message = set_sensor_properties_passive_inventory(sensor_id, prads_enabled)
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] set_sensor_properties_pasive_inventory failed: %s" % message)
                    success, message = set_sensor_properties_netflow(sensor_id, netflow_enabled)
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] set_sensor_properties_netflow failed: %s" % message)

                if vpn_ip is not None:
                    success, message = set_system_vpn_ip(system_id, vpn_ip)
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] set_system_vpn_ip failed: %s" % message)

                if ha_role is not None:
                    success, message = set_system_ha_role(system_id, ha_role)
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] set_system_ha_role failed: %s" % message)
                else:
                    success, message = set_system_ha_role(system_id, 'NULL')
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] set_system_ha_role failed: %s" % message)

                if ha_ip is not None:
                    success, message = set_system_ha_ip(system_id, ha_ip)
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] set_system_ha_ip: %s" % message)
                    success, message = fix_system_references()
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] fix_system_references: %s" % message)
                    if ha_name is not None:
                        success, message = set_system_ha_name(system_id, ha_name)
                        if not success:
                            logger.warning("[MonitorRetrievesRemoteInfo] set_system_ha_name failed: %s" % message)
                else:
                    success, message = set_system_ha_ip(system_id, '')
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] set_system_ha_ip failed: %s" % message)

                if hostname is not None:
                    success, message = db_system_update_hostname(system_id, hostname)
                    if not success:
                        logger.warning("[MonitorRetrievesRemoteInfo] db_system_update_hostname failed: %s" % message)

                # Backups
                success, message = get_backup_list(system_id=system_id,
                                                   backup_type="configuration",
                                                   no_cache=True)
                if not success:
                    logger.warning("[MonitorRetrievesRemoteInfo] get_backup_list failed: %s" % message)

        except Exception as err:
            api_log.error("Something wrong happened while running the MonitorRetrievesRemoteInfo monitor %s" % str(err))
            return False
        return True


class MonitorPendingUpdates(Monitor):
    """ Monitor for pending updates

    """
    def __init__(self):
        Monitor.__init__(self, MonitorTypes.MONITOR_PENDING_UPDATES)
        self.message = 'Pending updates monitor started'

    def start(self):
        """ Starts the monitor activity
        """
        rt = True
        self.remove_monitor_data()
        # Clear caché
        flush_cache(namespace='system')
        # Load all system from current_local
        logger.info("Checking for pending updates")
        result, systems = get_systems()
        if not result:
            logger.error("Can't retrieve the system info: %s" % str(systems))
            return False

        pending_updates = False
        for (system_id, system_ip) in systems:
            (success, info) = apimethod_get_update_info(system_id)
            if success:
                try:
                    sys_pending_updates = info['pending_updates']
                    pending_updates = pending_updates or sys_pending_updates
                    logger.info("Pending Updates for system %s (%s): %s" % (system_id, system_ip, sys_pending_updates))
                    monitor_data = {"pending_updates": sys_pending_updates}
                    self.save_data(system_id, ComponentTypes.SYSTEM, self.get_json_message(monitor_data))
                except Exception as e:
                    logger.error("[MonitorPendingUpdates] Error: %s" % str(e))
                    rt = False
                    break
            else:
                logger.error("MonitorPendingUpdates: %s" % info)
                rt = False
                break

        if pending_updates:
            success, local_ip = get_system_ip_from_local()
            if not success:
                logger.error("[MonitorPendingUpdates] Unable to get local IP: %s" % local_ip)
                return False

            success, is_pro = get_is_professional(local_ip)
            if success and is_pro:
                success, is_trial = system_is_trial('local')
                if success and is_trial:
                    logger.info("[MonitorPendingUpdates] Trial version. Skipping download of release info file")
                    return rt

            success, msg = ansible_download_release_info(local_ip)
            if not success:
                logger.error("[MonitorPendingUpdates] Unable to retrieve release info file: %s" % msg)
                return False

        return rt


class MonitorDownloadMessageCenterMessages(Monitor):
    """This monitor will connect to the message center server and it will download all the new messages"""

    def __init__(self):
        Monitor.__init__(self, MonitorTypes.MONITOR_PLATFORM_MESSAGE_CENTER_DATA)
        self.message = 'Pending updates monitor started'

    def start(self):
        """ Starts the monitor activity
        """
        self.remove_monitor_data()
        monitor_data = {}

        success, system_id = get_system_id_from_local()
        if not success:
            return False

        # Load all system from current_local
        logger.info("MCServer downloading messages")
        messages, conn_failed = get_message_center_messages()
        if conn_failed:
            monitor_data['mc_server_connectivity'] = False
            logger.error("Cannot connect to Message Center server")
            self.save_data(system_id,
                           ComponentTypes.SYSTEM,
                           self.get_json_message(monitor_data))
            return True

        # Save a current status message for each message on the list
        success, data = load_external_messages_on_db(messages)
        logger.info("MCServer messages donwloaded.. %s:%s" % (success, str(data)))
        return True


class MonitorSystemCheckDB(Monitor):
    """
        Check database tables. Generate a notifcation message
        if some table use a innodb engine
    """
    def __init__(self):
        """
            Init method
        """
        Monitor.__init__(self, MonitorTypes.MONITOR_SYSTEM_CHECK_DB)
        self.message = 'System check DB started'

    def start(self):
        """
            Start monitor. Connect to database is local
        """
        (success, system_id) = get_system_id_from_local()
        if not success:
            api_log.error("Can't get local system_id")
            return False

        self.remove_monitor_data()

        # OSSIM must not tell to migrate the DB
        rc, pro = system_is_professional(system_id)
        if not pro:
            return True

        (success, result) = check_any_innodb_tables()
        mresult = False
        if success:
            if len(result) > 0:
                #  I need the component ID
                # (success, result) = insert_current_status_message("00000000-0000-0000-0000-000000010017",
                #                                                  system_id, "system", str(result))
                self.save_data(system_id,
                               ComponentTypes.SYSTEM,
                               self.get_json_message({"has_innodb": True,
                                                      "innodb_tables": result}))
                if not success:
                    api_log.error("Can't insert notification into system: %s" % str(result))
                    mresult = False
                else:
                    mresult = True
            else:
                mresult = True  # No messages to insert
        else:
            api_log.error("Can't check current database engine")
            mresult = False
        return mresult


class MonitorWebUIData(Monitor):
    """
    Check several database values affecting the system and/or web integrity
    - information regarding the wizard
    - information to know whether a system has been inserted in the system
    - information regarding max number of assets for a giving license
    - information to check whether a license period has expired
    - information to check if there is contribution to OTX
    """
    __WEB_MESSAGES = {"MESSAGE_WIZARD_SHOWN": "00000000000000000000000000010019",
                      "MESSAGE_SENSOR_NOT_INSERTED": "00000000000000000000000000010020",
                      "MESSAGE_TRIAL_EXPIRED": "00000000000000000000000000010021",
                      "MESSAGE_TRIAL_EXPIRES_7DAYS": "00000000000000000000000000010022",
                      "MESSAGE_TRIAL_EXPIRES_2DAYS": "00000000000000000000000000010023",
                      "MESSAGE_LICENSE_VIOLATION": "00000000000000000000000000010024",
                      "MESSAGE_OTX_CONNECTION": "00000000000000000000000000010025",
                      "MESSAGE_BACKUP_RUNNING": "00000000000000000000000000010026"}

    def __init__(self):
        """
            Init method
        """
        Monitor.__init__(self, MonitorTypes.MONITOR_WEBUI_DATA)
        self.message = 'Web UI data monitor started'

    def start(self):
        """ Starts the monitor activity
        """
        try:
            # Remove the previous monitor data.
            self.remove_monitor_data()
            monitor_data = {}
            success, system_id = get_system_id_from_local()
            if not success:
                return False

            # Now
            now = int(time.time())

            # Firstly, wizard data!
            wizard_dict = {}
            success, start_welcome_wizard, welcome_wizard_date = get_wizard_data()
            if not success:
                api_log.error("There was an error retrieving the wizard data")

            wizard_shown = True
            if start_welcome_wizard == 2:
                # if difference between now and welcome_wizard_date is less
                # than a week, display message
                if (now - welcome_wizard_date) < 420:
                    wizard_shown = False

            wizard_dict['wizard_shown'] = wizard_shown
            monitor_data[self.__WEB_MESSAGES['MESSAGE_WIZARD_SHOWN']] = wizard_dict

            # Time to look for orphan sensors
            orphan_sensors_dict = {}
            success, message = check_any_orphan_sensor()
            orphan_sensors = False
            if not success:
                api_log.error(message)
                orphan_sensors = True

            orphan_sensors_dict['orphan_sensors'] = orphan_sensors
            monitor_data[self.__WEB_MESSAGES['MESSAGE_SENSOR_NOT_INSERTED']] = orphan_sensors_dict

            # Has the trial version expired?
            success, expires, message = get_trial_expiration_date()
            trial_expired = False
            trial_expires_7days = False
            trial_expires_2days = False
            if not success:
                rc, pro = system_is_professional()
                if rc:
                    if pro:
                        # OK, we have an error here
                        api_log.error(message)
                    else:
                        pass
            else:
                # expire=9999-12-31
                expiration_date = expires.split('=')[1]
                if expiration_date:
                    mktime_expression = datetime.datetime.strptime(expiration_date,
                                                                   "%Y-%m-%d").timetuple()
                    expires = int(time.mktime(mktime_expression))

                    one_week_left = now - 604800
                    two_days_left = now - 172800

                    if expires < one_week_left:
                        trial_expires_7days = True
                    elif expires < two_days_left:
                        trial_expires_2days = True
                    elif expires < now:
                        trial_expired = True
                    else:
                        pass
                else:
                    if os.path.isfile("/etc/ossim/ossim.lic"):
                        api_log.warning("Valid license but no web admin user found!")
                    else:
                        api_log.debug("Expiration date can't be determined: License file not found")

            monitor_data[self.__WEB_MESSAGES["MESSAGE_TRIAL_EXPIRED"]] = {'trial_checked': success,
                                                                          'trial_expired': trial_expired}
            monitor_data[self.__WEB_MESSAGES["MESSAGE_TRIAL_EXPIRES_7DAYS"]] = {'trial_checked': success,
                                                                                'trial_expired': trial_expires_7days}
            monitor_data[self.__WEB_MESSAGES["MESSAGE_TRIAL_EXPIRES_2DAYS"]] = {'trial_checked': success,
                                                                                'trial_expired': trial_expires_2days}

            # Check max number of assets
            assets = len(get_asset_list())
            contracted_devices = get_license_devices()
            over_assets = False
            exceeding_assets = 0
            if assets > contracted_devices:
                exceeding_assets = assets - contracted_devices
                over_assets = True
            monitor_data[self.__WEB_MESSAGES["MESSAGE_LICENSE_VIOLATION"]] = {'over_assets': over_assets,
                                                                              'exceeding_assets': exceeding_assets}

            # OTX contribution
            otx_enabled = get_config_otx_enabled()
            monitor_data[self.__WEB_MESSAGES["MESSAGE_OTX_CONNECTION"]] = {'otx_enabled': otx_enabled}

            # Backup in progress?
            success, running, message = check_backup_process_running()
            if not success:
                api_log.error(message)

            monitor_data[self.__WEB_MESSAGES["MESSAGE_BACKUP_RUNNING"]] = {'backup_check': success,
                                                                           'backup_running': running}

            # Save monitor data
            self.save_data(system_id,
                           ComponentTypes.SYSTEM,
                           self.get_json_message(monitor_data))

        except Exception as err:
            api_log.error("Error processing WebUIData monitor information: %s" % str(err))
            return False
        return True


class MonitorSupportTunnel(Monitor):
    """
        Run monitor every hour. If no ssh up and
        file keys stats > 1 hour kill the tunnel.
        must be checked in all systems.
    """
    def __init__(self):
        Monitor.__init__(self, MonitorTypes.MONITOR_SUPPORT_TUNNELS)
        self.message = 'Support tunnels monitor started'

    def start(self):
        """
            Check in all system if there is a tunnel up and keys exits.
            if keys exists and tunnel down, exec all tunnel shutdown process
        """
        success, systems = ret = get_systems()
        if not success:
            logger.error("Can't get systems list")
            return ret
        result = True
        for (system_id, system_ip) in systems:
            logger.info("Checking supports tunnels in system ('%s','%s')" % (system_id, system_ip))
            success, result = check_support_tunnels(system_ip)
            if not result:
                logger.error("Can't check support tunnel in system ('%s','%s')" % (system_id, system_ip))
            else:
                logger.info("Tunnel in ('%s','%s'): %s" % (system_id, system_ip, result))
        return result, ''

class MonitorSystemRebootNeeded(Monitor):
    def __init__(self):
        Monitor.__init__(self, MonitorTypes.MONITOR_SYSTEM_REBOOT_NEEDED)
        self.message = 'System reboot needed monitor started'

    def start(self):
        """
        Starts the monitor activity
        """
        result, systems = get_systems()
        if not result:
            logger.error("Cannot retrieve system info: %s" % str(systems))
            return False
        self.remove_monitor_data()

        for (system_id, system_ip) in systems:
            result, msg = system_reboot_needed(system_ip)
            if result:
                if not self.save_data(system_id, ComponentTypes.SYSTEM, self.get_json_message({'reboot_needed': msg})):
                    logger.error("Cannot save monitor info")
            else:
                logger.error("Cannot retrieve system %s information: %s") % (system_id, msg)

        return True
