Name:           gammaray
Version:        2.3.0
Release:        1
Summary:        An introspection tool for Qt applications
Source:         %{name}-%{version}.tar.gz
Url:            http://github.com/KDAB/GammaRay
Group:          Development/Tools
License:        GPL-2.0+
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Vendor:         Klaralvdalens Datakonsult AB (KDAB)
Packager:       Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

BuildRequires: cmake
BuildRequires: libqt5-qtdeclarative-private-headers-devel libQt5Concurrent-devel libqt5-qttools-devel libqt5-qtsvg-devel libQt5PrintSupport-devel libqt5-qtscript-devel libQt5WebKitWidgets-devel
# TODO: this seems only to be in the update repo?
# libqt5-qtconnectivity-devel
BuildRequires: kdstatemachineeditor-devel
BuildRequires: kcoreaddons-devel

%if %{defined suse_version}
BuildRequires: update-desktop-files
%endif

%if %{defined fedora}
BuildRequires: desktop-file-utils
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

%package kf5-plugins
Summary:        GammaRay plug-ins to introspect KF5 applications
Group:          Development/Tools
Requires:       %{name} = %{version}

%description kf5-plugins
Plug-ins for the GammaRay introspection tool to debug KF5 applications,
such as a KJob tracker.

%package devel
Summary:        Development files for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}

%description devel
The %{name}-devel package contains libraries and header files for
developing GammaRay plug-ins.

%prep
%setup -q

%build
%if %{defined fedora}
%cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
%else
%if "%{_lib}"=="lib64"
cmake . -DLIB_SUFFIX=64 -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
%else
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
%endif
%endif
%__make %{?_smp_mflags}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

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
%dir %{_prefix}/share/appdata/
%{_prefix}/share/appdata/GammaRay.appdata.xml
%{_prefix}/share/icons/hicolor
%{_prefix}/share/doc/gammaray
%{_mandir}/man1/gammaray.1.gz
%{_bindir}/gammaray
%dir %{_libdir}/gammaray/libexec/
%{_libdir}/gammaray/libexec/gammaray-client
%{_libdir}/gammaray/libexec/gammaray-launcher
%{_libdir}/libgammaray_common-*.so.*
%{_libdir}/libgammaray_core-*.so.*
%{_libdir}/libgammaray_ui-*.so.*
%{_libdir}/libgammaray_client.so.*
%dir %{_libdir}/gammaray/
%dir %{_libdir}/gammaray/*/
%dir %{_libdir}/gammaray/*/*/
%{_libdir}/gammaray/*/*/libgammaray_widget_export_actions.so
%{_libdir}/gammaray/*/*/gammaray_probe.so
%{_libdir}/gammaray/*/*/gammaray_inprocessui.so
%{_libdir}/gammaray/*/*/gammaray_actioninspector*
%{_libdir}/gammaray/*/*/gammaray_codecbrowser*
%{_libdir}/gammaray/*/*/gammaray_fontbrowser*
%{_libdir}/gammaray/*/*/gammaray_selectionmodelinspector*
%{_libdir}/gammaray/*/*/gammaray_signalmonitor*
%{_libdir}/gammaray/*/*/gammaray_statemachineviewer*
%{_libdir}/gammaray/*/*/gammaray_timertop*
%{_libdir}/gammaray/*/*/gammaray_widgetinspector*
%{_libdir}/gammaray/*/*/gammaray_sceneinspector*
%{_libdir}/gammaray/*/*/gammaray_styleinspector*
%{_libdir}/gammaray/*/*/gammaray_scriptenginedebugger*
%{_libdir}/gammaray/*/*/gammaray_webinspector*
%{_libdir}/gammaray/*/*/gammaray_objectvisualizer*
%{_libdir}/gammaray/*/*/gammaray_qmlsupport*
%{_libdir}/gammaray/*/*/gammaray_quickinspector*
%{_libdir}/gammaray/*/*/gammaray_translatorinspector*
%{_libdir}/gammaray/*/*/styles/

%files kf5-plugins
%defattr(-,root,root)
%{_libdir}/gammaray/*/*/gammaray_kjobtracker*


%files devel
%defattr(-,root,root)
%{_includedir}/gammaray
%{_libdir}/libgammaray_common-*.so
%{_libdir}/libgammaray_core-*.so
%{_libdir}/libgammaray_ui-*.so
%{_libdir}/libgammaray_client.so
%{_libdir}/cmake/GammaRay/
%{_libdir}/qt5/mkspecs/modules/*

%changelog
* Fri Jul 10 2015 Allen Winter <allen.winter@kdab.com> 2.3.0
  2.3.0 final
* Tue Jan 27 2015 Allen Winter <allen.winter@kdab.com> 2.2.1
  2.2.1 final
* Fri Oct 31 2014 Allen Winter <allen.winter@kdab.com> 2.2.0
  2.2.0 final
* Thu Oct 23 2014 Allen Winter <allen.winter@kdab.com> 2.1.2
  2.1.2 final
* Sat Aug 30 2014 Allen Winter <allen.winter@kdab.com> 2.1.1
  2.1.1 final
* Fri Jun 27 2014 Allen Winter <allen.winter@kdab.com> 2.1.0
  2.1.0 final
* Thu Jun 26 2014 Allen Winter <allen.winter@kdab.com> 2.0.98
  2.1.0 release candidate 1
* Fri Apr 18 2014 Allen Winter <allen.winter@kdab.com> 2.0.2
  2.0.2 final
* Fri Feb 28 2014 Allen Winter <allen.winter@kdab.com> 2.0.1
  2.0.1 final
* Fri Jan 17 2014 Allen Winter <allen.winter@kdab.com> 2.0.0
  2.0.0 final
* Sat Jan 11 2014 Allen Winter <allen.winter@kdab.com> 1.9.96
  Second 2.0 beta release
* Fri Dec 20 2013 Allen Winter <allen.winter@kdab.com> 1.9.95
  First 2.0 beta release
* Thu Oct 03 2013 Allen Winter <allen.winter@kdab.com> 1.3.2
  Second 1.3 bugfix release
* Tue Apr 30 2013 Allen Winter <allen.winter@kdab.com> 1.3.1
  First 1.3 bugfix release
* Sun Jan 27 2013 Allen Winter <allen.winter@kdab.com> 1.3.0
  Update to latest release
* Wed Jun 27 2012 Allen Winter <allen.winter@kdab.com> 1.1.98
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
