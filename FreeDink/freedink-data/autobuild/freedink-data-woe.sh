#!/bin/bash -ex

##	Derived release (doesn't include Audacity and Rosegarden
##	projects, original Ogg Vorbis files, etc.)
##      Used for the NSIS installer
if [ $# -ne 1 ]; then
    echo "Usage: $0 version"
    exit 1
fi

PACKAGE=freedink-data
VERSION=$1

export LANG=C.UTF8
unset LC_ALL LANGUAGE

rm -rf $PACKAGE-$VERSION/
tar xf $PACKAGE-$VERSION.tar.gz
pushd $PACKAGE-$VERSION/
make install DESTDIR=`pwd`/t
mkdir zip/

# Documentation
for i in *.txt; do
    cp $i zip/freedink-data-$i
done
for i in COPYING NEWS; do
    cp $i zip/freedink-data-$i.txt
done
cp -a licenses zip/
for i in zip/*.* zip/licenses/*.txt zip/licenses/URLS; do
    LANG=C.UTF-8 sed -i -e 's/\(^\|[^\r]\)$/\1\r/' $i
done

# Dink
mv t/usr/local/share/dink/dink/ zip/

# Set reproducible date for all generated files:
# (considering the tarball has clean dates... which it has not)
SOURCE_DATE_EPOCH=$(date -d$(echo $VERSION | grep -Po '\d{8}')Z0000 +%s)
find zip/ -newermt "@${SOURCE_DATE_EPOCH}" -print0 \
  | xargs -0r touch --no-dereference --date="@${SOURCE_DATE_EPOCH}"

rm -f ../$PACKAGE-$VERSION-nosrc.zip
# Reproducible build:
# TZ=UTC: avoid MS-DOS timestamp variations due to timezone
#   https://wiki.debian.org/ReproducibleBuilds/TimestampsInZip
# -X: strip platform-specific info (timestamps, uid/gid, permissions)
# sort file list https://wiki.debian.org/ReproducibleBuilds/FileOrderInTarballs
find zip/ -type d -print0 | xargs -r -0 chmod 00755
find zip/ -type f -print0 | xargs -r -0 chmod 00644
(cd zip/ && find . -print0 \
  | LC_ALL=C sort -z \
  | TZ=UTC xargs -0 -n10 \
    zip -X ../../$PACKAGE-$VERSION-nosrc.zip)
popd  # $PACKAGE-$VERSION/
