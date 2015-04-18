agentdir      = $(datadir)/ossim/agent
agent_SCRIPTS = agent
agent_DATA    = util.py \
	Config.py \
	Agent.py \
	Parser.py \
	Monitor.py \
	ParserCisco.py \
	ParserSnort.py \
	ParserApache.py \
	ParserFW1.py \
	ParserIIS.py \
	ParserIptables.py \
	ParserRealSecure.py \
	ParserRRD.py \
	ParserCA.py \
	MonitorCA.py \
	MonitorNtop.py \
	MonitorNtopHostinfo.py \
	MonitorNtopSession.py

EXTRA_DIST   = $(agent_DATA)

install-data-hook:
	ln -sf $(datadir)/ossim/agent/agent $(bindir)/agent
