Name:           gammaray
Version:        1.3.2
Release:        0
Summary:        An introspection tool for Qt applications
Source:         %{name}-%{version}.tar.gz
# included upstream > 1.3.2
Patch0:         gammaray-rpath-fix.patch
Url:            http://github.com/KDAB/GammaRay
Group:          Development/Tools/Debuggers
License:        GPL-2.0+
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%if %{defined suse_version}
BuildRequires:  libqt4-devel libQtWebKit-devel cmake graphviz-devel update-desktop-files libkde4-devel
Requires:       graphviz
%endif

%if %{defined fedora}
BuildRequires:  gcc-c++ libqt4-devel qtwebkit-devel cmake desktop-file-utils graphviz-devel kdelibs-devel
# for pod2man
%if 0%{?fedora} > 18
BuildRequires: perl-podlators
%endif
Requires:       graphviz
%endif

%description
GammaRay is a tool for examining the internals of a Qt application
and to some extent also manipulate it. GammaRay uses injection
methods to hook into an application at runtime and provide access to
a wide variety of interesting information. It provides easy ways of
navigating through the complex internal structures you find in some
Qt frameworks, such as QGraphicsView, model/view, QTextDocument,
state machines and more.

Authors:
--------
     The GammaRay Team <gammaray-interest@kdab.com>

%package kde4-plugins
Summary:        GammaRay plug-ins to introspect KDE4 applications
Group:          Development/Tools/Debuggers
Requires:       %{name} = %{version}

%description kde4-plugins
Plug-ins for the GammaRay introspection tool to debug KDE4 applications,
such as a KJob tracker.

%package devel
Summary:        Development files for %{name}
Group:          Development/Libraries/C and C++
Requires:       %{name} = %{version}

%description devel
The %{name}-devel package contains libraries and header files for
developing GammaRay plug-ins.

%prep
%setup -q
%patch0 -p1

%build
%if %{defined fedora}
%cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
%else
%if "%{_lib}"=="lib64"
cmake . -DLIB_SUFFIX=64 -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
%else
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
%endif
%endif
%__make %{?_smp_mflags}

%install
%make_install

%if %{defined suse_version}
%suse_update_desktop_file GammaRay Development Qt Debugger
%endif

%clean
%__rm -rf "%{buildroot}"

%files
%defattr(-,root,root)
%{_prefix}/share/applications/GammaRay.desktop
%{_prefix}/share/icons/hicolor
%{_prefix}/share/doc/gammaray
%{_mandir}/man1/gammaray.1.gz
%{_libdir}/gammaray_probe.so
%{_libdir}/libgammaray_widget_export_actions.so
%{_bindir}/gammaray
%dir %{_libdir}/qt4/plugins/gammaray/
%{_libdir}/qt4/plugins/gammaray/gammaray_actioninspector*
%{_libdir}/qt4/plugins/gammaray/gammaray_scriptenginedebugger*
%{_libdir}/qt4/plugins/gammaray/gammaray_statemachineviewer*
%{_libdir}/qt4/plugins/gammaray/gammaray_timertop*
%{_libdir}/qt4/plugins/gammaray/gammaray_webinspector*
%{_libdir}/qt4/plugins/styles/

%files kde4-plugins
%defattr(-,root,root)
%{_libdir}/qt4/plugins/gammaray/gammaray_kjob*

%files devel
%defattr(-,root,root)
%{_prefix}/include/gammaray

%changelog
* Thu Oct 02 2013 Allen Winter <allen.winter@kdab.com> 1.3.2
  Second 1.3 bugfix release
* Tue Apr 30 2013 Allen Winter <allen.winter@kdab.com> 1.3.1
  First 1.3 bugfix release
* Sun Jan 27 2013 Allen Winter <allen.winter@kdab.com> 1.3.0
  Update to latest release
* Tue Jun 27 2012 Allen Winter <allen.winter@kdab.com> 1.1.98
- Rename version to use all integers
* Tue Jun 26 2012 Allen Winter <allen.winter@kdab.com> 1.2beta
- Update to beta release
* Wed Jun 20 2012 Allen Winter <allen.winter@kdab.com> 1.2prebeta
- Update to prebeta release
* Tue Jan 10 2012 Kevin Ottens <kevin.ottens@kdab.com> 1.1.0
- Update to latest release
* Sun Jan  8 2012 Kevin Ottens <kevin.ottens@kdab.com> 1.0.98.20120108
- Grabbing a new snapshot
* Wed Dec 21 2011 Kevin Ottens <kevin.ottens@kdab.com> 1.0.1
- First packaging
