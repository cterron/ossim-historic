%{?dist: %{expand: %%define %dist 1}}
%define vendor OSSIM

Summary:   Open Source Security Information Management (OSSIM)
Name:      ossim
Version:   0.9.8
Release:   2
License:   BSD
Group:     Applications/Security
URL:       http://www.ossim.net
Distribution: %{vendor}
Source0:   %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot

BuildRequires: glib2-devel libgda-devel >= 1.0.4 gnet2-devel >= 2.0.4 python gettext autoconf automake gcc

Requires: ossim-server = %{version} ossim-framework = %{version} ossim-utils = %{version} ossim-agent = %{version} ossim-contrib = %{version} ossim-mysql = %{version}


%description
OSSIM Open Source Security Information Management. All packages depend from it. 

%package server
Summary: OSSIM Server
Group:	 Applications/Security
Requires: glib2 >= 2.4.6 libgda >= 1.0.4 gnet2 >= 2.0.4 gda-mysql >= 1.0.4

%description server
OSSIM aims to unify network monitoring, security, correlation and 
qualification in one single tool. Using Snort, Acid, Mrtg, NTOP, 
OpenNMS, nmap, nessus and rrdtool we want the user to have full control 
over every network or security aspect.


%package agent
Summary:   OSSIM Agent
Group:     Applications/Security
Requires: python >= 2.3 MySQL-python >= 0.9.2 

%description agent
OSSIM Agent
An agent in OSSIM is set of python script that gathers and sends the
output of the different plugin or tool to the correlation engine for
further process.


%package utils
Summary:   OSSIM Utils
Group:     Applications/Security
Requires: perl perl-Compress-Zlib perl-DBI perl-DBD-MySQL rrdtool perl-rrdtool

%description utils
OSSIM Utils



%package framework
Summary:   OSSIM Web framework
Group:     Applications/Security
Requires:  ossim-utils php >= 4.3.4 httpd  php-adodb php-mysql >= 4.3.4  rrdtool python python-rrdtool MySQL-python nmap php-jpgraph base-ossim  phpgacl adodb
%{?rhfc2:Requires:   php-domxml >= 4.3.4  }
%{?rhfc3:Requires:   php-gd >= 4.3.4 php-xml >= 4.3.4  }
%{?rhel4:Requires:   php-gd >= 4.3.4 php-xml >= 4.3.4 }
%{?rhfc4:Requires:   php-gd >= 4.3.4 php-xml >= 4.3.4 }


%description framework
OSSIM Web framework


%package contrib
Summary: OSSIM contrib
Group:   Applications/Security

%description contrib
Open Source Security Information Management (Contrib)


%package mysql
Summary: OSSIM Mysql
Group:   Applications/Security
Requires: mysql mysql-server

%description mysql
Open Source Security Information Management (mysql)


%prep
%setup -q

%build
%{__aclocal}
%{__autoheader}
%{__autoconf}
%{__automake} --add-missing --gnu
%configure --sysconfdir=/etc --localstatedir=/var
%{__make}

%install
%{__rm} -rf $RPM_BUILD_ROOT
%makeinstall prefix=$RPM_BUILD_ROOT
cd agent
python setup.py install --prefix=$RPM_BUILD_ROOT/usr
cd -
cd frameworkd
python setup.py install --prefix=$RPM_BUILD_ROOT/usr
cd -


# locales
LANGUAGES="es de en fr ja"

for lang in $LANGUAGES; do \
    /usr/bin/msgfmt -v -o locale/$lang/LC_MESSAGES/ossim.mo locale/$lang/LC_MESSAGES/ossim.po; \
done

%{__install} -d -m0755 $RPM_BUILD_ROOT/usr/share/locale

for lang in $LANGUAGES; do \
        %{__install} -d $RPM_BUILD_ROOT/usr/share/locale/$lang/LC_MESSAGES/ ; \
        %{__install} -m 0644 locale/$lang/LC_MESSAGES/ossim.?o $RPM_BUILD_ROOT/usr/share/locale/$lang/LC_MESSAGES/ ; \
done

%{__install} -d -m0755 $RPM_BUILD_ROOT/%{perl_sitearch}
%{__cp} -f include/ossim_conf.pm $RPM_BUILD_ROOT/%{perl_sitearch}

%{__install} -d -m0755 $RPM_BUILD_ROOT/var/www/cgi-bin
%{__cp} -f scripts/draw_graph_fournier.pl $RPM_BUILD_ROOT/var/www/cgi-bin/draw_graph.pl
%{__cp} -f scripts/draw_graph_combined.pl $RPM_BUILD_ROOT/var/www/cgi-bin


