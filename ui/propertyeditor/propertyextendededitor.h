/*
  propertyextendededitor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTYEXTENDEDEDITOR_H
#define GAMMARAY_PROPERTYEXTENDEDEDITOR_H

#include <QVariant>
#include <QWidget>

#include <memory>

namespace GammaRay {
namespace Ui {
class PropertyExtendedEditor;
}

/** Base class for property editors that open a separate dialog. */
class PropertyExtendedEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)
public:
    explicit PropertyExtendedEditor(QWidget *parent = nullptr);
    ~PropertyExtendedEditor();

    QVariant value() const;
    void setValue(const QVariant &value);

    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

    virtual void showEditor(QWidget *parent) = 0;

Q_SIGNALS:
    void editorClosed();

protected:
    bool isInlineEditable() const;
    void setInlineEditable(bool editable);

    void save(const QVariant &value);

private slots:
    void slotEdit();

private:
    std::unique_ptr<Ui::PropertyExtendedEditor> ui;
    QVariant m_value;
    bool m_inlineEditable;
    bool m_readOnly;
};
}

#endif // GAMMARAY_PROPERTYEXTENDEDEDITOR_H
