from unittest import TestCase
import nose
from tempfile import mkdtemp
from os import removedirs
import os.path
from shutil import copyfile,rmtree
import inspect
import ansible.runner
import sys
import random


class test_av_config (TestCase):
    @classmethod
    def setUpClass (cls):
        cls.__tempdir = mkdtemp(prefix="nose.temp",dir="/tmp")
        cls.__confpath = os.path.join ( os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))),"conf")
        
    @classmethod
    def tearDownClass (cls):
        rmtree (cls.__tempdir)

    def get_iface_list (self,conffile,debugfile,inventory):
        module_args = "path=[sensor]interfaces conffile=%s debugfile=%s makebackup=False op=get" % (conffile,debugfile)
        i = ansible.inventory.Inventory (inventory)
        runner = ansible.runner.Runner( module_name='av_config', module_args =  module_args, pattern='*',inventory = i)
        response = runner.run()
        nose.tools.ok_ (response['contacted'].get ('127.0.0.1'),msg = "System doesn't return data") 
        ifaces  = response['contacted']['127.0.0.1']['data']
        return ifaces



 
        
    def test1(self):
        # Correctly load / modify / write cycle
        # Load a conf1
        copyfile (os.path.join (test_av_config.__confpath,"c1.conf"),os.path.join (test_av_config.__tempdir,"c1.conf"))
        pathload = os.path.join (test_av_config.__tempdir, "c1.conf")
        debugfile = os.path.join (test_av_config.__tempdir,"av.log")
        inventory = os.path.join (test_av_config.__confpath,"hosts")
        # Se puedeusar el inventario directo
        module_args = "path=[sensor]interfaces conffile=%s debugfile=%s makebackup=False op=get" % (pathload,debugfile)
        i = ansible.inventory.Inventory (inventory)
        # Call the setup module to obtain the interfaces
        runner = ansible.runner.Runner ( module_name='setup', module_args = "filter=ansible_interfaces", pattern='*', inventory = i)
        responselive = runner.run()
        nose.tools.ok_ (responselive['contacted'].get('127.0.0.1'), msg = "Can't obtain system interfaces")
        # Live interfeaces
        # Loaded file
        ifaces  =  self.get_iface_list (pathload,debugfile,inventory)
        for iface in ifaces:
          nose.tools.ok_ (iface in ['gamusino','silvermoon','waterdeep'],msg = "The ossim_setup.conf don't loaded correctly")
        nose.tools.ok_ (responselive['contacted']['127.0.0.1'].get('ansible_facts'),msg="Can't obtain ansible facts")
        nose.tools.ok_ (responselive['contacted']['127.0.0.1']['ansible_facts'].get('ansible_interfaces'), msg = "Can't get ansible interfaces")
        ifacelive = responselive['contacted']['127.0.0.1']['ansible_facts'].get('ansible_interfaces')
        nose.tools.ok_ (len(ifacelive)>=1, msg="We need at least a interface in the system")
        # I don't want the "lo" interface
        if 'lo' in ifacelive:
          ifacelive.remove ('lo')
        clist = random.sample (ifacelive,random.randint(1,len(ifacelive)))
        c = ",".join(clist)
        module_args = "path=[sensor]interfaces conffile=%s debugfile=%s makebackup=False value=%s op=set" % (pathload,debugfile,c)
        runner = ansible.runner.Runner( module_name='av_config', module_args =  module_args, pattern='*',inventory = i)
        responseset = runner.run()
        # Verify
        nose.tools.ok_ (responseset['contacted'].get ('127.0.0.1'),msg = "System doesn't return data") 
        nose.tools.ok_ (responseset['contacted']['127.0.0.1'].get('data'),msg = "System doesn't return data") 
        nose.tools.ok_ (responseset['contacted']['127.0.0.1']['data'] == 'OK',msg = "System doesn't return data") 
        # Reverify 
        ifacelist = self.get_iface_list (pathload, debugfile, inventory)
        s1 = set(ifacelist)
        s2 = set (clist)
        nose.tools.ok_ (len(s1) == len(s2), msg = "Different interfaces list in ossim_setup.conf bad written")
        nose.tools.ok_ (s1.issubset (s2) == True,  msg = "Different interfaces list in ossim_setup.conf bad written")

    def test2(self):
        copyfile (os.path.join (test_av_config.__confpath,"c2.conf"),os.path.join (test_av_config.__tempdir,"c2.conf"))
        pathload = os.path.join (test_av_config.__tempdir, "c2.conf")
        debugfile = os.path.join (test_av_config.__tempdir,"av.log")
        inventory = os.path.join (test_av_config.__confpath,"hosts")
        module_args = "path=[sensor]interfaces conffile=%s debugfile=%s makebackup=False op=get" % (pathload,debugfile)
        i = ansible.inventory.Inventory (inventory)
        # Call the setup module to obtain the interfaces
        runner = ansible.runner.Runner( module_name='av_config', module_args =  module_args, pattern='*',inventory = i)
        response = runner.run()
        nose.tools.ok_ (response['contacted'].get ('127.0.0.1'),msg = "System doesn't return data") 
        ifaces  = response['contacted']['127.0.0.1']['data']
        nose.tools.ok_ (ifaces == None, "System return a not empty iface list")
    def test3(self):
        """Test 3
          Trying to use an uknon interface"
        """
        copyfile (os.path.join (test_av_config.__confpath,"c1.conf"),os.path.join (test_av_config.__tempdir,"c1.conf"))
        pathload = os.path.join (test_av_config.__tempdir, "c1.conf")
        debugfile = os.path.join (test_av_config.__tempdir,"av.log")
        inventory = os.path.join (test_av_config.__confpath,"hosts")
        i = ansible.inventory.Inventory (inventory)
        module_args = "path=[sensor]interfaces conffile=%s debugfile=%s makebackup=False value=%s op=set" % (pathload,debugfile,'gamusino')
        runner = ansible.runner.Runner( module_name='av_config', module_args =  module_args, pattern='*',inventory = i)
        response = runner.run()
        nose.tools.ok_ (response['contacted'].get ('127.0.0.1'),msg = "System doesn't return data") 
        nose.tools.ok_ (response['contacted']['127.0.0.1'].get('failed'),msg = "System doesn't return data") 











        

        



          
    
        # Obtain the ifaces list via ip
      