%{__install} -d -m0755 $RPM_BUILD_ROOT/etc/httpd/conf.d
%{__cp} -f etc/httpd/ossim.conf $RPM_BUILD_ROOT/etc/httpd/conf.d

# core sql
%{__install} -d -m0755 $RPM_BUILD_ROOT/usr/share/ossim/db/
%{__cp} -f db/*.sql  $RPM_BUILD_ROOT/usr/share/ossim/db/

# contrib sql
%{__cp} -f contrib/acid/create_acid_tbls_mysql.sql $RPM_BUILD_ROOT/usr/share/ossim/db/create_acid_tbls_mysql.sql 
%{__cp} -f contrib/snort/create_snort_tbls_mysql.sql $RPM_BUILD_ROOT/usr/share/ossim/db/create_snort_tbls_mysql.sql

# fedora init scripts 
%{__install} -d -m0755 $RPM_BUILD_ROOT/etc/init.d
%{__cp} -f contrib/fedora/init.d/ossim-agent $RPM_BUILD_ROOT/etc/init.d/ossim-agent
%{__cp} -f contrib/fedora/init.d/ossim-server $RPM_BUILD_ROOT/etc/init.d/ossim-server
%{__cp} -f contrib/fedora/init.d/ossim-framework $RPM_BUILD_ROOT/etc/init.d/ossim-framework

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


%files server
%defattr(-,root,root,0755)
%doc AUTHORS BUGS ChangeLog CONFIG COPYING FAQ FILES INSTALL* LICENSE
%config %{_sysconfdir}/ossim/server/config.xml
%config %{_sysconfdir}/ossim/server/directives.xml
%config %{_sysconfdir}/ossim/server/generic.xml
%config %{_sysconfdir}/ossim/server/trojans.xml
%config %{_sysconfdir}/ossim/server/directives.dtd
%config %{_sysconfdir}/logrotate.d/ossim-server
%{_bindir}/ossim-server
/var/log/ossim
/etc/init.d/ossim-server

%files agent
%defattr(-,root,root,0755)
%doc ChangeLog
%config %{_sysconfdir}/ossim/agent/config.xml
%config %{_sysconfdir}/ossim/agent/plugins
%{_datadir}/ossim/agent/
%{_datadir}/ossim-agent/pyossim/
%{_datadir}/doc/ossim-agent/
%{_mandir}/man8/ossim-agent.8.gz
%{_bindir}/ossim-agent
%config %{_sysconfdir}/logrotate.d/ossim-agent
/etc/init.d/ossim-agent
/var/log/ossim

%files utils
%defattr(-,root,root,0755)
%doc ChangeLog
%config %{_sysconfdir}/ossim/framework/ossim.conf
%{perl_sitearch}
%{_datadir}/ossim/perl/
%{_datadir}/ossim/scripts/
/var/lib/ossim/backup

%files framework
%defattr(-,root,root,0755)
%doc ChangeLog
%config %{_sysconfdir}/ossim/framework/mrtg-rrd.cfg
%config %{_sysconfdir}/httpd/conf.d/ossim.conf
%config %{_sysconfdir}/cron.d/ossim-framework
%config %{_sysconfdir}/cron.daily/ossim-backup
%config %{_sysconfdir}/cron.daily/acid-backup
%config %{_sysconfdir}/logrotate.d/ossim-framework
%{_bindir}/ossim-framework
%{_datadir}/ossim-framework/ossimframework/
%{_datadir}/ossim/fonts/
%{_datadir}/ossim/mrtg/
%{_datadir}/ossim/include/
%{_datadir}/ossim/pixmaps/
%{_datadir}/ossim/www/
%{_datadir}/locale/
%attr(0755,root,root) /var/www/cgi-bin/draw_graph.pl
%attr(0755,root,root) /var/www/cgi-bin/draw_graph_combined.pl
/etc/init.d/ossim-framework
/var/lib/ossim/rrd

%files contrib
%defattr(-,root,root,0755)
%doc ChangeLog
%{_datadir}/ossim/contrib/

%files mysql
%doc ChangeLog
%defattr(-,root,root,0755)
%{_datadir}/ossim/db/


%changelog
* Sun Nov 22 2005 Scott R. Shinn <scott@atomicrocketturtle.com> 0.9.8-2
- included missing sql
- included missing init script
- update for ART dar
- path tweak for ossim.pm
- build updates for rh9/rhel3/rhfc2

* Mon Jun 27 2005 Juan Manuel Lorenzo Sarria <juanma@ossim.net> 0.9.8-1
- New release, packages for FC3 and FC4

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
