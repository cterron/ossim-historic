SUBDIRS= backup \
	conf \
	control_panel \
	directives \
	editor \
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
	sec_report \
	sensor \
	session \
	setup \
	signature \
	style \
	vulnmeter

wwwdir = $(localstatedir)/www/ossim
www_DATA = index.html \
	top.php

EXTRA_DIST   = $(www_DATA)

install-data-hook:
	mkdir -p $(localstatedir)/www/ossim/mrtg/global_qualification
	mkdir -p $(localstatedir)/www/ossim/mrtg/host_qualification
	mkdir -p $(localstatedir)/www/ossim/mrtg/net_qualification
	mkdir -p $(localstatedir)/www/ossim/mrtg/level_qualification
	mkdir -p $(localstatedir)/www/ossim/mrtg-html/global_qualification
	mkdir -p $(localstatedir)/www/ossim/mrtg-html/host_qualification
	mkdir -p $(localstatedir)/www/ossim/mrtg-html/net_qualification
	mkdir -p $(localstatedir)/www/ossim/mrtg-html/level_qualification
	mkdir -p $(localstatedir)/www/ossim/tmp

