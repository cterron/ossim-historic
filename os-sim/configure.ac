dnl Process this file with autoconf to produce a configure script.

AC_INIT(server/main.c)
AM_CONFIG_HEADER(config.h)

PACKAGE=server
VERSION=0.1

AM_INIT_AUTOMAKE($PACKAGE,$VERSION)

AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_INSTALL

PKG_CHECK_MODULES(DEPS, glib-2.0 >= 2.2 gobject-2.0 >= 2.2 gthread-2.0 >= 2.2 libgda >= 0.9 gnet-2.0 > 2.0)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_OUTPUT([
Makefile 
server/Makefile 
pixmaps/Makefile 
])
