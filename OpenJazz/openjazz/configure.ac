AC_INIT([openjazz], [20190106], [https://github.com/AlisterT/openjazz/issues], [openjazz], [http://www.alister.eu/jazz/oj/])

AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects tar-pax no-dist-gzip dist-xz])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CANONICAL_HOST
AC_PROG_CXX
AC_PROG_CXXCPP
PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([SDL], [sdl])
PKG_CHECK_MODULES([LIBXMP], [libxmp], [AC_DEFINE(USE_XMP, [1], [Enable XMP Music])], [
	PKG_CHECK_MODULES([LIBMODPLUG], [libmodplug], [AC_DEFINE(USE_MODPLUG, [1], [Enable ModPlug Music])], [
		AC_MSG_WARN([No music library found. OpenJazz will run, but music will be silent.])
	])
])
PKG_CHECK_MODULES([ZLIB], [zlib])

AS_CASE([$host],
	[*-*-mingw*], [NET_LIBS="-lws2_32" OJ_HOST="MinGW (Windows)"],
	[*-haiku*], [HOST_CFLAGS="-D_BSD_SOURCE" HOST_LIBS="-lbe" NET_LIBS="-lnetwork" OJ_HOST="Haiku"],
	[arm-none-linux-gnueabi], [HOST_CFLAGS="-DPANDORA -fsigned-char" HOST_LIBS="-lts" OJ_HOST="Pandora"],
	[arm-gph-linux-gnueabi], [HOST_CFLAGS="-DCAANOO -fsigned-char" OJ_HOST="GP2X Caanoo"],
	[arm-openwiz-linux-gnu], [HOST_CFLAGS="-DWIZ -fsigned-char" OJ_HOST="GP2X Wiz"],
	[arm-open2x-linux], [HOST_CFLAGS="-DGP2X -fsigned-char" OJ_HOST="GP2X"],
	[mipsel-linux*], [HOST_CFLAGS="-DDINGOO" OJ_HOST="Dingoo"],
	[armv7l-unknown-linux-gnueabihf], [HOST_CFLAGS="-DGAMESHELL" OJ_HOST="GameShell"])
AC_SUBST(HOST_CFLAGS)
AC_SUBST(HOST_LIBS)
AC_SUBST(NET_LIBS)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AS_IF([test "x$OJ_HOST" != "x"],[
	echo ""
	echo "Detected Platform: $OJ_HOST. If this is wrong, please open a bug report!"
	echo ""
])
