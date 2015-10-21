# This code must run in a ossim machine with user avapi. Then.
# The ansible modules && ossim_setup.conf must be present and must
# run with the avapi user.
""" Test several Ansible Manager functions"""  
from __future__ import print_function
from unittest import TestCase
import nose
from shutil import rmtree
from tempfile import mkdtemp
from pwd import getpwuid
from os import getuid, environ, makedirs
import os.path
import inspect
import StringIO
import re
import sys
import random
import mock
#



sys.path.append ("/usr/share/alienvault-center/av-libs/avconfig")
from avconfig import netinterfaces

from ansiblemethods.helper import read_file
from ansiblemethods.system.network import get_iface_list, \
                                          get_iface_stats, \
                                          get_conf_network_interfaces, \
                                          set_interfaces_roles
 
from ansiblemethods.server.logger import delete_raw_logs
from ansiblemethods.sensor.network import get_sensor_interfaces, \
                                          set_sensor_interfaces

from ansiblemethods.ansiblemanager import Ansible
AMGR = Ansible()


class TestAnsibleMgr (TestCase):
    """ Root test class for several Ansible methods """
    _multiprocess_can_split_ = False 
    # No, I can't split the tests
    # because each test modified de conf of the system

    @classmethod
    def setUpClass(cls): # pylint: disable-msg=C0103
        cls.__tempdir = mkdtemp(prefix="nose.temp", dir="/tmp")
        cls.__confpath = os.path.join(
                         os.path.dirname(
                         os.path.abspath(
                         inspect.getfile(inspect.currentframe()))), "conf")
        cls.__re_section = re.compile(r'^\[.*\]$')
        cls.__re_interfaces = re.compile(r'^interfaces=\s*(?P<ifaces>.*)')


        # The machine must be running with avapi user
        nose.tools.ok_(getpwuid (getuid())[0] == 'avapi',
                       msg="Test must be run with avapi user")
        nose.tools.ok_(environ.get('VIRTUAL_ENV') != None,
                       msg="Test must be run from the avapi virtual enviroment")
    @classmethod
    def tearDownClass (cls): # pylint: disable-msg=C0103
        rmtree (cls.__tempdir)
    def __verify_iface_list (self, ifaces):
        (result, setup) = read_file ("127.0.0.1", "/etc/ossim/ossim_setup.conf")
        nose.tools.ok_ (result == True,
                        msg="Error read_file. Can't read ossim_setup.conf")
        # Search the [sensor]/interface headers 
        cstr = StringIO.StringIO (setup)
        line = cstr.readline()
        ifacesfile = None
        while line:
            #print line
            if line == "[sensor]\n":
                line = cstr.readline()
                while line:
                    nose.tools.ok_ (TestAnsibleMgr.__re_section.match(line) == None,
                        msg="Section start without interfaces key.Bad ossim_setup.conf")
                    match = TestAnsibleMgr.__re_interfaces.match(line)
                    if match:
                        ifacesfile = match.group ('ifaces')
                        #print "ifaces=> " + ifacesfile
                        nose.tools.ok_ ( ifacesfile != None,
                            msg="Can't capture interfaces in [sensor]/ossim_setup.conf")
                        break
                    line = cstr.readline() 
                # 
                nose.tools.ok_ (ifacesfile != None, msg="Can't capture interfaces in [sensor]/ossim_setup.conf")
                break 
            line = cstr.readline ()
        # Here we have the ifaces. We must compare with 
        nose.tools.ok_ (ifacesfile != None, msg ="Can't capture interfaces in [sensor]/ossim_setup.conf")
        ifaceslist = [x.strip() for x in ifacesfile.split (",")] 
        for iface in ifaces:
            nose.tools.ok_ ((iface in ifaceslist) == True, msg ="%s from ansible not in ossim_setup.conf" % iface)
        return True
             
         
    def test_get_sensor_interfaces (self):
        print ("Testing get_sensor_interfaces")
        (result, ifaces) = get_sensor_interfaces ("127.0.0.1")
        #print ifaces
        nose.tools.ok_ (result == True, msg="Error get_sensor_interfaces => " + str(ifaces))
        # Call the ansible module to obtain as root the interfaces fields of sensor
        nose.tools.ok_ (result == True, msg="Error read_file. Can't read ossim_setup.conf")
        nose.tools.ok_ (self.__verify_iface_list (ifaces) == True, msg="Can't verify interface list")
        
    def test_set_sensor_interfaces (self):
        print ("Testing set_sensor_interfaces")
        sys_ifaces = [iface.name for iface in  netinterfaces.get_network_interfaces() if iface.name !='lo']
        #print "Traza 1:" + str(sys_ifaces)
        nose.tools.ok_ (len(sys_ifaces)>0, msg="The system needs at least one network interface disting of lo")
        # Generate a list of ramdom ifaces
        test_ifaces = random.sample (sys_ifaces, random.randint(1, len(sys_ifaces)))
        # Backup de system interfaces
        (result, backup_ifaces) = get_sensor_interfaces ("127.0.0.1")
        nose.tools.ok_ (result == True, msg="Can't get backup of ossim_setup.conf interfaces")
        (result, resp) = set_sensor_interfaces ("127.0.0.1", ",".join(test_ifaces))
        nose.tools.ok_ (result == True, msg="Error in set_sensor_interfaces")
        # Verify
        nose.tools.ok_ (self.__verify_iface_list (test_ifaces) == True, msg="Can't verify interface list")
        # Restore backup 
        (result, resp) = set_sensor_interfaces ("127.0.0.1",
                                                ",".join  (backup_ifaces))
        nose.tools.ok_ (result == True, msg="Can't restore backup interfaces =>" + str(resp) + " Result: " + str(result))
        nose.tools.ok_ (self.__verify_iface_list (backup_ifaces) == True, msg="Can't verify interface list")
    def test_set_sensor_interface_error (self):
        print ("Testing set_sensor_interfacesa with an unknown interface name")
        sys_ifaces = [iface.name for iface in  netinterfaces.get_network_interfaces() if iface.name !='lo']
        #print "Traza 1:" + str(sys_ifaces)
        nose.tools.ok_ (len(sys_ifaces)>0, msg="The system needs at least one network interface disting of lo")
        (result, resp) = set_sensor_interfaces ("127.0.0.1", ["gamusino", "ascodevida"])
        nose.tools.ok_ (result == False, msg="gamusiono must be no a valid iface name")
    def test_get_iface_list(self):
        print ("Testing get_iface_list")
        (result, ifaces_list) = get_iface_list ("127.0.0.1")
        nose.tools.ok_ (result == True, "Error:get_iface_list: Can't get iface list")
        nose.tools.ok_ (type(ifaces_list).__name__ == 'dict', "Error:get_iface_list: Bad type")
        ifaces = [x.strip()  for x in ifaces_list.keys() if x != 'lo']
        sys_ifaces = [iface.name for iface in  netinterfaces.get_network_interfaces() if iface.name !='lo']
        nose.tools.ok_ (len (sys_ifaces) == len (ifaces), msg="Number of interfaces mismatch")
        sys_set = set (sys_ifaces)
        ansible_set = set (ifaces)
        nose.tools.ok_ (ansible_set.issubset(sys_set))
    def test_get_iface_stats (self):
        print ("Testing get_iface_stats")
        # This difficult to test, because the dynamic nature of the counter
        # I'm only test if the function return all the ifaces in the system
        (result, dstats) = get_iface_stats ("127.0.0.1")
        nose.tools.ok_ (result == True, msg="Error:get_iface_list: Can't get iface list")
        nose.tools.ok_ (type(dstats).__name__ == 'dict', msg="Error:get_iface_list: Bad type")
        ifaces = [x.strip()  for x in dstats.keys() if x != 'lo']
        sys_ifaces = [iface.name for iface in  netinterfaces.get_network_interfaces() if iface.name !='lo']
        nose.tools.ok_ (len (sys_ifaces) == len (ifaces), msg="Number of interfaces mismatch")
        sys_set = set(sys_ifaces)
        ansible_set = set(ifaces)
        nose.tools.ok_(ansible_set.issubset(sys_set))
        # verify each result 
        #print d
        for value in dstats.values():
          #print v,type(v).__name__
            nose.tools.ok_ (type(value).__name__ == 'dict', msg="Error:get_iface_stats: Bad type")
            nose.tools.ok_ (len(value) ==2, msg="Error:get_iface_stats: Bad response in key " + str(value))
            nose.tools.ok_ (value.get('TX') != None, msg="Error:get_iface_stats: No TX key in response" + str(value))
            nose.tools.ok_ (value.get('RX') != None, msg="Error:get_iface_stats: No RX key in response" + str(value))
    @mock.patch ('ansiblemethods.ansiblemanager.Ansible.run_module')
    def test_conf_network_interfaces(self, mock_ansible):
        print ("Testing conf_network_interfaces")
        # Test the error response
        mock_ansible.return_value = {'dark':{ '127.0.0.1':{'msg':"test message"}}}
        result, msg = get_conf_network_interfaces("127.0.0.1")
        nose.tools.ok_ (result == False, "Response must be false")
        nose.tools.ok_ (msg == "get_conf_network_interfaces test message", "Bad message response")
        # Test a OK path
        mock_ansible.return_value = {'dark':{}, 'contacted': 
                { '127.0.0.1' :
                    {
                     'result': 
                       [[u'match /files/etc/network/interfaces/iface/*', [{u'value': u'inet', u'label': u'/files/etc/network/interfaces/iface[1]/family'}, {u'value': u'loopback', u'label': u'/files/etc/network/interfaces/iface[1]/method'}, {u'value': u'The primary network interface', u'label': u'/files/etc/network/interfaces/iface[1]/#comment'}, {u'value': u'inet', u'label': u'/files/etc/network/interfaces/iface[2]/family'}, {u'value': u'static', u'label': u'/files/etc/network/interfaces/iface[2]/method'}, {u'value': u'192.168.60.5', u'label': u'/files/etc/network/interfaces/iface[2]/address'}, {u'value': u'255.255.255.0', u'label': u'/files/etc/network/interfaces/iface[2]/netmask'}, {u'value': u'network 192.168.60.0\t# WARNING: Unrecognized option', u'label': u'/files/etc/network/interfaces/iface[2]/#comment[1]'}, {u'value': u'broadcast 192.168.60.255\t# WARNING: Unrecognized option', u'label': u'/files/etc/network/interfaces/iface[2]/#comment[2]'}, {u'value': u'dns-* options are implemented by the resolvconf package, if installed', u'label': u'/files/etc/network/interfaces/iface[2]/#comment[3]'}, {u'value': u'8.8.8.8', u'label': u'/files/etc/network/interfaces/iface[2]/dns-nameservers'}, {u'value': u'alienvault', u'label': u'/files/etc/network/interfaces/iface[2]/dns-search'}, {u'value': u'192.168.60.1', u'label': u'/files/etc/network/interfaces/iface[2]/gateway'}, {u'value': u'inet', u'label': u'/files/etc/network/interfaces/iface[3]/family'}, {u'value': u'static', u'label': u'/files/etc/network/interfaces/iface[3]/method'}, {u'value': u'10.0.0.1', u'label': u'/files/etc/network/interfaces/iface[3]/address'}, {u'value': u'255.255.255.0', u'label': u'/files/etc/network/interfaces/iface[3]/netmask'}, {u'value': u'eth1 eth2', u'label': u'/files/etc/network/interfaces/iface[3]/slaves'}, {u'value': u'inet', u'label': u'/files/etc/network/interfaces/iface[4]/family'}, {u'value': u'static', u'label': u'/files/etc/network/interfaces/iface[4]/method'}, {u'value': u'10.0.0.2', u'label': u'/files/etc/network/interfaces/iface[4]/address'}, {u'value': u'255.255.0.0', u'label': u'/files/etc/network/interfaces/iface[4]/netmask'}]], [u'match /files/etc/network/interfaces/iface', [{u'value': u'lo', u'label': u'/files/etc/network/interfaces/iface[1]'}, {u'value': u'eth0', u'label': u'/files/etc/network/interfaces/iface[2]'}, {u'value': u'bond0', u'label': u'/files/etc/network/interfaces/iface[3]'}, {u'value': u'eth2', u'label': u'/files/etc/network/interfaces/iface[4]'}]]] 
                    }
                }
        }
        # 
        result, msg = get_conf_network_interfaces("127.0.0.1")
        nose.tools.ok_ (result == True, "Response must be false")
        # We have 
        nose.tools.ok_ (len(msg.keys()) == 4, "The response have 4 interface keys")
        set1 = set(['eth0', 'bond0', 'eth2', 'lo'])
        set2 = set (msg.keys())
        nose.tools.ok_ (set1 == set2, "Bad intefaces in response")
        # Verify configurarion
        responses = {
            'lo':{},
            'eth0':{
                'address': '192.168.60.5',
                'netmask': '255.255.255.0',
                'gateway': '192.168.60.1'
             },
            'bond0':{
                'address': '10.0.0.1',
                'netmask': '255.255.255.0'
            },
            'eth2':
            {
                'address': '10.0.0.2',
                'netmask': '255.255.0.0'
            }
        
    
        } 
        for key, value in msg.items():
            nose.tools.ok_ (value==responses[key], "Bad interface %s data returned %s" %  (key, str(responses[key])))


    #@mock.patch ('ansiblemethods.system.network.get_conf_network_interfaces')
    #@mock.patch ('ansiblemanager.Ansible.run_module')
    #def test_set_network_interfaces(self,mock_ansible,mock_conf_network):
    #    print "Testing set_conf_iface"
    #    # Verify thar we must call with all params
    #    (result, msg) = set_conf_iface("127.0.0.1","eth0")
    #    nose.tools.ok_(result == False,"Bad response")
    #    nose.tools.ok_(msg == "Can't configure interface","Bad response from server  '%s'" % msg)
    #    # Verify the ip incorrect
    #    (result, msg) = set_conf_iface("127.0.0.1","eth0",ipaddr="a.b.c.d")
    #    nose.tools.ok_ (result == False,"Response must be false")
    #    nose.tools.ok_ (msg ==  "Can't configure interface","Bad message response")
    #     # Verify the ip incorrect
    #    (result, msg) = set_conf_iface("127.0.0.1","eth0",netmask="a.b.c.d")
    #    nose.tools.ok_ (result == False,"Response must be false")
    #    nose.tools.ok_ (msg ==  "Can't configure interface","Bad message response")
    #    # Verify the ip incorrect
    #    (result, msg) = set_conf_iface("127.0.0.1","eth0",gateway="a.b.c.d")
    #    nose.tools.ok_ (result == False,"Response must be false")
    #    nose.tools.ok_ (msg ==  "Can't configure interface","Bad message response")

    #    mock_conf_network.return_value =(True, {
    #        'eth0':{
    #            'address': '192.168.1.2',
    #            'netmask': '255.255.255.0',
    #            'gateway': '192.168.1.1',
    #            'path': '/files/etc/network/interfaces/iface[1]'
    #        },
    #        'eth1':{
    #            'address': '10.0.0.1',
    #            'netmask': '255.255.255.0',
    #            'path': '/files/etc/network/interfaces/iface[2]'
    #        },
    #        'eth2':{
    #            'address' : '172.12.0.1',
    #            'netmask' : '255.255.0.0',
    #            'path' : '/files/etc/network/interfaces/iface[3]'
    #        }
    #        
    #    })
    #    # 
    #    # Create a new interface
    #    (rc, msg) = set_conf_iface("127.0.0.1","eth3",ipaddr="192.168.2.4",netmask="255.255.255.0")
    #    nose.tools.ok_ (rc == True,"Can't create the new interface")
    #    # See the calls
    #    # Verify that we call to the ansible module has the correct params
    #    nose.tools.ok_ (mock_ansible.call_args[1]['module'] == 'av_augeas',"Bad module called")
    #    nose.tools.ok_ (mock_ansible.call_args[1]['host_list'] == ['127.0.0.1'],"Bad ip list")
    #    response="""commands=\'set /files/etc/network/interfaces/auto[last()+1]/1 eth3 set /files/etc/network/interfaces/iface[last()+1] eth3 set /files/etc/network/interfaces/iface[.=\\\"eth3\\\"]/family inet set /files/etc/network/interfaces/iface[.=\\\"eth3\\\"]/method static set /files/etc/network/interfaces/iface[.=\\\"eth3\\\"]/address 192.168.2.4 set /files/etc/network/interfaces/iface[.=\\\"eth3\\\"]/netmask 255.255.255.0 \' validate_filepath=no"""
    #    nose.tools.ok_ (mock_ansible.call_args[1]['args'] == response, "Bad response")
    #    # Verify the gateway
    #    (rc, msg) = set_conf_iface("127.0.0.1","eth3",ipaddr="192.168.2.4",netmask="255.255.255.0",gateway="192.168.2.1")
    #    nose.tools.ok_ (rc == True,"Can't create the new interface")
    #    response="""commands='set /files/etc/network/interfaces/auto[last()+1]/1 eth3 set /files/etc/network/interfaces/iface[last()+1] eth3 set /files/etc/network/interfaces/iface[.=\\\"eth3\\\"]/family inet set /files/etc/network/interfaces/iface[.=\\\"eth3\\\"]/method static set /files/etc/network/interfaces/iface[.=\\\"eth3\\\"]/address 192.168.2.4 set /files/etc/network/interfaces/iface[.=\\\"eth3\\\"]/netmask 255.255.255.0 set /files/etc/network/interfaces/iface[.=\\\"eth3\\\"]/gateway 192.168.2.1  rm /files/etc/network/interfaces/iface[1]/gateway ' validate_filepath=no"""
    #    nose.tools.ok_ (mock_ansible.call_args[1]['args'] == response, "Bad response")
    #    # Try to change a bad gateway
    #    (rc, msg) = set_conf_iface("127.0.0.1","eth0",ipaddr="192.168.2.4",netmask="255.255.255.0",gateway="192.168.3.1")
    #    nose.tools.ok_ (rc == False,"We can't change the gateway to a bad iface")
    #
    @mock.patch('ansiblemethods.ansiblemanager.Ansible.run_module')
    def test_delete_raw_logs (self, mock_ansible):
        print("Testing  test_delete_raw_logs")
        mock_ansible.return_value = {'dark':{}, 'contacted':{'127.0.0.1':{'msg':'Todo ok', 'failed':False}}}
        (ret, msg) = delete_raw_logs("127.0.0.1", start="2011/01/20", end="2012/01/22", path="/tmp")
        nose.tools.ok_ (ret == True, "Response must be true")
        nose.tools.ok_ (msg['msg'] == 'Todo ok', "Bad message returned")
        nose.tools.ok_ (msg['failed'] == False, "Bad message returned")
        mock_ansible.assert_called_with(host_list=['127.0.0.1'], module="av_logger",
                args="start=2011/01/20 end=2012/01/22 path=/tmp ")
        # Test paraams
        (ret, msg) = delete_raw_logs("127.0.0.1", start="2011/01/20")
        nose.tools.ok_ (ret == True, "Response must be true")
        nose.tools.ok_ (msg['msg'] == 'Todo ok', "Bad message returned")
        nose.tools.ok_ (msg['failed'] == False, "Bad message returned")
        mock_ansible.assert_called_with(host_list=['127.0.0.1'], module="av_logger",
                args="start=2011/01/20 path=/var/ossim/logs ")
        # Test the error:
        mock_ansible.return_value = {'dark':{'127.0.0.1':{'msg':"Fallo"}}}
        (ret, msg) = delete_raw_logs("127.0.0.1", start="2011/01/20")
        nose.tools.ok_ (ret == False, "Response must be false")
        nose.tools.ok_ (msg == 'Fallo', "Bad response")
        # Test the error with failed 
        mock_ansible.return_value = {'dark':{},
            'contacted':{'127.0.0.1':{'failed':True, 'msg':'Fallo 2'}}}
        (ret, msg) = delete_raw_logs("127.0.0.1", start="2011/01/20")
        nose.tools.ok_ (ret == False, "Response must be false")
        nose.tools.ok_ (msg == 'Fallo 2', "Bad response")
    def test_delete_raw_logs_all(self):
        dtemp = mkdtemp(dir="/tmp/", prefix="nose.logger.temp")
        # Create a logger dir struct
        makedirs(os.path.join(dtemp, "2014", "04", "03", "10", "192.168.60.1"))
        makedirs(os.path.join(dtemp, "2014", "04", "03", "10", "192.168.60.2"))
        makedirs(os.path.join(dtemp, "2014", "04", "01", "11", "192.168.60.2"))
        # This test involves calling ansible directly on the local machine
        response = AMGR.run_module(host_list = ['127.0.0.1'], module = 'av_logger', args = "path="+dtemp+" deleteall=True", use_sudo=True)
        deleted = response['contacted']['127.0.0.1']['dirsdeleted']
        nose.tools.ok_ (len(deleted) == 4)
        #Open dir and verify that we don't have any file left
        for dirw in os.walk(dtemp):
            if dtemp != dirw[0]:
                nose.tools.ok_(False) 
    
        

            

        
        rmtree (dtemp)
    
    # We use the next code in the test
    _side_data = [
    {'dark':{'127.0.0.1':{'msg':'Error 1'}}},
    {'dark':{}, 'contacted':{'127.0.0.1':{'msg':'Todo casco', 'Failed':True}}},
    {
            'contacted':
            {
                '127.0.0.1': 
                {
                    'data':'eth0'
                }
            },
            'dark':{}
     },
     {'dark':{'127.0.0.1':{'msg':'Error 2'}}},
     {'dark':{}, 'contacted':{'127.0.0.1':{'msg':'Todo casco 2', 'Failed':True}}},
    # Here we test a  bad admin iface
     {
            'contacted':
            {
                '127.0.0.1': 
                {
                    'data':'eth10'
                }
            },
            'dark':{}
     },
     {
        'dark': {},
        'contacted':
             {'127.0.0.1': {'invocation': {'module_name': 'av_setup', 'module_args': 'filter=ansible_interfaces'}, u'verbose_override': True, u'changed': False, u'ansible_facts': {u'ansible_interfaces': [u'lo', u'eth2', u'eth1', u'eth0']}}
        }
    },
    # No exits iface
    {
            'contacted':
            {
                '127.0.0.1': 
                {
                    'data':'eth0'
                }
            },
            'dark':{}
     },
     {
        'dark': {},
        'contacted':
             {'127.0.0.1': {'invocation': {'module_name': 'av_setup', 'module_args': 'filter=ansible_interfaces'}, u'verbose_override': True, u'changed': False, u'ansible_facts': {u'ansible_interfaces': [u'lo', u'eth2', u'eth1', u'eth0']}}
        }
    },
    # Check till get_sensor_interfaces
 
   {
            'contacted':
            {
                '127.0.0.1': 
                {
                    'data':'eth0'
                }
            },
            'dark':{}
     },
     {
        'dark': {},
        'contacted':
             {'127.0.0.1': {'invocation': {'module_name': 'av_setup', 'module_args': 'filter=ansible_interfaces'}, u'verbose_override': True, u'changed': False, u'ansible_facts': {u'ansible_interfaces': [u'lo', u'eth2', u'eth1', u'eth0']}}
        }
    }, 
   
    
    



    ] 
    def _side_set_interface_roles_run_module(*args, **kargs):
        print (TestAnsibleMgr._side_data[0])
        return TestAnsibleMgr._side_data.pop(0)     
    @mock.patch('ansiblemethods.ansiblemanager.Ansible.run_module', side_effect=_side_set_interface_roles_run_module)
    @mock.patch('ansiblemethods.sensor.network.get_sensor_interfaces')
    def test_set_interfaces_roles(self, mock_get_sensor_interfaces, mock_ansible):
        #Init data
        # First test, verify a error
        (ret, result) = set_interfaces_roles('127.0.0.1', {})
        nose.tools.ok_(ret == False)
        # Check the failed attribute
        (ret, result) = set_interfaces_roles('127.0.0.1', {})
        nose.tools.ok_(ret == False)
        # Iface is admin
        (ret, msg) = set_interfaces_roles('127.0.0.1', {'eth0':{'role':'monitoring'}})  
        nose.tools.ok_(ret == False, msg == "The iface 'eth0' is the admin interface. You can't set the role")
        # Now we have several calls to run_module
        # I need to return a list of methods
        #(rc, result) = set_interfaces_roles ('127.0.0.1',{'eth1':{'role':'monitoring'}})  
        #nose.tools.ok_ (rc == False and result['msg'] == "Error 2")
        #(rc, result) = set_interfaces_roles ('127.0.0.1',{'eth1':{'role':'monitoring'}})  
        #nose.tools.ok_ (rc == False and result['msg'] == "Todo casco 2")
        ## Now a mala leche check.
        #(rc, result) = set_interfaces_roles ('127.0.0.1',{'eth1':{'role':'monitoring'}}) 
        #nose.tools.ok_ (rc == False and result == "Internal error admin iface 'eth10' not in system interfaces '[u'lo', u'eth2', u'eth1', u'eth0']'")
        ## Now check a non-exist inface
        #(rc, result) = set_interfaces_roles ('127.0.0.1',{'eth10':{'role':'monitoring'}}) 
        #print(result)
        #nose.tools.ok_ (rc == False and result == "There are interfaces in the call not present in the system")
        ##
        #mock_get_sensor_interfaces.return_value = (False, "Error")
        #(rc, result) = set_interfaces_roles ('127.0.0.1',{'eth1':{'role':'monitoring'}}) 
        #nose.tools.ok_ (rc == False and result == "Can't get current sensor interfaces")
        ## I need to mock here some functions
        ## That must be tested in its own test


    def test_iface_down(self):
        """ Test iface down"""
        pass
    def test_iface_up (self):
        """ Test iface up"""
        pass
    def test_iface_debian_down(self):
        """ Test debian ifdown """
        pass

        
        

        
 
        
        
       

        


