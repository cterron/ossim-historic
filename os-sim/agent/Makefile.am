SUBDIRS=doc pyossim util

ossim_agentdir      = $(datadir)/ossim/agent
ossim_agent_SCRIPTS = ossim-agent
ossim_agent_DATA    = AUTHORS \
	COPYING \
	INSTALL \
	MANIFEST \
	setup.py

EXTRA_DIST   = $(ossim_agent_DATA)

install-data-hook:
	ln -sf $(datadir)/ossim/agent/ossim-agent $(bindir)/ossim-agent

