#include "propertyeditordelegate.h"
#include "propertyeditorfactory.h"

using namespace GammaRay;

PropertyEditorDelegate::PropertyEditorDelegate(QObject* parent): QStyledItemDelegate(parent)
{
   setItemEditorFactory(PropertyEditorFactory::instance());
}

PropertyEditorDelegate::~PropertyEditorDelegate()
{
}

void PropertyEditorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  editor->setProperty("displayString", index.data(Qt::DisplayRole));
  QStyledItemDelegate::setEditorData(editor, index);
}

#include "propertyeditordelegate.moc"
