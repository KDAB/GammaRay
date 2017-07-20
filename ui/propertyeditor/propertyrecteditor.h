#ifndef PROPERTYRECTEDITOR_H
#define PROPERTYRECTEDITOR_H

#include "propertyeditor/propertyextendededitor.h"

#include <QDialog>

namespace GammaRay {
namespace Ui {
class PropertyRectEditorDialog;
}


class PropertyRectEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PropertyRectEditorDialog(const QRectF &rect, QWidget *parent = nullptr);
    explicit PropertyRectEditorDialog(const QRect &rect, QWidget *parent = nullptr);
    ~PropertyRectEditorDialog();

    QRectF rectF() const;

private:
    Ui::PropertyRectEditorDialog *ui;
};

class PropertyRectEditor : public PropertyExtendedEditor
{
    Q_OBJECT
public:
    explicit PropertyRectEditor(QWidget *parent = nullptr);

protected slots:
    void edit() override;
};

class PropertyRectFEditor : public PropertyExtendedEditor
{
    Q_OBJECT
public:
    explicit PropertyRectFEditor(QWidget *parent = nullptr);

protected slots:
    void edit() override;
};
}

#endif // PROPERTYRECTEDITOR_H
