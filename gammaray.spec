Name:           gammaray
Version:        1.2.0
Release:        1.2
Summary:        A tool to poke around in a Qt-application
Source:         %{name}-%{version}.tar.gz
Url:            git@github.com:KDAB/GammaRay.git
Group:          Development/Tools/Debuggers
License:        GPL v2, or any later version
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

#VTK devel is only available on some newer openSUSE and Fedora distros
%if %{defined suse_version}
%if %{suse_version} <= 1210
BuildRequires:  libqt4-devel libQtWebKit-devel cmake graphviz-devel update-desktop-files
%else
BuildRequires:  libqt4-devel libQtWebKit-devel cmake graphviz-devel update-desktop-files vtk-devel
%endif
Requires:       graphviz
%endif

%if %{defined fedora}
%if %{fedora} < 17
BuildRequires:  gcc-c++ libqt4-devel qtwebkit-devel cmake desktop-file-utils graphviz-devel
%else
BuildRequires:  gcc-c++ libqt4-devel qtwebkit-devel cmake desktop-file-utils graphviz-devel vtk-devel
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

%prep
%setup -q

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
%{_libdir}/qt4/plugins/gammaray
%{_libdir}/qt4/plugins/styles
%{_libdir}/qt4/plugins/styles/gammaray_injector_style.so
%{_prefix}/include/gammaray

%changelog
* Tue Jul 05 2012 Allen Winter <allen.winter@kdab.com> 1.2.0
- Update to 1.2.0 release
* Tue Jul 03 2012 Allen Winter <allen.winter@kdab.com> 1.1.99
- Update to beta2 release
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
