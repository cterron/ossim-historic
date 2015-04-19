SUBDIRS= backup \
	conf \
	control_panel \
	directives \
	editor \
    hids \
	host \
	incidents \
    java \
	net \
	ntop \
	pixmaps \
	policy \
	port \
	report \
	riskmeter \
	rrd_conf \
	scan \
	sensor \
	session \
	setup \
	signature \
	style \
	vulnmeter \
	netscan
	
wwwdir = $(datadir)/ossim/www
www_DATA = index.php \
	top.php

EXTRA_DIST   = $(www_DATA)

install-data-hook:
	mkdir -p $(localstatedir)/lib/ossim/rrd/host_qualification
	mkdir -p $(localstatedir)/lib/ossim/rrd/net_qualification
	mkdir -p $(localstatedir)/lib/ossim/rrd/global_qualification
	mkdir -p $(localstatedir)/lib/ossim/rrd/level_qualification
	   

