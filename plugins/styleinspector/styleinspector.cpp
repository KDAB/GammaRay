/*
  styleinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "styleinspector.h"
#include "complexcontrolmodel.h"
#include "controlmodel.h"
#include "pixelmetricmodel.h"
#include "primitivemodel.h"
#include "standardiconmodel.h"
#include "stylehintmodel.h"

#include <core/objecttypefilterproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>

#include <ui/palettemodel.h>
#include <common/objectbroker.h>

#include <QApplication>
#include <QItemSelectionModel>

using namespace GammaRay;

StyleInspector::StyleInspector(Probe *probe, QObject *parent)
    : StyleInspectorInterface(parent)
    , m_primitiveModel(new PrimitiveModel(this))
    , m_controlModel(new ControlModel(this))
    , m_complexControlModel(new ComplexControlModel(this))
    , m_pixelMetricModel(new PixelMetricModel(this))
    , m_standardIconModel(new StandardIconModel(this))
    , m_standardPaletteModel(new PaletteModel(this))
    , m_styleHintModel(new StyleHintModel(this))
{
    auto *styleFilter = new ObjectTypeFilterProxyModel<QStyle>(this);
    styleFilter->setSourceModel(probe->objectListModel());
    auto *singleColumnProxy = new SingleColumnObjectProxyModel(this);
    singleColumnProxy->setSourceModel(styleFilter);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.StyleList"), singleColumnProxy);

    QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(singleColumnProxy);
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            this, &StyleInspector::styleSelected);

    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.StyleInspector.PrimitiveModel"), m_primitiveModel);
    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.StyleInspector.ControlModel"), m_controlModel);
    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.StyleInspector.ComplexControlModel"),
                         m_complexControlModel);
    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.StyleInspector.PixelMetricModel"),
                         m_pixelMetricModel);
    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.StyleInspector.StandardIconModel"),
                         m_standardIconModel);
    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.StyleInspector.PaletteModel"),
                         m_standardPaletteModel);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.StyleInspector.StyleHintModel"), m_styleHintModel);
}

StyleInspector::~StyleInspector() = default;

void StyleInspector::styleSelected(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;
    const QModelIndex index = selection.first().topLeft();
    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
    QStyle *style = qobject_cast<QStyle *>(obj);
    m_primitiveModel->setStyle(style);
    m_controlModel->setStyle(style);
    m_complexControlModel->setStyle(style);
    m_pixelMetricModel->setStyle(style);
    m_standardIconModel->setStyle(style);
    m_standardPaletteModel->setPalette(style ? style->standardPalette() : qApp->palette());
    m_styleHintModel->setStyle(style);
}
