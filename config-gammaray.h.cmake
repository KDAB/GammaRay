#include <qglobal.h>

// relative install dirs
#ifdef Q_OS_ANDROID
# define GAMMARAY_PLUGIN_INSTALL_DIR "lib"
#else
# define GAMMARAY_PLUGIN_INSTALL_DIR "${PLUGIN_INSTALL_DIR}"
#endif
#define GAMMARAY_LIBEXEC_INSTALL_DIR "${LIBEXEC_INSTALL_DIR}"
#define GAMMARAY_BIN_INSTALL_DIR "${BIN_INSTALL_DIR}"

#define GAMMARAY_PLUGIN_VERSION "${GAMMARAY_PLUGIN_VERSION}"
#define GAMMARAY_PROBE_ABI "${GAMMARAY_PROBE_ABI}"

// relative inverse install dirs
#define GAMMARAY_INVERSE_BIN_DIR "${GAMMARAY_INVERSE_BIN_DIR}"
#define GAMMARAY_INVERSE_PROBE_DIR "${GAMMARAY_INVERSE_PROBE_DIR}"
#define GAMMARAY_INVERSE_LIBEXEC_DIR "${GAMMARAY_INVERSE_LIBEXEC_DIR}"

// probe name
#ifdef Q_OS_ANDROID
#define GAMMARAY_PROBE_NAME "libgammaray_probe"
#else
#define GAMMARAY_PROBE_NAME "gammaray_probe"
#endif

// build options
#cmakedefine BUILD_TIMER_PLUGIN

#cmakedefine HAVE_PRIVATE_QT_HEADERS

#cmakedefine HAVE_STDINT_H

#cmakedefine HAVE_QT_WIDGETS
#cmakedefine HAVE_QT_SVG
#cmakedefine HAVE_QT_DESIGNER
#cmakedefine HAVE_QT_PRINTSUPPORT
#cmakedefine HAVE_QT_WEBKIT1

#cmakedefine HAVE_VTK
#cmakedefine HAVE_GRAPHVIZ
#cmakedefine HAVE_ELF_H

#if !defined(QT_NO_SHAREDMEMORY) && !defined(QT_NO_SYSTEMSEMAPHORE) && !defined(Q_OS_ANDROID)
#define HAVE_SHM
#endif

#cmakedefine GAMMARAY_UNKNOWN_CXX_MANGLED_NAMES
