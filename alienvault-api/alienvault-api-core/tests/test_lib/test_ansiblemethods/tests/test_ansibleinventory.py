from nose import with_setup
from nose.tools import raises
import unittest
import sys
import os
import random
import string
import difflib
from shutil import copyfile
lib_path = os.path.dirname(os.path.abspath(os.path.join(__file__, os.pardir)))
api_path = os.path.abspath(os.path.join(lib_path,os.pardir))
parent_api = os.path.abspath(os.path.join(api_path,os.pardir))
sys.path.insert(0,parent_api)
from ansiblemethods.ansibleinventory import *

TEST_FILES_PATH = os.path.abspath(os.path.join(__file__, os.pardir))+"/data/"

#hosts1: Invalid file -- 
#hosts2: Ungrouped hosts
#hosts3: grouped hosts

class TestAnsibleInventory(unittest.TestCase):

    def get_random_filename(self,size=5):
        tmp_filename = "/tmp/" + ''.join(random.choice(string.ascii_uppercase + string.digits) for x in range(size))
        return tmp_filename
    
    def setUp(self):
        print ("TestAnsibleInventory:setup() before each test method")

    def tearDown(self):
        print ("TestAnsibleInventory:teardown() after each test method")

    @classmethod
    def setUpClass(cls):
        print ("TestAnsibleInventory::setup_class() before any methods in this class")

    @classmethod
    def tearDownClass(cls):
        print ("TestAnsibleInventory::tearDownClass() after any methods in this class")
        
        
    @raises(AnsibleInventoryManagerFileNotFound)
    def test_load_nonexisting_file(self):
        """Test load of an invalid file"""
        print ('TestAnsibleInventory:: test_load_nonexisting_file()  <============================ actual test code')
        tmp_filename = self.get_random_filename()
        ansiblefile = AnsibleInventoryManager(inventory_file=tmp_filename) 

    def test_load_invalid_file(self):
        """Test load of an invalid file"""
        print ('TestAnsibleInventory:: test_load_invalid_file()  <============================ actual test code')
        ansiblefile = AnsibleInventoryManager(inventory_file=TEST_FILES_PATH+"hosts1")
        host_list = ansiblefile.get_hosts()
        self.assertEqual(len(host_list),1)
        self.assertEqual(host_list[0],"<novalidhostinventoryfile>")
        del ansiblefile

    def test_get_hosts(self):
        print ('TestAnsibleInventory:: test_get_hosts()  <============================ actual test code')
        ansiblefile = AnsibleInventoryManager(inventory_file=TEST_FILES_PATH+"hosts2")
        expected_host_list = ["ungroupedhost1","ungroupedhost2","ungroupedhost3","ungroupedhost4","ungroupedhost5","ungroupedhost6",]
        host_list = ansiblefile.get_hosts()
        self.assertEqual(len(host_list),len(expected_host_list))
        self.assertEqual(sorted(expected_host_list), sorted(host_list))
        del ansiblefile

    def test_get_hosts_2(self):
        print ('TestAnsibleInventory:: test_get_hosts_2()  <============================ actual test code')
        ansiblefile = AnsibleInventoryManager(inventory_file=TEST_FILES_PATH+"hosts3")
        expected_host_list = ["host1","host2","host3","host4","host5","host6",]
        host_list = ansiblefile.get_hosts()
        self.assertEqual(len(host_list),len(expected_host_list))
        self.assertEqual(sorted(expected_host_list), sorted(host_list))
        del ansiblefile

    def test_get_groups(self):
        print ('TestAnsibleInventory:: test_get_groups()  <============================ actual test code')
        ansiblefile = AnsibleInventoryManager(inventory_file=TEST_FILES_PATH+"hosts3")
        expected_group_list = ["group1","group2", "ungrouped","all"]
        group_list = [group.name for group in ansiblefile.get_groups()]
        self.assertEqual(len(expected_group_list),len(group_list))
        self.assertEqual(sorted(expected_group_list), sorted(group_list))
        del ansiblefile

    def test_get_groups_2(self):
        print ('TestAnsibleInventory:: test_get_groups_2()  <============================ actual test code')
        ansiblefile = AnsibleInventoryManager(inventory_file=TEST_FILES_PATH+"hosts2")
        expected_group_list = ["ungrouped","all"]
        group_list = [group.name for group in ansiblefile.get_groups()]
        self.assertEqual(len(expected_group_list),len(group_list))
        self.assertEqual(sorted(expected_group_list), sorted(group_list))
        del ansiblefile

    def test_get_groups_for_host(self):
        print ('TestAnsibleInventory:: test_get_groups_for_host()  <============================ actual test code')
        
        ansiblefile = AnsibleInventoryManager(inventory_file=TEST_FILES_PATH+"hosts3")
        expected_grous_for_host2 = ["ungrouped","all","group1", "group2"]
        group_list = [group.name for group in ansiblefile.get_groups_for_host("host2")]
        self.assertEqual(len(expected_grous_for_host2),len(group_list))
        self.assertEqual(sorted(expected_grous_for_host2), sorted(group_list))
        
        expected_grous_for_host2 = ["ungrouped","all"]
        group_list = [group.name for group in ansiblefile.get_groups_for_host("host3")]
        self.assertEqual(len(expected_grous_for_host2),len(group_list))
        self.assertEqual(sorted(expected_grous_for_host2), sorted(group_list))
        del ansiblefile

    def test_delete_host(self):
        print ('TestAnsibleInventory:: test_delete_host()  <============================ actual test code')
        ansiblefile = AnsibleInventoryManager(inventory_file=TEST_FILES_PATH+"hosts3")
        expected_list =[("host%s" % i) for i in xrange(1,7)]
        given_list = ansiblefile.get_hosts()
        self.assertEqual(len(expected_list),len(given_list))
        self.assertEqual(sorted(expected_list), sorted(given_list))
        
        ansiblefile.delete_host("host1")
        expected_list =[("host%s" % i) for i in xrange(2,7)]
        given_list = ansiblefile.get_hosts()
        print given_list
        print expected_list
        self.assertEqual(len(expected_list),len(given_list))
        self.assertEqual(sorted(expected_list), sorted(given_list))
        self.assertEqual(ansiblefile.is_dirty(),True)
        del ansiblefile

    def test_delete_host_from_group(self):
        print ('TestAnsibleInventory:: test_delete_host()  <============================ actual test code')
        ansiblefile = AnsibleInventoryManager(inventory_file=TEST_FILES_PATH+"hosts3")
        expected_list =["host1","host2","host4"]
        group = ansiblefile.get_group("group1")
        given_list =  [host.name for host in group.get_hosts()]
        
        self.assertEqual(len(expected_list),len(given_list))
        self.assertEqual(sorted(expected_list), sorted(given_list))
        
        ansiblefile.delete_host("host1",group="group1")
        expected_list =["host2","host4"]
        
        group = ansiblefile.get_group("group1")
        given_list =  [host.name for host in group.get_hosts()]        
        self.assertEqual(len(expected_list),len(given_list))
        self.assertEqual(sorted(expected_list), sorted(given_list))
        
        del ansiblefile
   
    def test_get_group(self):
        print ('TestAnsibleInventory:: test_get_group()  <============================ actual test code')
        ansiblefile = AnsibleInventoryManager(inventory_file=TEST_FILES_PATH+"hosts3")
        group = ansiblefile.get_group("group1")
        self.assertNotEqual(group, None)

        group = ansiblefile.get_group("groupXXX")
        self.assertEqual(group, None)
        
        del ansiblefile

              
    def test_add_host(self):
        print ('TestAnsibleInventory:: test_add_host()  <============================ actual test code')
        ansiblefile = AnsibleInventoryManager(inventory_file=TEST_FILES_PATH+"hosts3")
        expected_list =[("host%s" % i) for i in xrange(1,7)]
        given_list = ansiblefile.get_hosts()
        self.assertEqual(len(expected_list),len(given_list))
        self.assertEqual(sorted(expected_list), sorted(given_list))
        
        ansiblefile.add_host("host7")
        expected_list =[("host%s" % i) for i in xrange(1,8)]
        given_list = ansiblefile.get_hosts()
        self.assertEqual(len(expected_list),len(given_list))
        self.assertEqual(sorted(expected_list), sorted(given_list))
        self.assertEqual(ansiblefile.is_dirty(),True)


        ansiblefile.add_host(host_ip="host8",group_list=["group1"])
        expected_list =["host1","host2","host4","host8"]

        group = ansiblefile.get_group("group1")
        given_list =  [host.name for host in group.get_hosts()]        

        self.assertEqual(len(expected_list),len(given_list))
        self.assertEqual(sorted(expected_list), sorted(given_list))
        self.assertEqual(ansiblefile.is_dirty(),True)
        
        del ansiblefile
     
    def test_save_inventory(self):
         ansiblefile = AnsibleInventoryManager(inventory_file=TEST_FILES_PATH+"hosts3")
         tmp_filename = self.get_random_filename()
         ansiblefile.save_inventory(backup_file=tmp_filename)
         bk_ansible_file = AnsibleInventoryManager(inventory_file=tmp_filename)
        
         self.assertEqual(ansiblefile.get_hosts(),bk_ansible_file.get_hosts())
         self.assertEqual([group.name for group in ansiblefile.get_groups()],[group.name for group in bk_ansible_file.get_groups()])
         ansiblefile.add_host("host7")
         os.remove(tmp_filename)
         del bk_ansible_file
         
         ansiblefile.save_inventory(backup_file=tmp_filename)
         bk_ansible_file = AnsibleInventoryManager(inventory_file=tmp_filename)
         self.assertNotEqual(ansiblefile.get_hosts(),bk_ansible_file.get_hosts())
         self.assertEqual([group.name for group in ansiblefile.get_groups()],[group.name for group in bk_ansible_file.get_groups()])
         #restore the file
         #copyfile(tmp_filename, TEST_FILES_PATH+"hosts3")
         os.remove(tmp_filename)
         del ansiblefile
