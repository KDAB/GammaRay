/*
  propertytexteditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTYTEXTEDITOR_H
#define GAMMARAY_PROPERTYTEXTEDITOR_H

#include <ui/propertyeditor/propertyextendededitor.h>

#include <QDialog>
#include <QLineEdit>

namespace GammaRay {
namespace Ui {
class PropertyTextEditorDialog;
}

/** Property editor dialog for texts. */
class PropertyTextEditorDialog : public QDialog
{
    Q_OBJECT
public:
    enum Mode
    {
        NoMode,
        StringMode,
        HexMode
    };

    explicit PropertyTextEditorDialog(const QString &text, QWidget *parent = nullptr);
    explicit PropertyTextEditorDialog(const QByteArray &bytes, QWidget *parent = nullptr);
    ~PropertyTextEditorDialog() override;

    void setReadOnly(bool readOnly);

    QString text() const;
    QByteArray bytes() const;

    void setMode(Mode mode);

public Q_SLOTS:
    void toggleMode();

protected:
    QScopedPointer<Ui::PropertyTextEditorDialog> ui;
    const QByteArray m_bytes;
    Mode m_mode;
};

/** Property editor for texts. */
class PropertyTextEditor : public PropertyExtendedEditor
{
    Q_OBJECT
public:
    explicit PropertyTextEditor(QWidget *parent = nullptr);
    void showEditor(QWidget *parent) override;
};

class PropertyByteArrayEditor : public PropertyExtendedEditor
{
    Q_OBJECT
public:
    explicit PropertyByteArrayEditor(QWidget *parent = nullptr);
    void showEditor(QWidget *parent) override;
};
}

#endif // GAMMARAY_PROPERTYTEXTEDITOR_H
