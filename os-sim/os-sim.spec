Summary:   Open Source Security Information Management (OSSIM)
Name:      os-sim
Version:   0.9.7rc2
Release:   1
License:   BSD
Group:     Applications/Security
URL:       http://www.ossim.net
Requires:  glib2 > 2.0
Requires:  libgda >= 1.0
Requires:  gnet2 >= 2.0
Source0:   %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot

BuildRequires: glib2-devel > 2.0 libgda-devel >= 1.0 gnet2-devel >= 2.0

%description
OSSIM aims to unify network monitoring, security, correlation and 
qualification in one single tool. Using Snort, Acid, Mrtg, NTOP, 
OpenNMS, nmap, nessus and rrdtool we want the user to have full control 
over every network or security aspect.

%package agent
Summary:   OSSIM Agent
Group:     Applications/Security

%description agent
OSSIM Agent

%package perl
Summary:   OSSIM Perl Module
Group:     Applications/Security

%description perl
OSSIM Perl Module

%package framework
Summary:   OSSIM Web framework
Group:     Applications/Security
Requires:  php >= 4.0 php-domxml >= 4.0 os-sim-perl >= %{version}

%description framework
OSSIM Web framework

%package scripts
Summary:   OSSIM Scripts
Group:     Applications/Security
Requires:  os-sim-perl >= %{version}

%description scripts
OSSIM Scripts

%prep
%setup -q

%build
%{__aclocal}
%{__autoheader}
%{__autoconf}
%{__automake} --add-missing --gnu
%configure
%{__make}

%install
%{__rm} -rf $RPM_BUILD_ROOT
%makeinstall prefix=$RPM_BUILD_ROOT

%{__install} -d -m0755 $RPM_BUILD_ROOT/%{perl_sitearch}
%{__cp} -f include/ossim_conf.pm $RPM_BUILD_ROOT/%{perl_sitearch}

%{__install} -d -m0755 $RPM_BUILD_ROOT/var/www/cgi-bin
%{__cp} -f scripts/draw_graph.pl $RPM_BUILD_ROOT/var/www/cgi-bin
%{__cp} -f scripts/draw_graph_combined.pl $RPM_BUILD_ROOT/var/www/cgi-bin

%{__install} -d -m0755 $RPM_BUILD_ROOT/etc/httpd/conf.d
%{__cp} -f etc/httpd/ossim.conf $RPM_BUILD_ROOT/etc/httpd/conf.d

%clean
%{__rm} -rf $RPM_BUILD_ROOT

%post agent
if [ -L %{_bindir}/ossim-agent ] || [ ! -e %{_bindir}/ossim-agent ] ; then
	rm -f %{_bindir}/ossim-agent; ln -sf %{_datadir}/ossim/agent/ossim-agent %{_bindir}/ossim-agent
fi

%post framework
if [ ! -e /var/www/ossim-users ] ; then
	touch /var/www/ossim-users
fi

%postun agent
if [ -L %{_bindir}/ossim-agent ] ; then
	rm -f %{_bindir}/ossim-agent
fi

%files
%defattr(-,root,root,0755)
%doc AUTHORS BUGS ChangeLog CONFIG COPYING FAQ FILES INSTALL* LICENSE
%doc NEWS README* TODO
%config %{_sysconfdir}/ossim/server/config.xml
%config %{_sysconfdir}/ossim/server/directives.xml
%config %{_sysconfdir}/ossim/server/generic.xml
%config %{_sysconfdir}/ossim/server/trojans.xml
%config %{_sysconfdir}/ossim/server/directives.dtd
%{_bindir}/ossim-server
%{_datadir}/ossim/db/
/var/log/ossim

%files agent
%defattr(-,root,root,0755)
%config %{_sysconfdir}/ossim/agent/config.xml
%{_datadir}/ossim/agent/
%attr(0755,root,root) %{_datadir}/ossim/agent/ossim-agent
/var/log/ossim

%files perl
%defattr(-,root,root,0755)
%config %{_sysconfdir}/ossim/framework/ossim.conf
%{perl_sitearch}
%{_datadir}/ossim/perl/

%files framework
%defattr(-,root,root,0755)
%config %{_sysconfdir}/ossim/framework/mrtg-rrd.cfg
%config %{_sysconfdir}/httpd/conf.d/ossim.conf
%{_datadir}/ossim/fonts/
%{_datadir}/ossim/mrtg/
%{_datadir}/ossim/php/
%{_datadir}/ossim/pixmaps/
%{_datadir}/ossim/scripts/control_panel.py
%{_datadir}/ossim/scripts/draw_graph.pl
%{_datadir}/ossim/scripts/draw_graph_combined.pl
%{_datadir}/ossim/scripts/draw_graph_fournier.pl
%{_datadir}/ossim/scripts/get_date.pl
%{_datadir}/ossim/scripts/get_rrd_value.pl
%{_datadir}/ossim/scripts/create_sidmap.pl
%{_datadir}/ossim/scripts/restoredb.pl
%attr(0755,root,root) /var/www/cgi-bin/draw_graph.pl
%attr(0755,root,root) /var/www/cgi-bin/draw_graph_combined.pl
/var/www/ossim/

%files scripts
%{_datadir}/ossim/scripts/chkconfig.pl
%{_datadir}/ossim/scripts/rrd_plugin.pl
%{_datadir}/ossim/scripts/do_nessus.pl
%{_datadir}/ossim/scripts/netbios.pl
%{_datadir}/ossim/scripts/services.pl
%{_datadir}/ossim/scripts/update_nessus_ids.pl
%{_datadir}/ossim/scripts/backupdb.pl
%{_datadir}/ossim/scripts/acid_cache.pl
%{_datadir}/ossim/scripts/test-directive.pl
/var/lib/ossim/backup

%changelog
* Fri Sep 24 2004 Dominique Karg <dk@ossim.net> 0.9.7-1
- New Release

* Fri May 05 2004 Fabio Ospitia Trujillo <fot@ossim.net> 0.9.4-1
- New packages: perl and scripts.
- New Release

* Wed Mar 24 2004 Fabio Ospitia Trujillo <fot@ossim.net> 0.9.3-1
- New Release

* Wed Mar 24 2004 Fabio Ospitia Trujillo <fot@ossim.net> 0.9.2-1
- New Release

* Thu Mar 03 2004 Fabio Ospitia Trujillo <fot@ossim.net> 0.9.1-1
- New Release

* Thu Jan 29 2004 Fabio Ospitia Trujillo <fot@ossim.net> 0.9.0-1
- Initial build.
