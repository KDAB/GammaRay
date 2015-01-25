/*
  sggeometrytab.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_QUICKINSPECTOR_SGGEOMETRYTAB_H
#define GAMMARAY_QUICKINSPECTOR_SGGEOMETRYTAB_H

#include <QWidget>
#include <QModelIndex>

class QAbstractItemModel;

namespace GammaRay {

class SGGeometryExtensionInterface;

class Ui_SGGeometryTab;
class PropertyWidget;

class SGGeometryTab : public QWidget
{
  Q_OBJECT

  public:
    explicit SGGeometryTab(PropertyWidget *parent);
    virtual ~SGGeometryTab();

  private:
    void setObjectBaseName(const QString &baseName);

  private:
    Ui_SGGeometryTab *m_ui;
    SGGeometryExtensionInterface *m_interface;
    QAbstractItemModel *m_model;
};

}

#endif // SGGEOMETRYTAB_H
