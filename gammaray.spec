Name:           gammaray
Version:        1.9.95
Release:        2
Summary:        An introspection tool for Qt applications
Source:         %{name}-%{version}.tar.gz
Url:            http://github.com/KDAB/GammaRay
Group:          Development/Tools/Debuggers
License:        GPL-2.0+
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%define rel 1.9

%if %{defined suse_version}
BuildRequires:  libqt4-devel libQtWebKit-devel cmake graphviz-devel update-desktop-files libkde4-devel
%if 0%{?suse_version} >= 1220
BuildRequires:  vtk-devel
%endif
# missing dependency for VTK in openSUSE Factory
%if 0%{?suse_version} > 1230
BuildRequires:  python-devel
%endif
Requires:       graphviz
%endif

%if %{defined fedora}
BuildRequires:  gcc-c++ qt-devel qtwebkit-devel cmake desktop-file-utils graphviz-devel kdelibs-devel
%if 0%{?fedora} >= 18
BuildRequires:  vtk-devel
%endif
# dependency ambiguity for vtk-java needed by vtk-devel in Fedora 19
%if 0%{?fedora} == 19
BuildRequires:  java-1.8.0-openjdk
%endif
# for pod2man
%if 0%{?fedora} >= 19
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

%if 0%{?suse_version} >= 1220 || 0%{?fedora} >= 18
%package vtk-plugins
Summary:        GammaRay visualization plug-ins using VTK
Group:          Development/Tools/Debuggers
Requires:       %{name} = %{version}

%description vtk-plugins
Visualization plug-ins for the GammaRay introspection tool that depend on VTK.
%endif

%package devel
Summary:        Development files for %{name}
Group:          Development/Libraries/C and C++
Requires:       %{name} = %{version}

%description devel
The %{name}-devel package contains libraries and header files for
developing GammaRay plug-ins.

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
%{_libdir}/gammaray_inprocessui.so
%{_libdir}/gammaray_probe.so
%{_libdir}/libgammaray_widget_export_actions.so
%{_libdir}/libgammaray_common.so*
%{_libdir}/libgammaray_core.so*
%{_libdir}/libgammaray_ui.so*
%{_bindir}/gammaray
%{_bindir}/gammaray-launcher
%{_bindir}/gammaray-client
%{_libdir}/qt4/plugins/gammaray/
%{_libdir}/qt4/plugins/gammaray/%{rel}/
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_codecbrowser_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_codecbrowser.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_codecbrowser_ui_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_codecbrowser_ui.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_fontbrowser_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_fontbrowser.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_fontbrowser_ui_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_fontbrowser_ui.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_selectionmodelinspector_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_selectionmodelinspector.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_selectionmodelinspector_ui_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_selectionmodelinspector_ui.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_statemachineviewer_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_statemachineviewer.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_statemachineviewer_ui_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_statemachineviewer_ui.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_timertop_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_timertop.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_timertop_ui_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_timertop_ui.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_actioninspector_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_actioninspector.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_widgetinspector_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_widgetinspector.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_sceneinspector_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_sceneinspector.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_styleinspector_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_styleinspector.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_scriptenginedebugger_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_scriptenginedebugger.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_webinspector_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_webinspector.desktop
%{_libdir}/qt4/plugins/styles/

%files kde4-plugins
%defattr(-,root,root)
%exclude %{_libdir}/qt4/plugins/gammaray/
%exclude %{_libdir}/qt4/plugins/gammaray/%{rel}/
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_kjobtracker_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_kjobtracker.desktop

%if 0%{?suse_version} >= 1220 || 0%{?fedora} >= 18
%files vtk-plugins
%defattr(-,root,root)
%exclude %{_libdir}/qt4/plugins/gammaray/
%exclude %{_libdir}/qt4/plugins/gammaray/%{rel}/
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_objectvisualizer_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_objectvisualizer.desktop
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_objectvisualizer_ui_plugin.so
%{_libdir}/qt4/plugins/gammaray/%{rel}/gammaray_objectvisualizer_ui.desktop
%endif

%files devel
%defattr(-,root,root)
%{_includedir}/gammaray

%changelog
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
