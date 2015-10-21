# python
# -*- coding: utf-8 -*-
"""
    Tests for apimethods.system
"""
from __future__ import print_function
import unittest
from mock import patch, call, DEFAULT, mock_open
import uuid

from apimethods.system.system import get_all
from apimethods.system.system import get_all_systems_with_ping_info
from apimethods.system.system import add_child_server
from apimethods.system.system import add_system_from_ip
from apimethods.system.system import add_system
from apimethods.system.system import apimethod_delete_system
from apimethods.system.system import sync_database_from_child


class TestSystemGetAll(unittest.TestCase):
    """
        Unit test for apimethods.system.get_all()
    """
    def setUp(self):
        pass

    def tearDown(self):
        pass

    @patch('apimethods.system.system.get_systems_full')
    def test_0001(self, mock_call):
        """
            Test the fail
        """
        mock_call.return_value = False, "mock value"
        ret = get_all()
        self.assertFalse(ret[0])

    @patch('apimethods.system.system.get_systems_full')
    def test_0002(self, mock_call):
        """
            Test OK
        """
        uuid1 = uuid.uuid1()
        uuid2 = uuid.uuid1()
        mock_call.return_value = (True,
                                  [(str(uuid1), {'admin_ip': '192.168.1.1',
                                    'profile': 'Server,Sensor',
                                    'hostname': 'ascodevida'}),
                                  (str(uuid2), {'admin_ip': '192.168.1.2',
                                  'profile': 'Server',
                                  'hostname': 'menzoberrazan'})
                                  ])
        ret = get_all()
        self.assertTrue(ret[0])
        # Check the results
        res = {str(uuid1):
                 {'hostname': 'ascodevida',
                 'admin_ip': '192.168.1.1',
                 'profile': 'Server,Sensor'},
            str(uuid2):
                 {'hostname': 'menzoberrazan',
                  'admin_ip': '192.168.1.2',
                  'profile': 'Server'}}
        for key, data in ret[1].items():
            self.assertTrue(key in res.keys())
            self.assertTrue(res[key] == data)

    @patch('apimethods.system.system.get_systems_full')
    def test_0003(self, mock_call):
        """
            Missing data
        """
        uuid1 = uuid.uuid1()
        try:
            mock_call.return_value = (True,
                 [(str(uuid1), {'admin_ip': '192.168.1.1',
                'profile': 'Server,Sensor'})])
            _ = get_all()
        except KeyError:
            return
        self.assertTrue(False)


class TestSystemAllSystemWithPingInfo(unittest.TestCase):
    """
        Unit test for apimethods.system.get_all_systems_with_ping_info
    """
    def setUp(self):
        pass

    def tearDown(self):
        pass

    @patch('apimethods.system.system.get_systems_full')
    @patch('apimethods.system.system.ping_system')
    def test_0001(self, mock_ping, mock_system_full):
        """
            Reachable OK
        """
        uuid1 = str(uuid.uuid1())
        mock_system_full.return_value = (True,
            [(uuid1, {'admin_ip': '192.168.1.1',
            'profile': 'Server,Sensor',
            'hostname': 'ascodevida',
            'vpn_ip': ''}),
            ])
        mock_ping.return_value = True, "PING"
        ret = get_all_systems_with_ping_info()
        self.assertTrue(ret[0])
        self.assertTrue(ret[1][uuid1]['reachable'])
        # Check with vpn_ip

    @patch('apimethods.system.system.get_systems_full')
    @patch('apimethods.system.system.ping_system')
    def test_0002(self, mock_ping, mock_system_full):
        """
            Reachable False
        """
        uuid1 = str(uuid.uuid1())
        mock_system_full.return_value = (True,
             [(uuid1, {'admin_ip': '192.168.1.1',
                'profile': 'Server,Sensor',
                'hostname': 'ascodevida',
                'vpn_ip': ''}),
            ])
        mock_ping.return_value = False, "not reachable"
        ret = get_all_systems_with_ping_info()
        self.assertTrue(ret[0])
        self.assertFalse(ret[1][uuid1]['reachable'])

    @patch('apimethods.system.system.get_systems_full')
    @patch('apimethods.system.system.ping_system')
    def test_0003(self, mock_ping, mock_system_full):
        """
            Reachable true with VPN
        """
        uuid1 = str(uuid.uuid1())
        mock_system_full.return_value = (True,
             [(uuid1, {'admin_ip': '192.168.1.1',
            'profile': 'Server,Sensor',
            'hostname': 'ascodevida',
            'vpn_ip': '192.168.1.2'})])
        mock_ping.return_value = True, "PING"
        ret = get_all_systems_with_ping_info()
        self.assertTrue(ret[0])
        self.assertTrue(ret[1][uuid1]['reachable'])
        self.assertTrue(call('192.168.1.2') == mock_ping.call_args_list[0])

    @patch('apimethods.system.system.get_systems_full')
    def test_0004(self, mock_system_full):
        """
            Can't obtain system list
        """
        mock_system_full.return_value = (False, "PUF!")
        ret = get_all_systems_with_ping_info()
        self.assertFalse(ret[0])


