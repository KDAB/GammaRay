/*
  methodstab.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef METHODSTAB_H
#define METHODSTAB_H

#include <QWidget>

class QModelIndex;

namespace GammaRay {

class MethodsExtensionInterface;

class Ui_MethodsTab;
class PropertyWidget;

class MethodsTab : public QWidget
{
  Q_OBJECT
  public:
    explicit MethodsTab(PropertyWidget *parent);
    virtual ~MethodsTab();

  private:
    void setObjectBaseName(const QString &baseName);

  private slots:
    void methodActivated(const QModelIndex &index);
    void methodContextMenu(const QPoint &pos);

  private:
    Ui_MethodsTab *m_ui;
    MethodsExtensionInterface *m_interface;

    QString m_objectBaseName;
};

}

#endif // METHODSTAB_H
