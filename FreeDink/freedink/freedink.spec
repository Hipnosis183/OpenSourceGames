Name:		freedink
Version:	109.6
Release:	1%{?dist}
Summary:	Humorous top-down adventure and role-playing game

BuildRequires:	gcc-c++
BuildRequires:	SDL2-devel SDL2_gfx-devel SDL2_ttf-devel SDL2_image-devel SDL2_mixer-devel
BuildRequires:	fontconfig-devel
BuildRequires:	glm-devel
BuildRequires:	cxxtest
# https://docs.fedoraproject.org/en-US/packaging-guidelines/#_desktop_files
BuildRequires:	desktop-file-utils
# https://docs.fedoraproject.org/en-US/packaging-guidelines/AppData/
BuildRequires:	libappstream-glib
License:	GPLv3+
URL:		https://www.gnu.org/software/freedink/
Source0:	https://ftp.gnu.org/gnu/freedink/freedink-%{version}.tar.gz

Requires:	freedink-engine = %{version}-%{release} freedink-dfarc
# Reference bundled copy of gnulib - cf. https://fedorahosted.org/fpc/ticket/174
Provides:	bundled(gnulib)

%description
Dink Smallwood is an adventure/role-playing game, similar to Zelda,
made by RTsoft. Besides twisted humor, it includes the actual game
editor, allowing players to create hundreds of new adventures called
Dink Modules or D-Mods for short.

GNU FreeDink is a new and portable version of the game engine, which
runs the original game as well as its D-Mods, with close
compatibility, under multiple platforms.

This package is a meta-package to install the game, its data and a
front-end to manage game options and D-Mods.


%package engine
Summary:	Humorous top-down adventure and role-playing game (engine)
Requires:	freedink-data

%if 0%{?with_included_liberation_font}
# No dependency
%else
# Respect Fedora guidelines (see below)
Requires: liberation-sans-fonts
%endif

%description engine
Dink Smallwood is an adventure/role-playing game, similar to classic
Zelda, made by RTsoft. Besides twisted humor, it includes the actual
game editor, allowing players to create hundreds of new adventures
called Dink Modules or D-Mods for short.

GNU FreeDink is a new and portable version of the game engine, which
runs the original game as well as its D-Mods, with close
compatibility, under multiple platforms.

This package contains the game engine alone.


%prep
%autosetup -p1