class TestAddChildServer(unittest.TestCase):
    """
        Tests for add_child_server
    """
    def setUp(self):
        pass

    def tearDown(self):
        pass

    @patch('apimethods.system.system.db_get_server')
    @patch('apimethods.system.system.ans_add_server')
    @patch('apimethods.system.system.db_add_child_server')
    @patch('apimethods.system.system.ans_add_server_hierarchy')
    def test0001(self, *args):
        """
        Test the add_child_server positive exit
        """
        m_db_get_server = args[3]  # I prefer this to the params.
        m_ans_add_server = args[2]
        m_ans_add_server_hierarchy = args[0]
        # Generete a "local server info"
        uuidlocal = str(uuid.uuid1())
        localinfo = {
            'id': uuidlocal,
            'name': 'localserver',
            'ip': '127.0.0.1',
            'port': '40001',
            'descr': 'Mock server local'
        }
        m_db_get_server.return_value = (True, localinfo)
        m_ans_add_server.return_value = (True, "Mock ans_add_server")
        m_ans_add_server_hierarchy.return_value = (True, "Mock ans_add_server_hierarchy")
        guuid = str(uuid.uuid1())
        res = add_child_server('192.168.1.1', guuid)
        (_, keydict) = m_ans_add_server.call_args
        self.assertTrue(keydict.get('system_ip') == '192.168.1.1')
        self.assertTrue(keydict.get('server_id') == uuidlocal)
        self.assertTrue(keydict.get('server_ip') == '127.0.0.1')
        self.assertTrue(keydict.get('server_port') == '40001')
        self.assertTrue(keydict.get('server_descr') == localinfo['descr'])
        #
        (_, keydict) = m_ans_add_server_hierarchy.call_args
        self.assertTrue(keydict.get('system_ip') == '192.168.1.1')
        self.assertTrue(keydict.get('parent_id') == localinfo['id'])
        self.assertTrue(keydict.get('child_id') == guuid)
        self.assertTrue(res[0])

    @patch('apimethods.system.system.db_get_server')
    @patch('apimethods.system.system.ans_add_server')
    @patch('apimethods.system.system.db_add_child_server')
    @patch('apimethods.system.system.ans_add_server_hierarchy')
    def test0002(self, *args):
        """
        Test the add_child_server positive exit. Fail db_get_server
        """
        m_db_get_server = args[3]  # I prefer this to the params.
        m_ans_add_server = args[2]
        m_ans_add_server_hierarchy = args[0]
        # Generete a "local server info"
        m_db_get_server.return_value = (False, "PETE")
        m_ans_add_server.return_value = (True, "Mock ans_add_server")
        m_ans_add_server_hierarchy.return_value = (True, "Mock ans_add_server_hierarchy")
        guuid = str(uuid.uuid1())
        res = add_child_server('192.168.1.1', guuid)
        self.assertFalse(res[0])
        self.assertTrue(res[1] == 'PETE')

    @patch('apimethods.system.system.db_get_server')
    @patch('apimethods.system.system.ans_add_server')
    @patch('apimethods.system.system.db_add_child_server')
    @patch('apimethods.system.system.ans_add_server_hierarchy')
    def test0003(self, *args):
        """
        Test the add_child_server positive exit. Fail ans_add_server
        """
        m_db_get_server = args[3]  # I prefer this to the params.
        m_ans_add_server = args[2]
        m_ans_add_server_hierarchy = args[0]
        # Generete a "local server info"
        uuidlocal = str(uuid.uuid1())
        localinfo = {
            'id': uuidlocal,
            'name': 'localserver',
            'ip': '127.0.0.1',
            'port': '40001',
            'descr': 'Mock server local'
        }
        m_db_get_server.return_value = (True, localinfo)
        m_ans_add_server.return_value = (False, "Mock ERROR ans_add_server")
        m_ans_add_server_hierarchy.return_value = (True, "Mock ans_add_server_hierarchy")
        guuid = str(uuid.uuid1())
        res = add_child_server('192.168.1.1', guuid)
        self.assertFalse(res[0])
        self.assertTrue(res[1] == 'Mock ERROR ans_add_server')

    @patch('apimethods.system.system.db_get_server')
    @patch('apimethods.system.system.ans_add_server')
    @patch('apimethods.system.system.db_add_child_server')
    @patch('apimethods.system.system.ans_add_server_hierarchy')
    def test0004(self, *args):
        """
        Test the add_child_server positive exit. Fail ans_add_server_hierarchy
        """
        m_db_get_server = args[3]  # I prefer this to the params.
        m_ans_add_server = args[2]  #
        m_ans_add_server_hierarchy = args[0]
        # Generete a "local server info"
        uuidlocal = str(uuid.uuid1())
        guuid = str(uuid.uuid1())
        localinfo = {
            'id': uuidlocal,
            'name': 'localserver',
            'ip': '127.0.0.1',
            'port': '40001',
            'descr': 'Mock server local'
        }
        m_db_get_server.return_value = (True, localinfo)
        m_ans_add_server_hierarchy.return_value = (False, "Mock ERROR ans_add_server_hierarchy")
        m_ans_add_server.return_value = (True, "Mock ans_add_server")
        res = add_child_server('192.168.1.1', guuid)
        self.assertFalse(res[0])
        self.assertTrue(res[1] == 'Mock ERROR ans_add_server_hierarchy')


