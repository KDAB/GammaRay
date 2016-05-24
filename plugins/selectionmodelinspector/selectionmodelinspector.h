/*
  selectionmodelinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#ifndef GAMMARAY_SELECTIONMODELINSPECTOR_SELECTIONMODELINSPECTOR_H
#define GAMMARAY_SELECTIONMODELINSPECTOR_SELECTIONMODELINSPECTOR_H

#include <core/toolfactory.h>

#include <QItemSelectionModel>

QT_BEGIN_NAMESPACE
class QAbstractProxyModel;
QT_END_NAMESPACE

namespace GammaRay {

class SelectionModelInspector : public QObject
{
  Q_OBJECT
  public:
    explicit SelectionModelInspector(ProbeInterface *probe, QObject *parent = 0);

  private slots:
    void currentChanged(const QModelIndex &current);

  private:
    QAbstractProxyModel *m_current;
};

class SelectionModelInspectorFactory :
    public QObject, public StandardToolFactory<QItemSelectionModel, SelectionModelInspector>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_selectionmodelinspector.json")
  public:
    explicit SelectionModelInspectorFactory(QObject *parent = 0) : QObject(parent)
    {
    }

    QString name() const Q_DECL_OVERRIDE;
};

}

#endif // GAMMARAY_SELECTIONMODELINSPECTOR_H
