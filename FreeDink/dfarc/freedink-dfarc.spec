Name:		freedink-dfarc
Version:	3.14
Release:	1%{?dist}
Summary:	Frontend and .dmod installer for GNU FreeDink

Group:		Amusements/Games
License:	GPLv3+
URL:		http://www.gnu.org/software/freedink/
Source0:	ftp://ftp.gnu.org/gnu/freedink/dfarc-%{version}.tar.gz

%if 0%{?suse_version}
BuildRequires:	bzip2, wxWidgets-devel >= 3, intltool, gettext, gcc-c++
BuildRequires:	desktop-file-utils, update-desktop-files
%else
BuildRequires:	bzip2-devel, wxGTK3-devel, intltool, desktop-file-utils
%endif
Requires:	xdg-utils

%description
DFArc makes it easy to play and manage the Dink Smallwood game and
it's numerous Dink Modules (or D-Mods).

%prep
%setup -q -n dfarc-%{version}


%build
# Don't install desktop files, use %%post instead
%configure --disable-desktopfiles \
  --with-wx-config=/usr/libexec/wxGTK3/wx-config
make %{?_smp_mflags}


%install
make install DESTDIR=%{buildroot} INSTALL="install -p"
%find_lang dfarc
desktop-file-validate %{buildroot}/%{_datadir}/applications/%name.desktop
%if 0%{?suse_version}
%suse_update_desktop_file -i %name
%endif


%files -f dfarc.lang
%defattr(-,root,root,-)
%doc AUTHORS COPYING NEWS README THANKS TODO TRANSLATIONS.txt ChangeLog
%{_bindir}/*
%{_datadir}/applications/*
%{_datadir}/mime/packages/*
%{_datadir}/pixmaps/*
# Don't include system directories, only added files:
%{_datadir}/icons/hicolor/32x32/mimetypes/*
%{_mandir}/man1/*


%changelog
* Tue Jun 05 2018 Fedora Release Engineering <releng@fedoraproject.org> - 3.14-1
- New upstream release
- Fix directory traversal in D-Mod extractor (CVE-2018-0496) (#1584843)
  (no answer from security team)

* Wed Feb 07 2018 Fedora Release Engineering <releng@fedoraproject.org> - 3.12-9
- Rebuilt for https://fedoraproject.org/wiki/Fedora_28_Mass_Rebuild

* Thu Jan 11 2018 Igor Gnatenko <ignatenkobrain@fedoraproject.org> - 3.12-8
- Remove obsolete scriptlets

* Wed Aug 02 2017 Fedora Release Engineering <releng@fedoraproject.org> - 3.12-7
- Rebuilt for https://fedoraproject.org/wiki/Fedora_27_Binutils_Mass_Rebuild

* Wed Jul 26 2017 Fedora Release Engineering <releng@fedoraproject.org> - 3.12-6
- Rebuilt for https://fedoraproject.org/wiki/Fedora_27_Mass_Rebuild

* Fri Feb 10 2017 Fedora Release Engineering <releng@fedoraproject.org> - 3.12-5
- Rebuilt for https://fedoraproject.org/wiki/Fedora_26_Mass_Rebuild

* Wed Feb 03 2016 Fedora Release Engineering <releng@fedoraproject.org> - 3.12-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_24_Mass_Rebuild

* Wed Jun 17 2015 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.12-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_23_Mass_Rebuild

* Sat May 02 2015 Kalev Lember <kalevlember@gmail.com> - 3.12-2
- Rebuilt for GCC 5 C++11 ABI change

* Thu Oct 16 2014 Sylvain Beucler <beuc@beuc.net> 3.12-1
- New upstream release
- Now requires wxWidgets >= 3.0
- Remove old %%clean section and use %%{buildroot} consistently

* Sun Oct 05 2014 Sylvain Beucler <beuc@beuc.net> 3.10-8
- tidy post-install scriptlets

* Thu Oct 02 2014 Rex Dieter <rdieter@fedoraproject.org> 3.10-7
- update icon/mime scriptlets

* Sat Aug 16 2014 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.10-6
- Rebuilt for https://fedoraproject.org/wiki/Fedora_21_22_Mass_Rebuild

* Sat Jun 07 2014 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.10-5
- Rebuilt for https://fedoraproject.org/wiki/Fedora_21_Mass_Rebuild

* Sat Aug 03 2013 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.10-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_20_Mass_Rebuild

* Wed Feb 13 2013 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.10-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_19_Mass_Rebuild

* Thu Jul 19 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.10-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_18_Mass_Rebuild

* Fri Apr 27 2012 Sylvain Beucler <beuc@beuc.net> - 3.10-1
- New upstream release

* Fri Jan 13 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.8.1-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_17_Mass_Rebuild

* Tue Feb 08 2011 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.8.1-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_15_Mass_Rebuild

* Sun Nov 14 2010 Sylvain Beucler <beuc@beuc.net> - 3.8.1-1
- New upstream release

* Thu Oct 28 2010 Sylvain Beucler <beuc@beuc.net> - 3.8-1
- New upstream release

* Wed Jul 14 2010 Dan Horák <dan@danny.cz> - 3.6-2
- rebuilt against wxGTK-2.8.11-2

* Sun Feb 21 2010 Sylvain Beucler <beuc@beuc.net> - 3.6-1
- New upstream release

* Fri Sep 18 2009 Sylvain Beucler <beuc@beuc.net> - 3.4-1
- New upstream release

* Wed Sep 16 2009 Sylvain Beucler <beuc@beuc.net> - 3.2.4-1
- New upstream release

* Fri Jul 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.2.3-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_12_Mass_Rebuild

* Fri Jul 10 2009 Sylvain Beucler <beuc@beuc.net> - 3.2.3-1
- New upstream release

* Wed Jun 03 2009 Sylvain Beucler <beuc@beuc.net> - 3.2.2-1
- New upstream release

* Tue Feb 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 3.2.1-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_11_Mass_Rebuild

* Wed Sep 24 2008 Sylvain Beucler <beuc@beuc.net> - 3.2.1-2
- Fix update-mime-database call (was conditional due to typo)
- Fix macros in comments
- Tidy changelog

* Tue Sep 23 2008 Sylvain Beucler <beuc@beuc.net> - 3.2.1-1
- New upstream release
- Fix source URI
- Clarify wxGlade upstream developer-only dependency
- Use 'install -p' to preserve timestamps
- Validate desktop files> * Desktop file
- Rebuild MIME cache after installing desktop files
- Add ChangeLog to the docs
- Don't own _datadir/icons/hicolor directories

* Sat Sep 20 2008 Sylvain Beucler <beuc@beuc.net> - 3.2-1
- New upstream release

* Thu Aug 28 2008 Sylvain Beucler <beuc@beuc.net> 3.0-1
- Initial package
