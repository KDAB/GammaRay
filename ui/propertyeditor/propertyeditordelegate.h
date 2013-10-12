#ifndef GAMMARAY_PROPERTYEDITORDELEGATE_H
#define GAMMARAY_PROPERTYEDITORDELEGATE_H

#include <QStyledItemDelegate>

namespace GammaRay {

class PropertyEditorDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PropertyEditorDelegate(QObject *parent);
    ~PropertyEditorDelegate();

    /*override*/ void setEditorData(QWidget* editor, const QModelIndex& index) const;
};

}

#endif // GAMMARAY_PROPERTYEDITORDELEGATE_H
