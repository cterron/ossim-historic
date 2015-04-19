SUBDIRS = src \
	agent \
	etc \
	db \
	pixmaps \
	include \
	mrtg \
	www \
	scripts \
	contrib

DISTCLEANFILES = install-sh mkinstalldirs missing config.guess config.sub aclocal.* config.h* configure

install-data-hook:
	mkdir -p $(localstatedir)/lib/ossim/backup
	mkdir -p $(localstatedir)/log/ossim
