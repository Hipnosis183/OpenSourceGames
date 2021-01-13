Instructions to build from scratch (from the repository sources).

If you downloaded a release you can skip the "Bootstrap" instructions
and go to "Dependencies" directly.

These instructions may sound redundant with the packaging specs (.deb,
.rpm, .ebuild, etc.) but they are necessary for people who want to
compile the latest, not-yet-packaged sources :)


Quick version
=============

Requires wxWidgets and bzip2 to compile... Just type

  ./configure && make && make install

to get it working.


Issues with wxGlade
==================

If you're recompiling from Git or modifying the .wxg files, there's an
issue with wxGlade [0.6.6, 0.6.9[.
See https://sourceforge.net/p/wxglade/bugs/161/ for details and a fix.

There is also an issue with IDs generation in .cpp files with 0.7.2.
http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=853009
Upgrading to 2017-01-28's default branch worked for me...
https://bitbucket.org/wxglade/wxglade/get/f172c83ff51d.zip
Then:
WXGLADE=/usr/src/wxglade/wxglade ./configure ...

Last, earlier versions of wxGlade are not officially compatible with
wxWidgets 3.
https://bitbucket.org/wxglade/wxglade/commits/acbfabfba67bc795a6ccecaabda933ecb0d0f63d#chg-wxglade.py
There's a work-around in 'src/Makefile.am', we can remove it when recent wxglade is ubiquitous.


On a minimal Debian system
==========================

## Bootstrap
# Source code:
apt-get install git-core
git clone git://git.sv.gnu.org/freedink/dfarc
cd dfarc

# autotools
apt-get install autoconf automake

apt-get install wx-common # for wxwin.m4
apt-get install intltool  # for intltool.m4
sh bootstrap
apt-get install python-wxglade


## Dependencies
# Base: GCC, make & al.
apt-get install build-essential
# DFArc uses bzip2, wxwidgets
apt-get install libbz2-dev libwxgtk3.0-dev intltool
# Optional:
# - upx compresses binary
# - xdg-utils to detect file browser
apt-get install upx-ucl xdg-utils

## Compilation
./configure
make
make install

## Release tests
make dist
make distcheck

# :)


On a minimal Fedora system
==========================

## Bootstrap
# Source code:
yum install git-core
git clone git://git.sv.gnu.org/freedink/dfarc
cd dfarc

# autotools
yum install autoconf automake

yum install wxGTK3-devel # for wxwin.m4
yum install intltool    # for intltool.m4
# beware: you need intltool >= 0.40 to bootstrap a proper
# configure.ac, otherwise you'll get old-style included intltool tools
# in your archive (instead of depending on the user system one), and
# which don't work well with AC_CONFIG_AUX_DIR
sh bootstrap


## Dependencies
# Base: GCC, make & al.
# Note: 'groupinstall' not working with pkcon yet
yum groupinstall 'Development Tools'
# or just:
#yum install make gcc-c++
# DFArc uses bzip2, wxwidgets
yum install wxGTK3-devel bzip2-devel intltool
# Optional:
# - upx compresses binary
# - xdg-utils to detect file browser
yum install upx xdg-utils

## Compilation
./configure --with-wx-config=/usr/libexec/wxGTK3/wx-config
make
make install

## Release tests
make dist
make distcheck

# :)


On a minimal Gentoo system
==========================

## Bootstrap
# Source code:
emerge dev-util/git
git clone git://git.sv.gnu.org/freedink/dfarc
cd dfarc

# I assume you already have autoconf & al. ;)

# Make sure you have the 'X' USE in /etc/make.conf or
# echo "x11-libs/wxGTK X" >> /etc/portage/package.use 
# echo "x11-libs/cairo X" >> /etc/portage/package.use 
emerge wxGTK    # for wxwin.m4
emerge intltool # for intltool.m4
# beware: you need intltool >= 0.40 to bootstrap a proper
# configure.ac, otherwise you'll get old-style included intltool tools
# in your archive (instead of depending on the user system one), and
# which don't work well with AC_CONFIG_AUX_DIR
sh bootstrap


## Dependencies
# I also assume you already have GCC, Make and gettext ;)
# DFArc uses bzip2, wxwidgets and intltool
emerge wxGTK
eselect wxwidgets list
eselect wxwidgets set 1 # or any option from the list above
emerge bzip2 intltool
# Optional:
# - upx compresses binary
# - xdg-utils to detect file browser
emerge upx xdg-utils


## Compilation
./configure
make
make install

## Release tests
make dist
make distcheck

# :)


On a minimal FreeBSD 6.3 system
===============================

## Bootstrap
# Source code:
pkg_add -r git
git clone git://git.sv.gnu.org/freedink
cd freedink

# autotools
# Note: you need to specify explicit versions
pkg_add -r autoconf261 automake19

pkg_add -r wxgtk2-common # for wxwin.m4
#pkg_add -r intltool      # for intltool.m4
# beware: you need intltool >= 0.40 to bootstrap a proper
# configure.ac, otherwise you'll get old-style included intltool tools
# in your archive (instead of depending on the user system one), and
# which don't work well with AC_CONFIG_AUX_DIR
set VERSION=0.40.0
wget http://ftp.acc.umu.se/pub/GNOME/sources/intltool/0.40/intltool-$VERSION.tar.bz2
tar xjf intltool-$VERSION.tar.bz2
pkg_add -r gmake
./configure && gmake && gmake install
cd intltool-$VERSION

sh bootstrap


## Dependencies
# I assume you already have GCC and Make ;)
# Required: wxWidgets, libbz2
pkg_add -r wxgtk2-unicode
pkg_add -r bzip2 # also installs libbz2
pkg_add -r intltool
./configure && make && make install
# Optional:
# - upx compresses binary
# - xdg-utils to detect file browser
pkg_add -r upx xdg-utils

# gmake is necessary for Makefile.in.in from intltool
pkg_add -r gmake

## Compilation
./configure --with-wx-config=wxgtk2u-2.8-config
gmake
gmake install

## Bug: intltool installs .mo files in $prefix/lib, check:
# http://bugzilla.gnome.org/show_bug.cgi?id=563034

## Release tests
make dist
make distcheck

# :)


On a minimal Woe system
=======================

Check doc/cross.txt (for cross compiling from GNU/Linux + MinGW32).
