clean:
	rm -f *~

INCLUDES = -DG_LOG_DOMAIN=\"Ossim\"

bin_PROGRAMS = server

server_SOURCES = \
		sim-config.c \
		sim-database.c \
		sim-signature.c \
		sim-policy.c \
		sim-host.c \
		sim-host-asset.c \
		sim-net.c \
		sim-net-asset.c \
		sim-message.c \
		sim-scheduler.c \
		sim-syslog.c \
		sim-organizer.c \
		sim-server.c \
		main.c

server_LDADD = $(DEPS_LIBS)

AM_CPPFLAGS = $(DEPS_CFLAGS)
