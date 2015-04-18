Summary:   Open Source Security Information Management (OSSIM)
Name:      os-sim
Version:   0.9.0
Release:   1
License:   GPL
Group:     Applications/Security
URL:       http://www.ossim.net
Requires:  glib2 > 2.0
Requires:  libgda >= 1.0
Requires:  gnet2 >= 2.0
Source0:   %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot

BuildRequires: glib2-devel > 2.0 libgda-devel >= 1.0 gnet2-devel >= 2.0

%description
OSSIM pretends to unify network monitoring, security, correlation and 
qualification in one single tool. Using Snort, Acid, Mrtg, NTOP, 
OpenNMS, nmap, nessus and rrdtool we want the user to have full control 
over every network or security aspect.

%package agent
Summary:   OSSIM Agent
Group:     Applications/Security
Requires:  %{name} >= %{version}

%description agent
OSSIM Agent

%package framework
Summary:   OSSIM Web framework
Group:     Applications/Security
Requires:  %{name} >= %{version}

%description framework
OSSIM Web framework

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

%{__install} -d -m0755 $RPM_BUILD_ROOT/%{perl_archlib}
%{__cp} -f include/ossim_conf.pm $RPM_BUILD_ROOT/%{perl_archlib}

%{__install} -d -m0755 $RPM_BUILD_ROOT/var/www/cgi-bin
%{__cp} -f scripts/draw_graph.pl $RPM_BUILD_ROOT/var/www/cgi-bin

%{__install} -d -m0755 $RPM_BUILD_ROOT/etc/httpd/conf.d
%{__cp} -f etc/httpd/ossim.conf $RPM_BUILD_ROOT/etc/httpd/conf.d

%{__install} -d -m0755 $RPM_BUILD_ROOT/etc/php.d/
%{__cp} -f etc/ossim.ini $RPM_BUILD_ROOT/etc/php.d

%post agent
rm -f %{_bindir}/agent ; ln -sf %{_datadir}/ossim/agent/agent %{_bindir}/agent

%postun agent
if [ -L %{_bindir}/agent ] ; then
	rm -f %{_bindir}/agent
fi

%clean
%{__rm} -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,0755)
%doc AUTHORS BUGS ChangeLog CONFIG COPYING FAQ FILES INSTALL* LICENSE
%doc NEWS README* TODO
%config %{_sysconfdir}/ossim/server/config.xml
%config %{_sysconfdir}/ossim/server/directives.xml
%{_bindir}/ossim
%{_datadir}/ossim/db/

%files agent
%defattr(-,root,root,0755)
%config %{_sysconfdir}/ossim/agent/config.xml
%{_datadir}/ossim/agent/
%attr(0755,root,root) %{_datadir}/ossim/agent/agent

%files framework
%defattr(-,root,root,0755)
%config %{_sysconfdir}/ossim/framework/ossim.conf
%config %{_sysconfdir}/ossim/framework/mrtg.cfg
%config %{_sysconfdir}/ossim/framework/mrtg-rrd.cfg
%config %{_sysconfdir}/httpd/conf.d/ossim.conf
%config %{_sysconfdir}/php.d/ossim.ini
%{_datadir}/ossim/fonts/
%{_datadir}/ossim/mrtg/
%{_datadir}/ossim/perl/
%{_datadir}/ossim/php/
%{_datadir}/ossim/pixmaps/
%{_datadir}/ossim/scripts/
%{perl_archlib}
%attr(0755,root,root) /var/www/cgi-bin/draw_graph.pl
/var/www/ossim/


%changelog
* Thu Jan 29 2004 Fabio Ospitia Trujillo <fot@ossim.net> 0.9.0-1
- Initial build.
