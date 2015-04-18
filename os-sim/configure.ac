AC_PREREQ(2.52)
AC_INIT(src/main.c)

AM_CONFIG_HEADER(config.h)

OS_SIM_MAJOR_VERSION=0
OS_SIM_MINOR_VERSION=8
OS_SIM_MICRO_VERSION=1
OS_SIM_VERSION=$OS_SIM_MAJOR_VERSION.$OS_SIM_MINOR_VERSION.$OS_SIM_MICRO_VERSION

AC_SUBST(OS_SIM_MAJOR_VERSION)
AC_SUBST(OS_SIM_MINOR_VERSION)
AC_SUBST(OS_SIM_MICRO_VERSION)
AC_SUBST(OS_SIM_VERSION)

AM_INIT_AUTOMAKE(os-sim, $OS_SIM_VERSION)

AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CXX
AC_STDC_HEADERS
#AM_PROG_LIBTOOL
AC_PROG_INSTALL

PKG_CHECK_MODULES(DEPS, glib-2.0 >= 2.2 gobject-2.0 >= 2.2 gthread-2.0 >= 2.2 libgda >= 1.0 gnet-2.0 > 2.0)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_OUTPUT([
Makefile 
src/Makefile
agent/Makefile 
etc/Makefile
etc/server/Makefile
etc/agent/Makefile
etc/framework/Makefile
db/Makefile
pixmaps/Makefile 
include/Makefile
include/classes/Makefile
www/Makefile
www/conf/Makefile
www/control_panel/Makefile
www/directives/Makefile
www/editor/Makefile
www/host/Makefile
www/net/Makefile
www/ntop/Makefile
www/pixmaps/Makefile
www/policy/Makefile
www/port/Makefile
www/report/Makefile
www/riskmeter/Makefile
www/rrd_conf/Makefile
www/scan/Makefile
www/sensor/Makefile
www/signature/Makefile
www/style/Makefile
www/vulnmeter/Makefile
mrtg/Makefile
mrtg/global/Makefile
mrtg/hosts/Makefile
mrtg/nets/Makefile
scripts/Makefile
contrib/Makefile
contrib/fonts/Makefile
])