class TestAddSystemFromIp(unittest.TestCase):
    """
        Tests for add_system_from_ip
    """
    def __init__(self, methodName):
        self.uuidlocal = None
        self.uuidserver_id = None
        self.uuidsystem_id = None
        self.uuidsensor_id = None
        self.system_info = {}
        super(TestAddSystemFromIp, self).__init__(methodName)

    def _init_mocks_remote_sensor(self, mocks):
        """
            Init mocks for a remote sensor
        """
        self.uuidlocal = str(uuid.uuid1())
        self.uuidserver_id = str(uuid.uuid1())
        self.uuidsystem_id = str(uuid.uuid1())
        self.uuidsensor_id = str(uuid.uuid1())
        self.system_info = {
            'profile': ['sensor'],
            'server_id': self.uuidserver_id,
            'system_id': self.uuidsystem_id,
            'hostname': 'ascodevida',
        }
        # mocks
        mocks['get_system_id_from_local'].return_value = (True, self.uuidlocal)
        mocks['ansible_add_system'].return_value = (True, "Mock OK ansible_add_system")
        mocks['ansible_get_system_info'].return_value = (True, self.system_info)
        mocks['get_sensor_id_from_sensor_ip'].return_value = (True, self.uuidsensor_id)
        mocks['db_add_system'].return_value = (True, "Mock OK db_add_system")
        mocks['create_directory_for_ossec_remote'].return_value = (True, "Mock OK create_directory_for_ossec_remote")
        mocks['add_child_server'].return_value = (True, "Mock OK add_child_server")

    def _init_mocks_remote_system(self, mocks):
        """
            Init mocks for a remote system
        """
        self._init_mocks_remote_sensor(mocks)
        # Ah, nice references :)
        self.system_info['profile'] = ['sensor', 'server']

    @patch.multiple('apimethods.system.system',
        api_log=DEFAULT, create_directory_for_ossec_remote=DEFAULT,
        db_add_system=DEFAULT, get_sensor_id_from_sensor_ip=DEFAULT,
        add_child_server=DEFAULT, ansible_get_system_info=DEFAULT,
        ansible_add_system=DEFAULT, get_system_id_from_local=DEFAULT)
    def test0001(self, **mocks):
        """
            test de ok call add_system_from_ip
        """
        self._init_mocks_remote_system(mocks)
        res = add_system_from_ip('192.168.0.1', 'ascodevida', add_to_database=True)
        self.assertTrue(res[0])
        # Check params
        (ordered, keydict) = mocks['ansible_add_system'].call_args
        self.assertTrue(keydict['local_system_id'] == self.uuidlocal)
        self.assertTrue(keydict['remote_system_ip'] == '192.168.0.1')
        self.assertTrue(keydict['password'] == 'ascodevida')
        #
        (ordered, keydict) = mocks['add_child_server'].call_args
        self.assertTrue(ordered[0] == '192.168.0.1')
        self.assertTrue(ordered[1] == self.system_info['server_id'])
        # add_db_system
        (ordered, keydict) = mocks['db_add_system'].call_args
        self.assertTrue(keydict['system_id'] == self.system_info['system_id'])
        self.assertTrue(keydict['name'] == self.system_info['hostname'])
        self.assertTrue(keydict['admin_ip'] == '192.168.0.1')
        self.assertTrue(keydict['profile'] == ",".join(self.system_info['profile']))
        self.assertTrue(keydict['server_id'] == self.system_info['server_id'])
        self.assertTrue(keydict['sensor_id'] is None)

    @patch.multiple('apimethods.system.system',
        api_log=DEFAULT, create_directory_for_ossec_remote=DEFAULT,
        db_add_system=DEFAULT, get_sensor_id_from_sensor_ip=DEFAULT,
        add_child_server=DEFAULT, ansible_get_system_info=DEFAULT,
        ansible_add_system=DEFAULT, get_system_id_from_local=DEFAULT)
    def test0002(self, **mocks):
        """
            test de fail call get_system_id_from_local
        """
        self._init_mocks_remote_system(mocks)
        mocks['get_system_id_from_local'].return_value = (False, "Mock ERROR get_system_id_from_local")
        res = add_system_from_ip('192.168.0.1', 'ascodevida', add_to_database=True)
        self.assertFalse(res[0])

    @patch.multiple('apimethods.system.system',
        api_log=DEFAULT, create_directory_for_ossec_remote=DEFAULT,
        db_add_system=DEFAULT, get_sensor_id_from_sensor_ip=DEFAULT,
        add_child_server=DEFAULT, ansible_get_system_info=DEFAULT,
        ansible_add_system=DEFAULT, get_system_id_from_local=DEFAULT)
    def test0003(self, **mocks):
        """
            Verify a remote sensor
        """
        self._init_mocks_remote_sensor(mocks)
        res = add_system_from_ip('192.168.0.1', 'ascodevida', add_to_database=True)
        self.assertTrue(res[0])
        # Check
        self.assertFalse(mocks['add_child_server'].called)
        self.assertTrue(mocks['get_sensor_id_from_sensor_ip'].called)
        (ordered, _) = mocks['get_sensor_id_from_sensor_ip'].call_args
        self.assertTrue(ordered[0] == '192.168.0.1')

    @patch.multiple('apimethods.system.system',
        api_log=DEFAULT, create_directory_for_ossec_remote=DEFAULT,
        db_add_system=DEFAULT, get_sensor_id_from_sensor_ip=DEFAULT,
        add_child_server=DEFAULT, ansible_get_system_info=DEFAULT,
        ansible_add_system=DEFAULT, get_system_id_from_local=DEFAULT)
    def test004(self, **mocks):
        """
            add a remote sensor. Don't store database
        """
        self._init_mocks_remote_sensor(mocks)
        res = add_system_from_ip('192.168.0.1', 'ascodevida', add_to_database=False)
        self.assertTrue(res[0])
        # Check
        self.assertFalse(mocks['db_add_system'].called)

    @patch.multiple('apimethods.system.system',
        api_log=DEFAULT, create_directory_for_ossec_remote=DEFAULT,
        db_add_system=DEFAULT, get_sensor_id_from_sensor_ip=DEFAULT,
        add_child_server=DEFAULT, ansible_get_system_info=DEFAULT,
        ansible_add_system=DEFAULT, get_system_id_from_local=DEFAULT)
    def test005(self, **mocks):
        """
            add remote sensor. Fails ansible_get_system_info
        """
        self._init_mocks_remote_sensor(mocks)
        mocks['ansible_get_system_info'].return_value = \
                                   (False, "Mock ERROR ansible_get_system_info")
        res = add_system_from_ip('192.168.0.1', 'ascodevida', add_to_database=False)
        self.assertFalse(res[0])
        self.assertTrue(mocks['ansible_get_system_info'].called)

    @patch.multiple('apimethods.system.system',
        api_log=DEFAULT, create_directory_for_ossec_remote=DEFAULT,
        db_add_system=DEFAULT, get_sensor_id_from_sensor_ip=DEFAULT,
        add_child_server=DEFAULT, ansible_get_system_info=DEFAULT,
        ansible_add_system=DEFAULT, get_system_id_from_local=DEFAULT)
    def test0006(self, **mocks):
        """
            add remote sensor. Fails get_sensor_id_from_sensor_ip
        """
        self._init_mocks_remote_sensor(mocks)
        mocks['get_sensor_id_from_sensor_ip'].return_value = (False, None)
        res = add_system_from_ip('192.168.0.1', 'ascodevida', add_to_database=True)
        self.assertTrue(res[0])
        self.assertTrue(mocks['get_sensor_id_from_sensor_ip'].called)
        (_, keydict) = mocks['db_add_system'].call_args
        self.assertTrue(keydict['sensor_id'] is None)

    @patch.multiple('apimethods.system.system',
        api_log=DEFAULT, create_directory_for_ossec_remote=DEFAULT,
        db_add_system=DEFAULT, get_sensor_id_from_sensor_ip=DEFAULT,
        add_child_server=DEFAULT, ansible_get_system_info=DEFAULT,
        ansible_add_system=DEFAULT, get_system_id_from_local=DEFAULT)
    def test0007(self, **mocks):
        """
            add remote system. Fails add_child_server
        """
        self._init_mocks_remote_system(mocks)
        mocks['add_child_server'].return_value = (False, "Mock ERROR add_child_server")
        res = add_system_from_ip('192.168.0.1', 'ascodevida', add_to_database=True)
        self.assertFalse(res[0])
        self.assertTrue(mocks['add_child_server'].called)

    @patch.multiple('apimethods.system.system',
        api_log=DEFAULT, create_directory_for_ossec_remote=DEFAULT,
        db_add_system=DEFAULT, get_sensor_id_from_sensor_ip=DEFAULT,
        add_child_server=DEFAULT, ansible_get_system_info=DEFAULT,
        ansible_add_system=DEFAULT, get_system_id_from_local=DEFAULT)
    def test0008(self, **mocks):
        """
            add remote system. Fails create_directory_for_ossec_remote
        """
        self._init_mocks_remote_system(mocks)
        mocks['create_directory_for_ossec_remote'].return_value = (False, "Mock ERROR create_directory_for_ossec_remote")
        res = add_system_from_ip('192.168.0.1', 'ascodevida', add_to_database=True)
        self.assertFalse(res[0])
        self.assertTrue(mocks['create_directory_for_ossec_remote'].called)


