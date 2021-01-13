DESTDIR=
PREFIX=/usr/local
DATADIR=$(PREFIX)/share
VERSION:=1.08.$(shell date +%Y%m%d)
releasedir=freedink-data-$(VERSION)
SOURCE_DATE_EPOCH:=$(shell date -d$$(echo $(VERSION) | grep -Po '\d{8}')Z0000 +%s)

all:
	@echo "No default action"

install:
#	Copy game data

#	The final directory is '$PREFIX/share/dink/dink' ('dink'
#	twice), as there can be site-wide D-Mods such as
#	'$PREFIX/share/dink/island'
	install -d -m 755 $(DESTDIR)$(DATADIR)/dink

#	'-a' will preserve timestamps, which some distros prefer. It
#	will also preserve symlinks, which may be useful as there are
#	a few duplicate musics. Use a .zip archive for woe releases,
#	it will duplicate files but avoid creating empty files instead
#	of symlinks.
	cp -a dink $(DESTDIR)$(DATADIR)/dink/

#	Tidy permissions
	find $(DESTDIR)$(DATADIR)/dink/dink/ -type d -print0 | xargs -0r chmod 755
	find $(DESTDIR)$(DATADIR)/dink/dink/ -type f -print0 | xargs -0r chmod 644

# Release:
# (Do this from a fresh Git checkout to avoid packaging temporary files)
dist: update-gmo
##	Source release

#	Not using Git because that doesn't ship the .mo files
#	Plus that forbids testing uncommitted changes
#	git archive --format=tar.gz --prefix=$(releasedir)/ -o > $(releasedir).tar.gz HEAD \
#	  -- ChangeLog COPYING NEWS *.txt *.spec Makefile autobuild/ debian/ dink/ doc/ licenses/ soundtest/ src/ \

	rm -rf $(releasedir)
	mkdir $(releasedir)
	chmod 00755 $(releasedir)
	umask 022; cp -dR ChangeLog COPYING NEWS *.txt *.spec Makefile autobuild/ debian/ dink/ doc/ licenses/ soundtest/ src/ $(releasedir)

#	Clean-up:
#	git files
	find $(releasedir)/ -name ".gitignore" -print0 | xargs -0r rm
#	backup files
	find $(releasedir)/ -name "*~" -print0 | xargs -0r rm
#	savegames
	find $(releasedir)/ -iname "save*.dat" -print0 | xargs -0r rm
#	cheat^Wdebugging tool
	rm -f $(releasedir)/dink/Story/key-67.c
#	debug mode output
	rm -f $(releasedir)/dink/DEBUG.TXT

#	Tarball:
#	Make it reproducible, not really needed, but fun and doesn't leak metadata
#	https://reproducible-builds.org/docs/archives/
#	Though any update to this Makefile changes the release checksum...
	tar -c --sort=name \
	  --mtime="@$(SOURCE_DATE_EPOCH)" \
	  --owner=root --group=root --numeric-owner \
	  $(releasedir)/ | gzip -n > $(releasedir).tar.gz

	rm -rf $(releasedir)

# Compile translation strings catalogs
update-gmo:
	cd dink/l10n; \
	for i in $$(ls ??.po | sed 's/\.po$$//'); do \
		mkdir -p $$i/LC_MESSAGES; \
		echo -n "$$i: "; \
		msgfmt --statistics $$i.po -o $$i/LC_MESSAGES/dink.mo; \
	done

reprotest:
	reprotest \
	  'make dist VERSION=$(VERSION) && ./autobuild/freedink-data-woe.sh $(VERSION) \
	    && chmod 644  \$(releasedir).tar.gz $(releasedir)-nosrc.zip' \
	  '$(releasedir).tar.gz $(releasedir)-nosrc.zip'
