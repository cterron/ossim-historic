# -*- coding: utf-8 -*-
#
# License:
#
# Copyright (c) 2014 AlienVault
# All rights reserved.
#
# This package is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
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
import json
import os
import time
from db.methods.sensor import get_sensor_ip_from_sensor_id
from ansiblemethods.sensor.nmap import ansible_run_nmap_scan, ansible_nmap_get_scan_progress, ansible_nmap_stop, \
    ansible_nmap_purge_scan_files, ansible_get_partial_results
from apimethods.utils import create_local_directory, set_ossec_file_permissions, touch_file
from apimethods.sensor.sensor import get_base_path_from_sensor_id
from db.redis.nmapdb import NMAPScansDB, NMAPScanCannotBeSaved
from db.redis.redisdb import RedisDBKeyNotFound
from apimethods.data.idmconn import IDMConnection
from apimethods.sensor.exceptions.nmap import APIMethodNMAPScanKeyNotFound, APIMethodNMAPScanException, \
    APIMethodNMAPScanCannotBeSaved, APIMethodNMAPScanCannotRetrieveBaseFolder, APIMethodNMAPScanCannotCreateLocalFolder, \
    APIMethodNMAPScanReportNotFound, APIMethodNMAPScanCannotReadReport, APIMethodNMAPScanReportCannotBeDeleted, \
    APIMethodNMAPScanCannotRun, APIMethodNMAPScanCannotRetrieveScanProgress
from apimethods.sensor.exceptions.common import APIMethodCannotResolveSensorID
import ast
import api_log


def get_nmap_directory(sensor_id):
    """Returns the nmap folder for the given sensor ID
    Args:
        sensor_id(str): Canonical Sensor ID
    Returns:
        destination_path: is an string containing the nmap folder when the method works properly or an
                         error string otherwise.
    Raises:
        APIMethodNMAPScanCannotRetrieveBaseFolder
        APIMethodNMAPScanCannotCreateLocalFolder

    """
    success, base_path = get_base_path_from_sensor_id(sensor_id)
    if not success:
        raise APIMethodNMAPScanCannotRetrieveBaseFolder(base_path)
    destination_path = base_path + "/nmap/"

    # Create directory if not exists
    success, msg = create_local_directory(destination_path)
    if not success:
        api_log.error(str(msg))
        raise APIMethodNMAPScanCannotCreateLocalFolder(msg)

    return destination_path


def apimethod_run_nmap_scan(sensor_id, target, idm, scan_type, rdns, scan_timing, autodetect, scan_ports,
                            output_file_prefix="", save_to_file=False, job_id=""):
    """Launches an MAP scan
    Args:
        sensor_ip: The system IP where you want to get the [sensor]/interfaces from ossim_setup.conf
        target: IP address of the component where the NMAP will be executed
        idm: Convert results into idm events
        scan_type: Sets the NMAP scan type
        rdns: Tells Nmap to do reverse DNS resolution on the active IP addresses it finds
        scan_timing: Set the timing template
        autodetect: Aggressive scan options (enable OS detection)
        scan_port: Only scan specified ports
        output_file_prefix: Prefix string to be added to the output filename
        save_to_file: Indicates whether you want to save the NMAP report to a file or not.

    Returns:
        nmap_report = The NMAP report or the filename where the report has been saved.

    Raises:
        APIMethodNMAPScanCannotRun
        APIMethodCannotResolveSensorID
        APIMethodNMAPScanCannotRetrieveBaseFolder
        APIMethodNMAPScanCannotCreateLocalFolder
    """

    (result, sensor_ip) = get_sensor_ip_from_sensor_id(sensor_id, local_loopback=False)
    if result is False:
        api_log.error(
            "[apimethod_run_nmap_scan] Cannot retrieve the sensor ip from the given sensor id <%s>" % sensor_id)
        raise APIMethodCannotResolveSensorID(sensor_id)

    success, nmap_report = ansible_run_nmap_scan(sensor_ip=sensor_ip, target=target, scan_type=scan_type, rdns=rdns,
                                                 scan_timing=scan_timing, autodetect=autodetect, scan_ports=scan_ports,
                                                 job_id=job_id)

    if not success:
        raise APIMethodNMAPScanCannotRun(nmap_report)

    if save_to_file:
        base_path = get_nmap_directory(sensor_id)

        filename = "%s/nmap_report_%s.json" % (base_path, output_file_prefix)
        with open(filename, "w") as f:
            f.write(json.dumps(nmap_report))

    if idm:
        conn = IDMConnection(sensor_id=sensor_id)
        if conn.connect():
            conn.send_events_from_hosts(nmap_report)
        else:
            api_log.error("[apimethod_run_nmap_scan] Cannot connect with the IDM Service")

    return nmap_report


