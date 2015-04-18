INCLUDES = -DG_LOG_DOMAIN=\"Os-sim\" \
	-DOS_SIM_GLOBAL_CONFIG_FILE=\""$(sysconfdir)/os-sim/config.xml"\" \
	-DOS_SIM_GLOBAL_DIRECTIVE_FILE=\""$(datadir)/os-sim/directives.xml"\"

bin_PROGRAMS = os-sim

os_sim_SOURCES = \
		sim-util.c \
		sim-config.c \
		sim-database.c \
		sim-alert.c \
		sim-category.c \
		sim-classification.c \
		sim-plugin.c \
		sim-plugin-sid.c \
		sim-sensor.c \
		sim-host.c \
		sim-net.c \
		sim-policy.c \
		sim-action.c \
		sim-rule.c \
		sim-directive.c \
		sim-host-level.c \
		sim-net-level.c \
		sim-container.c \
		sim-xml-config.c \
		sim-xml-directive.c \
		sim-command.c \
		sim-session.c \
		sim-scheduler.c \
		sim-organizer.c \
		sim-server.c \
		main.c

os_sim_LDADD = $(DEPS_LIBS)

AM_CPPFLAGS = $(DEPS_CFLAGS)
