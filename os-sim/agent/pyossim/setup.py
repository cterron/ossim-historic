from distutils.core import setup

docs = [ ('/usr/share/man/man8', ['docs/ossim-agent.8']) ]
data = docs

from pyossim.__init__ import VERSION

setup(
	name		= "ossim-agent",
      	version		= VERSION,
      	description	= "OSSIM agent",
      	author		= "David Gil",
      	author_email	= "dgil@ossim.net",
      	url		= "http://www.ossim.net",
      	packages	= [ 'pyossim' ],
      	scripts		= [ 'ossim-agent' ],
      	data_files	= data
      )

