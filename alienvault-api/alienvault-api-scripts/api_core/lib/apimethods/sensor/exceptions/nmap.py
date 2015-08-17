# -*- coding: utf-8 -*-
#
# License:
#
# Copyright (c) 20154 AlienVault
# All rights reserved.
#
# This package is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 dated June, 1991.
# You may not use, modify or distribute this program under any other version
# of the GNU General Public License.
#
# This package is distributed in the hope that it will be useful,
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


class APIMethodNMAPScanCannotRetrieveScanProgress(Exception):
    def __init__(self, msg=""):
        super(APIMethodNMAPScanCannotRetrieveScanProgress, self).__init__("Cannot retrieve the scan progress")



class APIMethodNMAPScanCannotRun(Exception):
    def __init__(self, msg=""):
        super(APIMethodNMAPScanCannotRun, self).__init__("Cannot run the nmap scan {0}".format(msg))


class APIMethodNMAPScanCannotCreateLocalFolder(Exception):
    def __init__(self, msg=""):
        super(APIMethodNMAPScanCannotCreateLocalFolder, self).__init__("Cannot create the local folder {0}".format(msg))


class APIMethodNMAPScanCannotRetrieveBaseFolder(Exception):
    def __init__(self, base_path=""):
        super(APIMethodNMAPScanCannotRetrieveBaseFolder, self).__init__("Cannot retrieve the base folder {0}".format(base_path))


class APIMethodNMAPScanKeyNotFound(Exception):
    def __init__(self, msg=""):
        super(APIMethodNMAPScanKeyNotFound, self).__init__("Item not found")


class APIMethodNMAPScanException(Exception):
    def __init__(self, msg=""):
        super(APIMethodNMAPScanException, self).__init__(str(msg))


class APIMethodNMAPScanCannotBeSaved(Exception):
    def __init__(self, msg=""):
        super(APIMethodNMAPScanCannotBeSaved, self).__init__("Cannot save the give task data")


class APIMethodNMAPScanReportNotFound(Exception):
    def __init__(self, msg=""):
        super(APIMethodNMAPScanReportNotFound, self).__init__("NMAP Scan Report not found {0}".format(msg))


class APIMethodNMAPScanCannotReadReport(Exception):
    def __init__(self, msg=""):
        super(APIMethodNMAPScanCannotReadReport, self).__init__("Cannot read the scan report {0}".format(msg))


class APIMethodNMAPScanReportCannotBeDeleted(Exception):
    def __init__(self, msg=""):
        super(APIMethodNMAPScanReportCannotBeDeleted, self).__init__("Cannot remove the given report")