def apimethod_get_nmap_scan(sensor_id, task_id):
    """Retrieves the result of an nmap scan
    Args:

    Raises:
        APIMethodNMAPScanCannotRetrieveBaseFolder
        APIMethodNMAPScanCannotCreateLocalFolder
        APIMethodNMAPScanReportNotFound
        APIMethodNMAPScanCannotReadReport
    """
    directory = get_nmap_directory(sensor_id)
    nmap_report_path = "{0}/nmap_report_{1}.json".format(directory, task_id)
    if not os.path.isfile(nmap_report_path):
        raise APIMethodNMAPScanReportNotFound(nmap_report_path)

    try:
        data = ''
        with open(nmap_report_path, "r") as f:
            data = json.loads(f.read())
    except Exception as e:
        api_log.error("[apimethod_get_nmap_scan] {0}".format(str(e)))
        raise APIMethodNMAPScanCannotReadReport(nmap_report_path)

    return data


def apimethod_delete_nmap_scan(sensor_id, task_id):
    """
    Args:
        sensor_id
        task_id
    Returns:

    Raises:
        APIMethodNMAPScanCannotRetrieveBaseFolder
        APIMethodNMAPScanReportNotFound
        APIMethodNMAPScanCannotCreateLocalFolder
        APIMethodNMAPScanReportCannotBeDeleted
    """
    apimethod_nmapdb_delete_task(task_id)
    directory = get_nmap_directory(sensor_id)
    nmap_report_path = "{0}/nmap_report_{1}.json".format(directory, task_id)
    if not os.path.isfile(nmap_report_path):
        raise APIMethodNMAPScanReportNotFound(nmap_report_path)

    try:
        os.remove(nmap_report_path)
    except Exception as e:
        api_log.error("[apimethod_delete_nmap_scan] {0}".format(str(e)))
        raise APIMethodNMAPScanReportCannotBeDeleted()


def apimethod_monitor_nmap_scan(sensor_id, task_id):
    """Monitors an NMAP scan
    Args:
        sensor_id: The sensor id where the NMAP is working.
        task_id: The celery task id that is launching the NMAP
    Raises
        APIMethodCannotResolveSensorID
        APIMethodNMAPScanCannotRetrieveScanProgress

    """

    (result, sensor_ip) = get_sensor_ip_from_sensor_id(sensor_id, local_loopback=False)
    if result is False:
        api_log.error(
            "[apimethod_monitor_nmap_scan] Cannot retrieve the sensor ip from the given sensor id <%s>" % sensor_id)
        raise APIMethodCannotResolveSensorID(sensor_id)
    try:
        nhosts = ansible_nmap_get_scan_progress(sensor_ip=sensor_ip, task_id=task_id)
    except Exception as e:
        api_log.error("[apimethod_monitor_nmap_scan]  Cannot retrieve scan progress {0}".format(str(e)))
        raise APIMethodNMAPScanCannotRetrieveScanProgress()
    return nhosts


def apimethod_get_nmap_scan_list(user):
    """Monitors all NMAP scan list
    Args:
        user: User login
    Returns:
        scans(dic): A python dic with all jobs.
    Raises:
        Exception: When something wrong happen
    """
    user_scans = []
    db = NMAPScansDB()
    scans = db.get_all()
    del db

    for scan in scans:
        if scan['scan_user'] == user:
            user_scans.append(scan)

    return user_scans


