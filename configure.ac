dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
LT_PREREQ([2.4.6])
AC_INIT([translated server daemon], [1.0.0], [xtwxy@hotmail.com], [translated], [https://github.com/xtwxy])
AC_PROG_CC

AC_CANONICAL_SYSTEM
#AC_ENABLE_SHARED

AC_CONFIG_MACRO_DIR([m4])

dnl Initialize automake
AM_INIT_AUTOMAKE([-Wall -Werror tar-ustar subdir-objects 1.9])

dnl this allows us specify individual liking flags for each target
AM_PROG_CC_C_O 
AM_PROG_AR

dnl Initialize Libtool
LT_INIT

AC_CONFIG_HEADERS([include/translated/config.h])

AC_CONFIG_FILES([
    Makefile
    include/Makefile
    libevq/Makefile
    librpc/Makefile
    libmodbus/Makefile
    translated/Makefile
    pkginfo/Makefile
])

AC_OUTPUT
