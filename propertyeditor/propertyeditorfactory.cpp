#include "propertyeditorfactory.h"
#include "propertyintpaireditor.h"

#include <QItemEditorFactory>

using namespace GammaRay;

PropertyEditorFactory::PropertyEditorFactory()
{
  registerEditor(QVariant::Point, new QStandardItemEditorCreator<PropertyPointEditor>());
  registerEditor(QVariant::Size, new QStandardItemEditorCreator<PropertySizeEditor>());
}

QWidget* PropertyEditorFactory::createEditor(QVariant::Type type, QWidget* parent) const
{
  QWidget *w = QItemEditorFactory::createEditor(type, parent);
  if (!w)
    return 0;

  // the read-only view is still in the background usually, so transpareny is not a good choice here
  w->setAutoFillBackground(true);
  return w;
}
