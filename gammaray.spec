Name:           gammaray
Version:        2.11.2
Release:        1
Summary:        An introspection tool for Qt applications
Source0:        %{name}-%{version}.tar.gz
Source1:        %{name}-%{version}.tar.gz.asc
Url:            https://github.com/KDAB/GammaRay
Group:          Development/Tools
License:        GPL-2.0-or-later
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Vendor:         Klaralvdalens Datakonsult AB (KDAB)
Packager:       Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

BuildRequires: cmake kdstatemachineeditor-devel
%if %{defined suse_version}
BuildRequires: libqt5-qtbase-private-headers-devel libqt5-qtdeclarative-private-headers-devel libQt5Concurrent-devel libqt5-qttools-devel libqt5-qtsvg-devel libQt5PrintSupport-devel libqt5-qtscript-devel libQt5WebKitWidgets-devel libqt5-qtconnectivity-devel libqt5-qt3d-devel wayland-devel libqt5-qtwayland-devel bluez-qt-devel
# TODO: this seems only to be in the update repo?
BuildRequires: kcoreaddons-devel
%endif

%if %{defined fedora}
BuildRequires: qt5-qtdeclarative-devel qt5-qtconnectivity-devel qt5-qttools-devel qt5-qtsvg-devel qt5-qtscript-devel qt5-qtwebkit-devel
%if 0%{?fedora} >= 21
BuildRequires:  kf5-kcoreaddons-devel
%endif
%if 0%{?fedora} > 23
BuildRequires:  ghostscript-core
%endif
%if 0%{?fedora} > 24
BuildRequires:  qt5-qt3d-devel wayland-devel qt5-qtwayland-devel kf5-syndication-devel
%endif
%if 0%{?fedora} > 30
BuildRequires:  qt5-qtbase-private-devel
%endif
%endif

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
     Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

%define debug_package %{nil}
%global __debug_install_post %{nil}

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
%autosetup

%build
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
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
%dir %{_prefix}/share/metainfo/
%{_prefix}/share/metainfo/com.kdab.GammaRay.metainfo.xml
%{_prefix}/share/icons/hicolor
%{_prefix}/share/doc/gammaray
%dir %{_prefix}/share/gammaray/
%dir %{_prefix}/share/gammaray/translations/
%{_prefix}/share/gammaray/translations/gammaray_*.qm
%{_mandir}/man1/gammaray.1.gz
%{_bindir}/gammaray
%{_libdir}/gammaray/libexec/gammaray-client
%{_libdir}/gammaray/libexec/gammaray-launcher
%{_libdir}/libgammaray_common-*.so.*
%{_libdir}/libgammaray_core-*.so.*
%{_libdir}/libgammaray_ui-*.so.*
%{_libdir}/libgammaray_client.so.*
%{_libdir}/libgammaray_kitemmodels*
%{_libdir}/libgammaray_kuserfeedback*
%{_libdir}/libgammaray_launcher*
%dir %{_libdir}/gammaray/
%dir %{_libdir}/gammaray/*/
%dir %{_libdir}/gammaray/*/*/
%{_libdir}/gammaray/*/*/libgammaray_widget_export_actions.so
%{_libdir}/gammaray/*/*/gammaray_probe.so
%{_libdir}/gammaray/*/*/gammaray_inprocessui.so
%{_libdir}/gammaray/*/*/gammaray_actioninspector*
%{_libdir}/gammaray/*/*/gammaray_bluetooth*
%{_libdir}/gammaray/*/*/gammaray_codecbrowser*
%{_libdir}/gammaray/*/*/gammaray_eventmonitor*
%{_libdir}/gammaray/*/*/gammaray_fontbrowser*
%{_libdir}/gammaray/*/*/gammaray_guisupport*
%{_libdir}/gammaray/*/*/gammaray_localeinspector*
%{_libdir}/gammaray/*/*/gammaray_mimetypes*
%{_libdir}/gammaray/*/*/gammaray_modelinspector*
%{_libdir}/gammaray/*/*/gammaray_network*
%{_libdir}/gammaray/*/*/gammaray_objectvisualizer*
%{_libdir}/gammaray/*/*/gammaray_qmlsupport*
%{_libdir}/gammaray/*/*/gammaray_qtivi_ui*
%{_libdir}/gammaray/*/*/gammaray_quickinspector*
%{_libdir}/gammaray/*/*/gammaray_quickwidgetsupport*
%{_libdir}/gammaray/*/*/gammaray_sceneinspector*
%{_libdir}/gammaray/*/*/gammaray_scriptenginedebugger*
%{_libdir}/gammaray/*/*/gammaray_signalmonitor*
%{_libdir}/gammaray/*/*/gammaray_statemachineviewer*
%{_libdir}/gammaray/*/*/gammaray_styleinspector*
%{_libdir}/gammaray/*/*/gammaray_sysinfo*
%{_libdir}/gammaray/*/*/gammaray_textdocumentinspector*
%{_libdir}/gammaray/*/*/gammaray_timertop*
%{_libdir}/gammaray/*/*/gammaray_translatorinspector*
%{_libdir}/gammaray/*/*/gammaray_webinspector*
%{_libdir}/gammaray/*/*/gammaray_widgetinspector*
%{_libdir}/gammaray/*/*/gammaray_wlcompositorinspector*
%{_libdir}/gammaray/*/*/styles/
%if 0%{?fedora} > 24
%{_libdir}/gammaray/*/*/gammaray_3dinspector*
%endif
%if 0%{?suse_version} > 1320
%{_libdir}/gammaray/*/*/gammaray_3dinspector*
%endif

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
%dir %{_prefix}/mkspecs/
%dir %{_prefix}/mkspecs/modules/
%{_prefix}/mkspecs/modules/*.pri

%changelog
* Fri Sep 18 2020 Allen Winter <allen.winter@kdab.com> 2.11.2
  2.11.2 patchlevel release
* Tue Mar 03 2020 Allen Winter <allen.winter@kdab.com> 2.11.1
  2.11.1 patchlevel release
* Wed Jul 03 2019 Allen Winter <allen.winter@kdab.com> 2.11.0
  2.11.0 final
* Sun Dec 16 2018 Allen Winter <allen.winter@kdab.com> 2.10.0
  2.10.0 final
* Wed Aug 01 2018 Allen Winter <allen.winter@kdab.com> 2.9.1
  2.9.1 final
* Wed Feb 07 2018 Allen Winter <allen.winter@kdab.com> 2.9.0
  2.9.0 final
* Tue Sep 05 2017 Allen Winter <allen.winter@kdab.com> 2.8.1
  2.8.1 bugfix release
* Wed Jun 07 2017 Allen Winter <allen.winter@kdab.com> 2.8.0
  2.8.0 final
* Tue Feb 14 2017 Allen Winter <allen.winter@kdab.com> 2.7.0
  2.7.0 final
* Thu Sep 29 2016 Allen Winter <allen.winter@kdab.com> 2.6.0
  2.6.0 final
* Wed Jul 27 2016 Allen Winter <allen.winter@kdab.com> 2.5.1
  2.5.1 bugfix release
* Fri Jul 08 2016 Allen Winter <allen.winter@kdab.com> 2.5.0
  2.5.0 final
* Fri Mar 11 2016 Allen Winter <allen.winter@kdab.com> 2.4.1
  2.4.1 final
* Tue Dec 22 2015 Allen Winter <allen.winter@kdab.com> 2.4.0
  2.4.0 final
* Sun Dec 20 2015 Allen Winter <allen.winter@kdab.com> 2.3.99
  2.4.0 release candidate 1
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