%build
# Using '--disable-embedded-resources' because 'rpmbuild' will remove
# them anyway (so it can make the -debuginfo package -- too bad :/)
%configure --disable-embedded-resources 
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT INSTALL="install -p"
%find_lang %{name}
%find_lang %{name}-gnulib
# %%files only support one '-f' argument (see below)
cat %{name}-gnulib.lang >> %{name}.lang
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}.desktop
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}edit.desktop
appstream-util validate-relax --nonet %{buildroot}%{_metainfodir}/*.appdata.xml
# http://fedoraproject.org/wiki/Packaging/Guidelines#Avoid_bundling_of_fonts_in_other_packages
# Policy insists on not installing a different version of "Liberation
# Sans". Beware that the system version may be different than the
# official FreeDink font, because Liberation changes regularly.
%if 0%{?with_included_liberation_font}
# Include it nonetheless for the sake of avoiding
# liberation-fonts<->liberation-sans-fonts distro naming conflicts in
# the freedink.org RPM repository
%else
# Remove it for compliance with Fedora guidelines
rm $RPM_BUILD_ROOT%{_datadir}/%{name}/LiberationSans-Regular.ttf
%endif


%files

%files engine -f %{name}.lang
%doc AUTHORS COPYING NEWS README THANKS TROUBLESHOOTING ChangeLog
%{_bindir}/*
%{_datadir}/applications/*
%{_metainfodir}/*
%{_datadir}/%{name}/
%{_datadir}/pixmaps/*
%{_datadir}/icons/hicolor/scalable/apps/*
%{_mandir}/man6/*


%changelog
* Sat Feb 16 2019 Sylvain Beucler <beuc@beuc.net> - 109.6-1
- New upstream release

* Thu Jan 31 2019 Fedora Release Engineering <releng@fedoraproject.org> - 109.4-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_30_Mass_Rebuild

* Sun Jan 27 2019 Sylvain Beucler <beuc@beuc.net> - 109.4-2
- Keep AppStream metadata in freedink-engine package
- Backport AppStream metadata from upstream
- Only package the icon and not the (shared) icon directories

* Sun Jan 27 2019 Sylvain Beucler <beuc@beuc.net> - 109.4-1
- New upstream release

* Sat Jan 19 2019 Sylvain Beucler <beuc@beuc.net> - 109.2-1
- New upstream release

* Fri Jul 13 2018 Fedora Release Engineering <releng@fedoraproject.org> - 108.4-9
- Rebuilt for https://fedoraproject.org/wiki/Fedora_29_Mass_Rebuild

* Fri Jun 01 2018 Sylvain Beucler <beuc@beuc.net> - 108.4-8
- Fix crash on loading game (#1448761)

* Wed Feb 07 2018 Fedora Release Engineering <releng@fedoraproject.org> - 108.4-7
- Rebuilt for https://fedoraproject.org/wiki/Fedora_28_Mass_Rebuild

* Wed Aug 02 2017 Fedora Release Engineering <releng@fedoraproject.org> - 108.4-6
- Rebuilt for https://fedoraproject.org/wiki/Fedora_27_Binutils_Mass_Rebuild

* Wed Jul 26 2017 Fedora Release Engineering <releng@fedoraproject.org> - 108.4-5
- Rebuilt for https://fedoraproject.org/wiki/Fedora_27_Mass_Rebuild

* Fri Feb 10 2017 Fedora Release Engineering <releng@fedoraproject.org> - 108.4-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_26_Mass_Rebuild

* Wed Feb 03 2016 Fedora Release Engineering <releng@fedoraproject.org> - 108.4-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_24_Mass_Rebuild

* Wed Jun 17 2015 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 108.4-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_23_Mass_Rebuild

* Wed Oct 22 2014 Sylvain Beucler <beuc@beuc.net> - 108.4-1
- New upstream release

* Sat Aug 16 2014 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 108.2-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_21_22_Mass_Rebuild

* Fri Jun 13 2014 Hans de Goede <hdegoede@redhat.com> - 108.2-3
- Rebuild for new SDL_gfx

* Sat Jun 07 2014 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 108.2-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_21_Mass_Rebuild

* Thu May 29 2014 Sylvain Beucler <beuc@beuc.net> - 108.2-1
- New upstream release

* Sat Aug 03 2013 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.08.20121209-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_20_Mass_Rebuild

* Wed Feb 13 2013 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.08.20121209-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_19_Mass_Rebuild

* Sun Dec 09 2012 Sylvain Beucler <beuc@beuc.net> - 1.08.20120912-1
- New upstream release

* Thu Jul 19 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.08.20120427-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_18_Mass_Rebuild

* Sun Jun 10 2012 Sylvain Beucler <beuc@beuc.net> - 1.08.20120427-2
- Add virtual provides for bundled(gnulib) copylib (#821754)

* Fri Apr 27 2012 Sylvain Beucler <beuc@beuc.net> - 1.08.20120427-1
- New upstream release

* Fri Jan 13 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.08.20101114-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_17_Mass_Rebuild

* Tue Feb 08 2011 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.08.20101114-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_15_Mass_Rebuild

* Sun Nov 14 2010 Sylvain Beucler <beuc@beuc.net> - 1.08.20101114-1
- New upstream release

* Tue Apr 20 2010 Sylvain Beucler <beuc@beuc.net> - 1.08.20100420-1
- New upstream release

* Sun Mar 21 2010 Sylvain Beucler <beuc@beuc.net> - 1.08.20100321-1
- New upstream release

* Fri Sep 18 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090918-1
- New upstream release

* Wed Sep 16 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090916-1
- New upstream release
- Can optionaly bundle default font, to avoid liberation-fonts
  vs. liberation-sans-fonts issues when building snapshot RPMs

* Fri Jul 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.08.20090120-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_12_Mass_Rebuild

* Tue Feb 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.08.20090120-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_11_Mass_Rebuild

* Wed Feb  4 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090120-2
- Apply Fedora font rename: liberation-fonts -> liberation-sans-fonts

* Tue Jan 20 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090120-1
- New upstream release (fix engine freeze in some DinkC scripts)

* Fri Jan  9 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090109-2
- Bump version to fix build tag issue

* Fri Jan  9 2009 Sylvain Beucler <beuc@beuc.net> - 1.08.20090109-1
- New upstream release
- Declare .mo translation catalogs

* Sun Oct  5 2008 Sylvain Beucler <beuc@beuc.net> - 1.08.20080920-4
- Use liberation-fonts in all distro versions

* Wed Sep 24 2008 Sylvain Beucler <beuc@beuc.net> - 1.08.20080920-3
- Don't use 'update-desktop-database' for simple desktop files
- Fix unescaped macros in comments
- Use spaces around '=' in version-specific dependency

* Wed Sep 24 2008 Sylvain Beucler <beuc@beuc.net> - 1.08.20080920-2
- Fix variable s/fedora_version/fedora/
- Meta-package depends on same version of freedink-engine
- Use "install -p" to preserve timestamps
- Validate installed .desktop files

* Sat Sep 20 2008 Sylvain Beucler <beuc@beuc.net> - 1.08.20080920-1
- New upstream release

* Thu Aug 28 2008 Sylvain Beucler <beuc@beuc.net> - 1.08.20080828-1
- Initial package
