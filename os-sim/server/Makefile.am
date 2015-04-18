clean:
	rm -f *~

INCLUDES = -DG_LOG_DOMAIN=\"Ossim\"

bin_PROGRAMS = server

server_SOURCES = \
		sim-util.c \
		sim-database.c \
		sim-host.c \
		sim-net.c \
		sim-signature.c \
		sim-message.c \
		sim-policy.c \
		sim-action.c \
		sim-rule.c \
		sim-alert.c \
		sim-directive.c \
		sim-host-level.c \
		sim-net-level.c \
		sim-container.c \
		sim-xml-directive.c \
		sim-config.c \
		sim-command.c \
		sim-session.c \
		sim-syslog.c \
		sim-scheduler.c \
		sim-organizer.c \
		sim-server.c \
		main.c

server_LDADD = $(DEPS_LIBS)

AM_CPPFLAGS = $(DEPS_CFLAGS)
