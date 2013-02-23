#ifndef GAMMARAY_ENUMS_H
#define GAMMARAY_ENUMS_H

#include <QMetaType>

namespace GammaRay {

/** Display mode settings of the property widget. */
namespace PropertyWidgetDisplayState {
  enum State {
    QObject, /// full QObject instance
    Object, /// non-QObject instance
    MetaObject /// QMetaObject instance only
  };
}

}

Q_DECLARE_METATYPE(GammaRay::PropertyWidgetDisplayState::State)

#endif
