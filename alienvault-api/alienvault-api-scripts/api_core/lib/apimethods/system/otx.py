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

import api_log
import urllib2
from json import loads

from apimethods.system.proxy import AVProxy
from db.methods.system import  get_system_ip_from_system_id, get_config_otx_enabled


def apimethod_get_otx_username(token):
    """
    Get all the information available about registered systems.
    """

    otx_api_call = "https://www.alienvault.com/apps/API/V1/get_username/%s" % str(token)
    proxy = AVProxy()
    if proxy is None:
        return False, "ERROR_CONNECTION"

    try:
        request = urllib2.Request(otx_api_call)
        response = proxy.open(request, timeout=30, retries=1)
        otx_response = loads(response.read())
    except Exception, err:
        api_log.error("[Apimethod apimethod_get_otx_username] ERROR_CONNECTION: %s" % str(err))
        return False, "ERROR_CONNECTION"

    if response is None or response.getcode() != 200:
        return False, "ERROR_CONNECTION"

    if not otx_response.get('username'):
        return False, "ERROR_NOT_REGISTERED_TOKEN"

    return True, otx_response

def is_otx_enabled():
    """Retrieves whether a system has OTX enabled or not
    Args:
        system_id (str) : The system_id of the system which you want to get the information
    Returns:
        success (bool)     : True if successful, False elsewhere
        otx_enabled(bool)  : True if OTX is enabled, otherwise False
    """
    return get_config_otx_enabled()