def apimethod_get_nmap_scan_status(task_id):
    """Returns the nmap status for the given task
    Args:
        task_id: The task id which status you want to know
    Returns:
        job(str): A python dic with the job information.
    Raises:
        APIMethodNMAPScanKeyNotFound: When the given id doesn't exist
        APIMethodNMAPScanException: When something wrong happen
    """

    try:
        job = None
        db = NMAPScansDB()
        tries = 3
        while tries > 0:
            try:
                raw_data = db.get(task_id)
                job = ast.literal_eval(raw_data)
                tries = 0
            except RedisDBKeyNotFound:
                #  Maybe the job is not in the database yet
                time.sleep(1)
            tries -= 1
    except Exception as e:
        raise APIMethodNMAPScanException(str(e))
    finally:
        del db
    if job is None:
        raise APIMethodNMAPScanKeyNotFound()
    return job


def apimethods_stop_scan(task_id):
    """Stops the given scan id
    Raises:
        APIMethodCannotResolveSensorID
        APIMethodNMAPScanKeyNotFound
        APIMethodNMAPScanException
    """
    # Stops the celery task.
    job = apimethod_get_nmap_scan_status(task_id)

    job["status"] = "Stopping"
    apimethod_nmapdb_update_task(task_id, job)
    (result, sensor_ip) = get_sensor_ip_from_sensor_id(job["sensor_id"], local_loopback=False)
    if not result:
        raise APIMethodCannotResolveSensorID(job["sensor_id"])

    base_path = get_nmap_directory(job["sensor_id"])
    success, result = ansible_nmap_stop(sensor_ip, task_id)
    if not success:
        raise APIMethodNMAPScanException(str(result))
    job["status"] = "Finished"
    job["reason"] = "Stopped by the user"
    apimethod_nmapdb_update_task(task_id, job)

    success, result_file = ansible_get_partial_results(sensor_ip, task_id)
    if success:
        try:
            results = {}
            if os.path.isfile(result_file):
                with open(result_file, "r") as f:
                    for line in f.readlines():
                        d = json.loads(line)
                        results[d["host"]] = d["scan"]
                filename = "%s/nmap_report_%s.json" % (base_path, task_id)
                with open(filename, "w") as f:
                    f.write(json.dumps(results))
        except Exception as e:
            raise APIMethodNMAPScanException(str(e))

def apimethods_nmap_purge_scan_files(task_id):
    """Purge the given scan files
    Raises:
        APIMethodCannotResolveSensorID
        APIMethodNMAPScanKeyNotFound
        APIMethodNMAPScanException
    """
    # Stops the celery task.
    job = apimethod_get_nmap_scan_status(task_id)

    (result, sensor_ip) = get_sensor_ip_from_sensor_id(job["sensor_id"], local_loopback=False)
    if not result:
        return False, "Cannot retrieve the sensor ip from the given sensor id {0}".format(job["sensor_id"])

    success, result = ansible_nmap_purge_scan_files(sensor_ip, task_id)
    return success, result


def apimethod_nmapdb_add_task(task_id, task_data):
    """Add a new nmap task to the nmapdb
    Raises:
        APIMethodNMAPScanCannotBeSaved
    """
    try:
        db = NMAPScansDB()
        db.add(task_id, task_data)
    except NMAPScanCannotBeSaved:
        raise APIMethodNMAPScanCannotBeSaved()
    finally:
        del db


def apimethod_nmapdb_get_task(task_id):
    """Returns the given task information
    Args:
        task_id: The task id which you want
    Returns:
        job(str): A python dic with the job information.
    Raises:
        APIMethodNMAPScanKeyNotFound: When the given id doesn't exist
        APIMethodNMAPScanException: When something wrong happen"""
    return apimethod_get_nmap_scan_status(task_id)


def apimethod_nmapdb_update_task(task_id, task_data):
    """Update nmap task in the nmapdb
    Raises:
        APIMethodNMAPScanCannotBeSaved
    """
    # When you add a new task if the task already exists, it updates it.
    apimethod_nmapdb_add_task(task_id, task_data)


def apimethod_nmapdb_delete_task(task_id):
    """Deelte
    Raises:
        APIMethodNMAPScanCannotBeSaved
    """
    try:
        db = NMAPScansDB()
        db.delete_key(task_id)
    except Exception:
        raise
    finally:
        del db

