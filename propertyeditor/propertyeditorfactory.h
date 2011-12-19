#ifndef GAMMARAY_PROPERTYEDITORFACTORY_H
#define GAMMARAY_PROPERTYEDITORFACTORY_H

#include <qitemeditorfactory.h>

namespace GammaRay {

/** Item editor factory with support for extra types while keeping support for the built-in ones. */
class PropertyEditorFactory : public QItemEditorFactory
{
  public:
    PropertyEditorFactory();
    virtual QWidget* createEditor(QVariant::Type type, QWidget* parent) const;
};

}

#endif // GAMMARAY_PROPERTYEDITORFACTORY_H
