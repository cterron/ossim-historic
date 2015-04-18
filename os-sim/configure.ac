AC_PREREQ(2.52)
AC_INIT(src/main.c)

AM_CONFIG_HEADER(config.h)

OS_SIM_MAJOR_VERSION=0
OS_SIM_MINOR_VERSION=9
OS_SIM_MICRO_VERSION=5
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


# Check for structures
AC_CHECK_MEMBER([struct sockaddr.sa_len],
                AC_DEFINE(HAVE_SOCKADDR_SA_LEN, 1,
                  [Define if struct sockaddr has sa_len field]),,
                [#include <sys/types.h>
                 #include <sys/socket.h>])

AC_MSG_CHECKING([for struct sockaddr_storage])
AC_TRY_COMPILE([ #include <sys/types.h>
#include <sys/socket.h>], [struct sockaddr_storage s; s],
  have_sockaddr_storage=yes, have_sockaddr_storage=no)
if test "$have_sockaddr_storage" = yes; then
  AC_DEFINE(HAVE_SOCKADDR_STORAGE,1,[Whether you have struct sockaddr_storage])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

# Check for IPv6 support
AC_MSG_CHECKING([for IPv6 availability])
AC_TRY_LINK([ #include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>], [struct sockaddr_in6 s; struct in6_addr t=in6addr_any; int i=AF_INET6; s; t.s6_addr[0] = 0;],
  [
    AC_DEFINE(HAVE_IPV6, 1, [Define if IPv6 is potentially available])
    AC_MSG_RESULT(yes)
  ],
  [
    AC_MSG_RESULT(no)
  ]
)

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
www/pixmaps/os/Makefile
www/policy/Makefile
www/port/Makefile
www/report/Makefile
www/riskmeter/Makefile
www/rrd_conf/Makefile
www/scan/Makefile
www/sensor/Makefile
www/sec_report/Makefile
www/sec_report/graphs/Makefile
www/signature/Makefile
www/style/Makefile
www/vulnmeter/Makefile
mrtg/Makefile
mrtg/level/Makefile
mrtg/global/Makefile
mrtg/hosts/Makefile
mrtg/nets/Makefile
scripts/Makefile
contrib/Makefile
contrib/fonts/Makefile
])
