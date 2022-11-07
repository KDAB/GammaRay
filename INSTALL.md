# Installing GammaRay

Please see the comments at the top of CMakeLists.txt for the available configuration options
you can pass to CMake.

## Requirements

To build GammaRay you will need *at least*:

- CMake 3.16.0
- a C++ compiler with C++11 support
- Qt 5.5 or higher, or Qt 6

Please be aware that GammaRay heavily relies on Qt private headers which can
be changed at will by The Qt Company between releases and may need to be
specially installed from your distribution. Before running CMake, make sure
your Qt installation provides private Qt headers.

i.e. when using distro provided Qt make sure you have (or the equivalent for Qt6):

- Redhat,Fedora: qt5-qtbase-private-devel
- Debian,Ubuntu: qtbase5-private-dev qtdeclarative5-private-dev
- SUSE: libqt5-qtbase-private-headers-devel libqt5-qtdeclarative-private-headers-devel

Optional FOSS packages (eg. KDSME, etc) provide extra functionality.
See the "Optional Dependencies" section below for more details.

## Building

Open a terminal capable of building Qt applications.
Make sure you have cmake, ninja, compiler, Qt, etc in PATH.

Adapt the instructions to suit your cmake generator and operating system.

```bash
    cmake -G Ninja -DCMAKE_INSTALL_PREFIX=/path/where/to/install ../path/to/gammaray
    cmake --build .
    cmake --build . --target install
```

Your system's installation of Qt will be used by default, which may not be the same as
the Qt returned by `qmake -v`.  To specify the Qt version to build against use the
CMake option `CMAKE_PREFIX_PATH` and point it to the folder of your installation:

For example:

```bash
    cmake -DCMAKE_PREFIX_PATH=$HOME/Qt/5.11.2/gcc_64 ..
```

or

```bash
    set CMAKE_PREFIX_PATH=c:\Qt\5.15.4\msvc2019_64
    cmake...
```

The installation directory defaults to `/usr/local` on UNIX `C:/Program Files` on Windows
and `/Applications` on MacOS.

Change the installation location by passing the option `-DCMAKE_INSTALL_PREFIX=/install/path` to CMake.

### Android

Build on Android:

```bash
    mkdir android-build
    cd android-build
    export ANDROID_NDK=/path/to/android-ndk
    cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
          -DCMAKE_FIND_ROOT_PATH=/android/qt5/install/path \
          -DCMAKE_INSTALL_PREFIX=/install/path ..
    make [-j CPU_NUMBER+2]
    make install
```

For more information about building CMake projects on Android see
<https://developer.android.com/ndk/guides/cmake>.

Using GammaRay on Android:

- add GammaRay probe to your android .pro file

```text
    myproject.pro
    ....
    android: QT += GammaRayProbe
    ...
```

- build & deploy and run your project
- forward GammaRay's socket

```bash
    adb forward tcp:11732 localfilesystem:/data/data/YOUR_ANDROID_PACKAGE_NAME(e.g. com.kdab.example)/files/+gammaray_socket
````

- run GammaRay GUI and connect to localhost:11732
- after you've finished, remove the forward:

```bash
    adb forward --remove tcp:11732
```

or

```bash
    adb forward --remove-all
```

... to remove all forwards

### Additional notes

To build a debug version pass `-DCMAKE_BUILD_TYPE=Debug` to cmake.

## Cross-compiling GammaRay

You'll find more information on this in the wiki:
<https://github.com/KDAB/GammaRay/wiki/Cross-compiling-GammaRay>

== Force a probe only build ==
If you already built GammaRay in the past and that you want to support more probes,
you don't need to rebuild entirely GammaRay for this specific Qt version.
You can instead just build the GammaRay probe for the new Qt version and install it
in you previous GammaRay installation.

You can make probe only builds that way:

```bash
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_PREFIX_PATH=/path/to/Qt/version/ \
        -DGAMMARAY_PROBE_ONLY_BUILD=true \
        -DGAMMARAY_BUILD_UI=false  \
        -DCMAKE_INSTALL_PREFIX=/path/to/your/previous/gammaray/prefix \
        /path/to/gammaray/sources
```

You can still use any cmake flags you want like `CMAKE_DISABLE_FIND_PACKAGE_<PACKAGE>` etc.

## Optional Dependencies

GammaRay relies on optional (FOSS) dependencies to help provide some of its
functionality, most prominently KDSME (<https://github.com/KDAB/KDStateMachineEditor>).

When you run cmake it will inform you about these missing dependencies.

You can also force CMake to ignore any or all of the optional dependencies
by passing the option `-DCMAKE_DISABLE_FIND_PACKAGE_<PACKAGE>=True`.

For instance:

```bash
    # tell cmake to ignore VTK
    cmake -DCMAKE_DISABLE_FIND_PACKAGE_VTK=True
```

## RPATH Settings (Linux only)

By default GammaRay will be build with RPATHs set to the absolute install location
of its dependencies (such as Qt). This is useful for easily using a self-built
GammaRay, but it might not be what you want when building installers or packages.

You can therefore change this via the following CMake options:

- `CMAKE_INSTALL_RPATH_USE_LINK_PATH=OFF` will disable settings RPATH to the location
  of dependencies. It will however still set relative RPATHs between the various
  GammaRay components. This is typically desired for Linux distros, where GammaRay's
  dependencies are all in the default search path anyway.

- `CMAKE_INSTALL_RPATH=<path(s)>` will add the specified absolute paths to RPATH,
  additionally to the relative RPATHs between GammaRay's components.
