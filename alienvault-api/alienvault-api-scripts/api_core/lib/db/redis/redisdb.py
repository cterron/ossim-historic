# -*- coding: utf-8 -*-
#
# License:
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
import redis
import ast
from redis_cache import RedisConnect, RedisNoConnException, to_unicode


class RedisDBInvalidKey(Exception):
    def __init__(self, key):
        super(RedisDBInvalidKey, self).__init__("Invalid Key {0}".format(key))


class RedisDBKeyNotFound(Exception):
    def __init__(self, key):
        super(RedisDBKeyNotFound, self).__init__("Redis DB key not found. Key = '{0}'".format(key))


class RedisDBItemCannotBeSaved(Exception):
    def __init__(self, msg):
        super(RedisDBItemCannotBeSaved, self).__init__("Redis DB item cannot be saved. Reason = '{0}'".format(msg))


class RedisDBKeysCannotBeLoaded(Exception):
    def __init__(self, msg):
        super(RedisDBKeysCannotBeLoaded, self).__init__("Redis DB keys cannot be loaded. Reason = '{0}'".format(msg))


class RedisDB(object):
    """NMAP Scans database redis abstraction
    """

    def __init__(self, host="localhost", port=6379, password=None, db=0, namespace=""):
        self.host = host
        self.port = port
        self.password = password
        self.db = db  # Use a different database to the OTX data
        self.namespace = namespace
        try:
            self.connection = RedisConnect(host=self.host,
                                           port=self.port,
                                           db=self.db,
                                           password=password).connect()
        except RedisNoConnException:
            self.connection = None
            pass

    def get_set_name(self):
        """Returns the NMAP Scans Set name
        Ex: AlienVault-NMAP-SCANS-NS-keys
        """
        return "AlienVault-{0}-keys".format(self.namespace)

    def get_namespace_key(self, key):
        """Returns the key with our namespace
        Ex: NMAP-SCANS-NS:55367639b45ff51548a4b58c
        """
        return "{0}:{1}".format(self.namespace, key)

    def store(self, key, value, expire=None):
        """Saves a given pair (key,value)
        Args:
            key(str): DB key
            value(str): DB JSON
        Returns:
            void
        Raises:
            RedisDBItemCannotBeSaved: When a item cannot be saved.
        """
        try:
            key = to_unicode(key)
            value = to_unicode(value)
            set_name = self.get_set_name()

            pipe = self.connection.pipeline()

            # Set the value
            if expire is not None:
                pipe.set(self.get_namespace_key(key), value)
            else:
                pipe.setex(self.get_namespace_key(key), expire, value)
            # add the key to the set
            pipe.sadd(set_name, key)
            # run the commands
            pipe.execute()
        except Exception as err:
            raise RedisDBItemCannotBeSaved(str(err))

    def get(self, key):
        """Returns the NMAP Scan with the given key
        Args:
            key(id): The pulse id
        Returns:
            pulse(dic): A python dic containing the NMAP Scan information
        Raises:
            RedisDBInvalidKey: Wrong formatted key
            RedisDBKeyNotFound: Item not found
        """
        key = to_unicode(key)
        if not key:
            raise RedisDBInvalidKey(key)

        value = self.connection.get(self.get_namespace_key(key))
        if not value:
            raise RedisDBKeyNotFound(key)

        return value

    def keys(self):
        """Returns a list of keys (pulse ids)
        Args:
            void
        Returns:
            [<str>]: A list of strings containing all the keys
        """
        try:
            keys = list(self.connection.smembers(self.get_set_name()))
        except Exception as err:
            raise RedisDBKeysCannotBeLoaded(str(err))
        return keys

    def flush(self):
        """Flushes the current database
        Args:
            void
        Returns:
            (int) The number of members deleted
        """
        keys = self.keys()
        keys.append(self.get_set_name())  # Remove the set name
        if not keys:
            return 0

        with self.connection.pipeline() as pipe:
            pipe.delete(*keys)
            pipe.execute()
        return len(keys)

    def add(self, items):
        """Add new items"""
        # Must be overwrite.
        pass

    def delete_key(self, key):
        """Removes the element with the given key
        Args:
            key(id): The pulse id
        Returns:
            void
        Raises:
            RedisDBInvalidKey: Wrong formatted key
            RedisDBKeyNotFound: Item not found
        """
        key = to_unicode(key)
        if not key:
            raise RedisDBInvalidKey(key)

        with self.connection.pipeline() as pipe:
            pipe.delete(self.get_namespace_key(key))
            pipe.execute()

    def get_all(self):
        key_list = self.keys()
        scans = []
        for key in key_list:
            try:
                value = self.get(key)
                value = ast.literal_eval(value)
                scans.append(value)
            except:
                pass
        return scans

    def update(self, key, value):
        pass