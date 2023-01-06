/*
  propertyrecteditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Hannah von Reth <hannah.vonreth@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

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
    ~PropertyRectEditorDialog() override;

    QRectF rectF() const;

private:
    Ui::PropertyRectEditorDialog *ui;
};

class PropertyRectEditor : public PropertyExtendedEditor
{
    Q_OBJECT
public:
    explicit PropertyRectEditor(QWidget *parent = nullptr);
    void showEditor(QWidget *parent) override;
};

class PropertyRectFEditor : public PropertyExtendedEditor
{
    Q_OBJECT
public:
    explicit PropertyRectFEditor(QWidget *parent = nullptr);
    void showEditor(QWidget *parent) override;
};
}

#endif // PROPERTYRECTEDITOR_H
