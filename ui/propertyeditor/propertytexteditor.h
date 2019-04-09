/*
  propertytexteditor.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    enum Mode {
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
    void showEditor(QWidget * parent) override;
};

class PropertyByteArrayEditor : public PropertyExtendedEditor
{
    Q_OBJECT
public:
    explicit PropertyByteArrayEditor(QWidget *parent = nullptr);
    void showEditor(QWidget * parent) override;
};
}

#endif // GAMMARAY_PROPERTYTEXTEDITOR_H