class TestAddSystem(unittest.TestCase):
    """
        Tests for add_system
    """
    def __init__(self, methodName):
        self._system_ip = '192.168.1.1'
        self._system_id = str(uuid.uuid1())
        super(TestAddSystem, self).__init__(methodName)

    def _init_mocks(self, mocks):
        """
            Init the mocks
        """
        mocks['get_system_ip_from_system_id'].return_value = (True, self._system_ip)
        mocks['add_system_from_ip'].return_value = (True, "Mock OK add_system_from_ip")

    @patch.multiple('apimethods.system.system',
                     get_system_ip_from_system_id=DEFAULT,
                     add_system_from_ip=DEFAULT)
    def test0001(self, **mocks):
        """
            Test add_system ok call
        """
        self._init_mocks(mocks)
        res = add_system(self._system_id, "ascodevida")
        self.assertTrue(res[0])
        (ordered, _) = mocks['get_system_ip_from_system_id'].call_args
        self.assertTrue(ordered[0] == self._system_id)
        (ordered, paramdict) = mocks['add_system_from_ip'].call_args
        self.assertTrue(ordered[0] == self._system_ip)
        self.assertTrue(ordered[1] == 'ascodevida')
        self.assertTrue(paramdict['add_to_database'] == False)

    @patch.multiple('apimethods.system.system',
                     get_system_ip_from_system_id=DEFAULT,
                     add_system_from_ip=DEFAULT)
    def test0002(self, **mocks):
        """
            test add_system fails get_system_ip_from_system_id
        """
        self._init_mocks(mocks)
        mocks['get_system_ip_from_system_id'].return_value = (False, "Mock ERROR get_system_ip_from_system_id")
        res = add_system(self._system_id, "ascodevida")
        self.assertFalse(res[0])
        self.assertTrue(mocks['get_system_ip_from_system_id'].called)

    @patch.multiple('apimethods.system.system',
                     get_system_ip_from_system_id=DEFAULT,
                     add_system_from_ip=DEFAULT)
    def test0003(self, **mocks):
        """
            test add_system fails add_system_from_ip
        """
        self._init_mocks(mocks)
        mocks['add_system_from_ip'].return_value = (False, "Mock ERROR add_system_from_ip")
        res = add_system(self._system_id, "ascodevida")
        self.assertFalse(res[0])
        self.assertTrue(mocks['add_system_from_ip'].called)


