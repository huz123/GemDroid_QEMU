#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([mjpg_streamer], [2.0], [1988pb@gmail.com])
AC_CONFIG_SRCDIR([plugins/input.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_INIT_AUTOMAKE
AC_PROG_RANLIB
AC_PROG_LIBTOOL
# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h malloc.h stdlib.h string.h sys/file.h sys/ioctl.h sys/socket.h sys/time.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday inet_ntoa memmove memset munmap select socket strchr strdup strndup strspn strstr strtol])

AC_CONFIG_FILES([Makefile src/Makefile plugins/Makefile])

AC_OUTPUT

