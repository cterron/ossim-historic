ossim_agentdir      = $(datadir)/ossim/agent
ossim_agent_SCRIPTS = ossim-agent
ossim_agent_DATA    = util.py \
	Config.py \
	Agent.py \
	Parser.py \
	Monitor.py \
	ParserArpwatch.py \
	ParserCisco.py \
	ParserCiscoPIX.py \
	ParserCiscoIDS.py \
	ParserSnort.py \
	ParserApache.py \
	ParserFW1.py \
	ParserIIS.py \
	ParserIptables.py \
	ParserRealSecure.py \
	ParserRRD.py \
	ParserCA.py \
	ParserOsiris.py \
	ParserPrelude.py \
	ParserPads.py \
	ParserP0f.py \
	ParserSyslog.py \
	ParserNTsyslog.py \
	MonitorCA.py \
	MonitorNtop.py \
	MonitorNtopHostinfo.py \
	MonitorNtopSession.py \
	MonitorOpennms.py \
	MonitorTcptrack.py \
	MonitorList.py \
	Server.py \
	Scheduler.py \
	Plugin.py \
	Watchdog.py

EXTRA_DIST   = $(ossim_agent_DATA)

install-data-hook:
	ln -sf $(datadir)/ossim/agent/ossim-agent $(bindir)/ossim-agent