class TestApimethodDeleteSystem(unittest.TestCase):
    """
        Tests for apimethod_delete_system
    """
    def __init__(self, methodName):
        self._system_ip = "192.168.1.1"
        self._system_id = str(uuid.uuid1())
        self._system_ip_local = "192.168.0.1"
        self._ansibleinventorymanager = None
        super(TestApimethodDeleteSystem, self).__init__(methodName)

    def _init_mocks(self, mocks):
        """
            Init mocks
        """
        mocks['get_system_ip_from_system_id'].return_value = (True, self._system_ip)
        mocks['db_remove_system'].return_value = (True, "Mock OK db_remove_system")
        mocks['get_system_ip_from_local'].return_value = (True, self._system_ip_local)
        mocks['ansible_remove_certificates'].return_value = (True, "Mock OK ansible_remove_certificates")
        self._ansibleinventorymanager = mocks['AnsibleInventoryManager'].return_value
        # mock the method called from this class

    @patch.multiple('apimethods.system.system', get_system_ip_from_system_id=DEFAULT,
        db_remove_system=DEFAULT, get_system_ip_from_local=DEFAULT,
        ansible_remove_certificates=DEFAULT,
        AnsibleInventoryManager=DEFAULT)
    def test0001(self, **mocks):
        """
            Test succes for apimethod_delete_system
        """
        self._init_mocks(mocks)
        res = apimethod_delete_system(self._system_id)
        self.assertTrue(res[0])
        (ordered, _) = mocks['get_system_ip_from_system_id'].call_args
        self.assertTrue(ordered[0] == self._system_id)
        (ordered, _) = mocks['db_remove_system'].call_args
        self.assertTrue(ordered[0] == self._system_id)
        (_, paramdict) = mocks['ansible_remove_certificates'].call_args
        self.assertTrue(paramdict['system_ip'] == self._system_ip_local)
        self.assertTrue(paramdict['system_id_to_remove'] == self._system_id)
        self.assertTrue(mocks['AnsibleInventoryManager'].return_value.delete_host.called)
        self.assertTrue(mocks['AnsibleInventoryManager'].return_value.save_inventory.called)
        (ordered, _) = mocks['AnsibleInventoryManager'].return_value.delete_host.call_args
        self.assertTrue(ordered[0] == self._system_ip)

    @patch.multiple('apimethods.system.system', get_system_ip_from_system_id=DEFAULT,
        db_remove_system=DEFAULT, get_system_ip_from_local=DEFAULT,
        ansible_remove_certificates=DEFAULT,
        AnsibleInventoryManager=DEFAULT)
    def test0002(self, **mocks):
        """
            apimethod_delete_system fails get_system_ip_from_system_id
        """
        self._init_mocks(mocks)
        # Fails
        mocks['get_system_ip_from_system_id'].return_value = (False, "Mock ERROR get_system_ip_from_system_id")
        res = apimethod_delete_system(self._system_id)
        self.assertFalse(res[0])
        self.assertTrue(mocks['get_system_ip_from_system_id'].called)
        self.assertFalse(mocks['db_remove_system'].called)

    @patch.multiple('apimethods.system.system', get_system_ip_from_system_id=DEFAULT,
        db_remove_system=DEFAULT, get_system_ip_from_local=DEFAULT,
        ansible_remove_certificates=DEFAULT,
        AnsibleInventoryManager=DEFAULT)
    def test0003(self, **mocks):
        """
            apimethod_delete_system fails db_remove_system
        """
        self._init_mocks(mocks)
        # Fails mock
        mocks['db_remove_system'].return_value = (False, "Mock ERROR db_remove_system")
        res = apimethod_delete_system(self._system_id)
        self.assertFalse(res[0])
        self.assertTrue(mocks['db_remove_system'].called)
        self.assertFalse(mocks['get_system_ip_from_local'].called)

    @patch.multiple('apimethods.system.system', get_system_ip_from_system_id=DEFAULT,
        db_remove_system=DEFAULT, get_system_ip_from_local=DEFAULT,
        ansible_remove_certificates=DEFAULT,
        AnsibleInventoryManager=DEFAULT)
    def test0004(self, **mocks):
        """
            apimethod_delete_system fails get_system_ip_from_local
        """
        self._init_mocks(mocks)
        # Fails mock
        mocks['get_system_ip_from_local'].return_value = (False, "Mock ERROR get_system_ip_from_local")
        res = apimethod_delete_system(self._system_id)
        self.assertFalse(res[0])
        self.assertTrue(mocks['get_system_ip_from_local'].called)
        self.assertFalse(mocks['ansible_remove_certificates'].called)

    @patch.multiple('apimethods.system.system', get_system_ip_from_system_id=DEFAULT,
        db_remove_system=DEFAULT, get_system_ip_from_local=DEFAULT,
        ansible_remove_certificates=DEFAULT,
        AnsibleInventoryManager=DEFAULT)
    def test0005(self, **mocks):
        """
            apimethod_delete_system fails AnsibleInventoryManager
        """
        self._init_mocks(mocks)
        # I need to raise a exception within the mocka
        mocks['AnsibleInventoryManager'].side_effect = Exception("Mock Exception")
        res = apimethod_delete_system(self._system_id)
        self.assertFalse(res[0])
        self.assertTrue(mocks['ansible_remove_certificates'].called)

    @patch.multiple('apimethods.system.system', get_system_ip_from_system_id=DEFAULT,
        db_remove_system=DEFAULT, get_system_ip_from_local=DEFAULT,
        ansible_remove_certificates=DEFAULT,
        AnsibleInventoryManager=DEFAULT)
    def test0006(self, **mocks):
        """
            apimethod_delete_system fails AnsibleInventoryManager.delete_host
        """
        self._init_mocks(mocks)
        # I need to raise a exception within the mocka
        mocks['AnsibleInventoryManager'].return_value.delete_host.side_effect = Exception("Mock Exception")
        res = apimethod_delete_system(self._system_id)
        self.assertFalse(res[0])
        self.assertTrue(mocks['ansible_remove_certificates'].called)

    @patch.multiple('apimethods.system.system', get_system_ip_from_system_id=DEFAULT,
        db_remove_system=DEFAULT, get_system_ip_from_local=DEFAULT,
        ansible_remove_certificates=DEFAULT,
        AnsibleInventoryManager=DEFAULT)
    def test0007(self, **mocks):
        """
            apimethod_delete_system fails AnsibleInventoryManager.save_inventory
        """
        self._init_mocks(mocks)
        # I need to raise a exception within the mocka
        mocks['AnsibleInventoryManager'].return_value.save_inventory.side_effect = Exception("Mock Exception")
        res = apimethod_delete_system(self._system_id)
        self.assertFalse(res[0])
        self.assertTrue(mocks['ansible_remove_certificates'].called)


