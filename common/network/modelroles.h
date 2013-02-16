#ifndef GAMMARAY_MODELROLES_H
#define GAMMARAY_MODELROLES_H

#include <QAbstractItemModel>

/**
 * @file modelroles.h A collection of custom model roles shared between client and server.
 */
namespace GammaRay {

/** Custom roles for GammaRay::ToolModel. */
namespace ToolModelRole {
  enum Role {
    ToolFactory = Qt::UserRole + 1,
    ToolWidget,
    ToolId,
    ToolWidgetParent
  };
}

/** Custom roles for GammaRay::ObjectMethodModel. */
namespace ObjectMethodModelRole {
  enum Role {
    MetaMethod = Qt::UserRole + 1,
    MetaMethodType
  };
}

}

#endif
