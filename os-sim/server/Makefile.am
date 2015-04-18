sim-parser.c:
	flex -osim-parser.c sim-parser.lex
clean:
	rm -f sim-parser.c *~

bin_PROGRAMS = server

server_SOURCES = sim-parser.c \
		sim-policy.c \
		sim-host.c \
		sim-host-asset.c \
		sim-net.c \
		sim-net-asset.c \
		sim-config.c \
		sim-database.c \
		sim-message.c \
		sim-syslog.c \
		sim-organizer.c \
		sim-server.c \
		main.c

server_LDADD = $(DEPS_LIBS) -lfl

AM_CPPFLAGS = $(DEPS_CFLAGS) \
	      -DPIXMAPS_DIR=\""$(datadir)/pixmaps"\" 
