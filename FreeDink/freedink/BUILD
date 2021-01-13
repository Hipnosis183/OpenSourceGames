Instructions to build from scratch (from the repository sources).

If you downloaded a release you can skip the "Bootstrap" instructions
and go to "Dependencies" directly.

These instructions may sound redundant with the packaging specs (.deb,
.rpm, .ebuild, etc.) but they are necessary for people who want to
compile the latest, not-yet-packaged sources :)


On a minimal Debian system
==========================

## Bootstrap
# Source code:
apt-get install git
git clone git://git.sv.gnu.org/freedink
cd freedink/

# Gnulib
(cd /usr/src && git clone git://git.sv.gnu.org/gnulib)
# or:
#apt-get install gnulib

# autoconf automake: base autotools
# pkg-config: for PKG_CHECK_MODULES
# help2man: to rebuild manpages
# gettext autopoint: for i18n
# rsync: to fetch translationproject.org
apt-get install autoconf automake pkg-config help2man gettext autopoint rsync
sh bootstrap


## Dependencies
# Base: GCC, make & al.
apt-get install build-essential
# Required: libraries
apt-get install pkg-config \
  libsdl2-dev \
  libsdl2-image-dev libsdl2-image-dbg \
  libsdl2-mixer-dev libsdl2-mixer-dbg \
  libsdl2-ttf-dev libsdl2-ttf-dbg \
  libsdl2-gfx-dev libsdl2-gfx-dbg \
  libfontconfig1-dev \
  libglm-dev \
  cxxtest
# Optional:
# - upx compresses binary
# - bzip is for .tar.bz2 release tarballs
apt-get install upx-ucl bzip2

## Compilation
./configure
make
make install

## Release tests
make dist
make distcheck

## Optional: software MIDI support, used by SDL_mixer
# Check doc/midi.txt for details
apt-get install timidity freepats

# :)


On a minimal Fedora system
==========================

(use 'yum' or 'pkcon' indifferently)

## Bootstrap
# Source code:
yum install git-core
git clone git://git.sv.gnu.org/freedink
cd freedink/

# Gnulib
(cd /usr/src && git clone git://git.sv.gnu.org/gnulib)
# No Fedora package, but there's no need for one.

# autoconf automake: base autotools
# pkg-config: for PKG_CHECK_MODULES
# help2man: to rebuild manpages
# gettext autpoint: for i18n
# rsync: to fetch translationproject.org
yum install autoconf automake pkg-config help2man gettext-devel rsync
sh bootstrap


## Dependencies
# Base: GCC, make & al.
yum groupinstall 'Development Tools'
# or just:
#yum install make gcc-c++
yum install SDL2-devel \
  SDL2_gfx-devel SDL2_ttf-devel SDL2_image-devel SDL2_mixer-devel \
  fontconfig-devel glm-devel
# Optional:
# - upx compresses binary
# - bzip is for .tar.bz2 release tarballs
yum install upx bzip2

## Compilation
./configure
make
make install

## Release tests
make dist
make distcheck

## Optional: software MIDI support, used by SDL_mixer
# Check doc/sound.txt for details
# timidity++ and timidity++-patches already installed as dependencies
#   from SDL_mixer

# :)


On a minimal Gentoo system
==========================

## Bootstrap
# Source code:
emerge dev-util/git
git clone git://git.sv.gnu.org/freedink
cd freedink

# Gnulib
(cd /usr/src && git clone git://git.sv.gnu.org/gnulib)
# or:
# Gnulib is currently masked, you need to unmask it:
# http://gentoo-wiki.com/Masked#Masked_by_missing_keyword
# Not that there are issues with the Gentoo wrapper. Use
# /usr/share/gnulib/gnulib-tool instead of /usr/bin/gnulib-tool if
# possible
#echo "dev-libs/gnulib **" >> /etc/portage/package.keywords
#emerge gnulib

# I assume you already have autoconf & al. ;)

emerge help2man # to rebuild manpages
emerge pkgconfig # for PKG_CHECK_MODULES
sh bootstrap


## Dependencies
# I also assume you already have GCC and Make ;)
emerge libsdl # (if not already done in bootstrap step)
emerge sdl-gfx sdl-ttf sdl-image sdl-mixer fontconfig
# Optional:
# - upx compresses binary
# - bzip is for .tar.bz2 release tarballs (included in base Gentoo)
emerge upx

./configure
make
make install

## Release tests
make dist
make distcheck

## Optional: software MIDI support, used by SDL_mixer
# Check doc/sound.txt for details
emerge media-sound/timidity++
echo "media-sound/timidity-freepats **" >> /etc/portage/package.keywords
emerge media-sound/timidity-freepats # GPLv>=2 + lax exception
emerge media-sound/timidity-eawpatches # non-free

# :)


On a minimal ArchLinux system
=============================

## Bootstrap
# Update packages list
pacman -Sy

# Source code:
pacman -S git
git clone git://git.sv.gnu.org/freedink
cd freedink

# Gnulib
(cd /usr/src && git clone git://git.sv.gnu.org/gnulib)
# No ArchLinux package, but there's no need for one.

# Install development tools (autoconf automake gcc m4 make pkgconfig)
pacman -S base-devel 

pacman -S help2man # to rebuild manpages
pacman -S gettext # for i18n
sh bootstrap


## Dependencies
# I also assume you already have GCC and Make ;)
pacman -S sdl # (if not already done in bootstrap step)
pacman -S sdl_gfx sdl_ttf sdl_image sdl_mixer fontconfig
# Optional:
# - upx compresses binary
# - bzip is for .tar.bz2 release tarballs (included in base Gentoo)
pacman -S bzip2 upx

./configure
make
make install

## Release tests
make dist
make distcheck

## Optional: software MIDI support, used by SDL_mixer
# Check doc/sound.txt for details
pacman -S timidity++
pacman -S timidity-freepats # GPLv>=2 + lax exception
cp /etc/timidity++/timidity-freepats.cfg /etc/timidity++/timidity.cfg
yaourt -S timidity-eawpatches # non-free, in the AUR
cp /etc/timidity++/timidity-eawpats.cfg /etc/timidity++/timidity.cfg

# :)


On a minimal FreeBSD 10.0 system
===============================

## Bootstrap
# Source code:
pkg install -y git
git clone git://git.sv.gnu.org/freedink
cd freedink/

# Gnulib
(cd /usr/src && git clone git://git.sv.gnu.org/gnulib)

# autoconf automake: base autotools
# pkgconf: for PKG_CHECK_MODULES
# help2man: to rebuild manpages
# gettext autopoint: for i18n
# rsync: to fetch translationproject.org
pkg install -y autoconf automake pkgconf help2man gettext rsync
sh bootstrap


## Dependencies
# I assume you already have GCC and Make ;)
pkg install -y sdl sdl_gfx sdl_ttf sdl_image sdl_mixer fontconfig
# Optional:
# - upx compresses binary
# - bzip is for .tar.bz2 release tarballs (included in base FreeBSD)
pkg install -y upx

./configure
make -k  # TODO: issue with spurious newlines added by msgmerge
make install -k

## Release tests
make dist
make distcheck

## Optional: software MIDI support, used by SDL_mixer
# Check doc/sound.txt for details
# timidity already installed as dependencies
#   from SDL_mixer
pkg install -y freepats

# To test, see http://www.freebsd.org/doc/en/books/handbook/x-config.html
# pkg install x11
# startx

# :)


On a minimal Woe system
=======================

Check doc/cross.txt (for cross compiling from GNU/Linux + MinGW32) or
doc/woe-compile.txt (for compiling from native MinGW32).