class TestSyncDatabaseFromChild(unittest.TestCase):
    """
        Tests for sync_database_from_child
    """
    def __init__(self, methodName):
        self._system_ip = "192.168.1.1"
        self._system_ip_local = "192.168.0.1"
        self._system_id = str(uuid.uuid1())
        super(TestSyncDatabaseFromChild, self).__init__(methodName)

    def _init_mocks(self, mocks):
        """
            Init the mocks
        """
        mocks['get_system_ip_from_system_id'].return_value = (True, self._system_ip)
        mocks['get_system_ip_from_local'].return_value = (True, self._system_ip_local)
        mocks['fetch_if_changed'].return_value = (True, "already in sync")
        mocks['fetch_if_changed'].side_effect = [(True,'Mock OK fetch_if_changed 1'), (True, 'Mock OK fetch_if_changed 2')]
        #mocks['open'].return_value = mocks['open']
        #mocks['open'].return_value.__enter__.return_value = mocks['open']
        #mocks['open'].return_value.readline.side_effect = ['8c6270b7344be88db62d9715a674d7ac']
        #mocks['Popen'].return_value.communicate.return_value = '8c6270b7344be88db62d9715a674d7ac', None
        mocks['call'].return_value = True

    @patch.multiple('apimethods.system.system',
         get_system_ip_from_system_id=DEFAULT,
         get_system_ip_from_local=DEFAULT,
         fetch_if_changed=DEFAULT,
         Popen=DEFAULT,
         call=DEFAULT)
    @patch('__builtin__.open',  mock_open())
    def test0001(self,  **mocks):
        """
            Test sync_database_from_child
        """
        print (mocks)
        self._init_mocks(mocks)
        #res = sync_database_from_child(self._system_id)
        #self.assertTrue(res[0])
    
# vim: ts=4:expandtab:listchars=eol:¶:list
