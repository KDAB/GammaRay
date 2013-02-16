#ifndef GAMMARAY_MODELROLES_H
#define GAMMARAY_MODELROLES_H

#include <QAbstractItemModel>

/**
 * @file modelroles.h A collection of custom model roles shared between client and server.
 */
namespace GammaRay {

static const int UserRole = 256; // Qt4 uses 32, Qt5 256, use the latter globaly to allow combining Qt4/5 client/servers.

/** Custom roles for GammaRay::ToolModel. */
namespace ToolModelRole {
  enum Role {
    ToolFactory = UserRole + 1,
    ToolWidget,
    ToolId,
    ToolWidgetParent
  };
}

/** Custom roles for GammaRay::ObjectMethodModel. */
namespace ObjectMethodModelRole {
  enum Role {
    MetaMethod = UserRole + 1,
    MetaMethodType
  };
}

}

#endif
