/*
  propertyeditorfactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "propertyeditorfactory.h"
#include "propertycoloreditor.h"
#include "propertyenumeditor.h"
#include "propertyfonteditor.h"
#include "propertyintpaireditor.h"
#include "propertydoublepaireditor.h"
#include "propertypaletteeditor.h"
#include "propertymatrixeditor.h"
#include "propertytexteditor.h"
#include "propertyrecteditor.h"
#include "propertymargineditor.h"

#include <QItemEditorFactory>

#include <algorithm>

using namespace GammaRay;

PropertyEditorFactory::PropertyEditorFactory()
{
    initBuiltInTypes();

    addEditor(QVariant::Color, new QStandardItemEditorCreator<PropertyColorEditor>());
    addEditor(QVariant::ByteArray, new QStandardItemEditorCreator<PropertyByteArrayEditor>(), true);
    addEditor(QVariant::Font, new QStandardItemEditorCreator<PropertyFontEditor>());
    addEditor(QVariant::Palette, new QStandardItemEditorCreator<PropertyPaletteEditor>(), true);
    addEditor(QVariant::Point, new QStandardItemEditorCreator<PropertyPointEditor>());
    addEditor(QVariant::PointF, new QStandardItemEditorCreator<PropertyPointFEditor>());
    addEditor(QVariant::Rect, new QStandardItemEditorCreator<PropertyRectEditor>());
    addEditor(QVariant::RectF, new QStandardItemEditorCreator<PropertyRectFEditor>());
    addEditor(QVariant::Size, new QStandardItemEditorCreator<PropertySizeEditor>());
    addEditor(QVariant::SizeF, new QStandardItemEditorCreator<PropertySizeFEditor>());
    addEditor(QVariant::String, new QStandardItemEditorCreator<PropertyTextEditor>(), true);
    addEditor(QVariant::Transform, new QStandardItemEditorCreator<PropertyMatrixEditor>());
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    addEditor(QVariant::Matrix, new QStandardItemEditorCreator<PropertyMatrixEditor>());
#endif
    addEditor(QVariant::Matrix4x4, new QStandardItemEditorCreator<PropertyMatrixEditor>());
    addEditor(QVariant::Vector2D, new QStandardItemEditorCreator<PropertyMatrixEditor>());
    addEditor(QVariant::Vector3D, new QStandardItemEditorCreator<PropertyMatrixEditor>());
    addEditor(QVariant::Vector4D, new QStandardItemEditorCreator<PropertyMatrixEditor>());
    addEditor(QVariant::Quaternion, new QStandardItemEditorCreator<PropertyMatrixEditor>());

    registerEditor(static_cast<TypeId>(qMetaTypeId<EnumValue>()), new QStandardItemEditorCreator<PropertyEnumEditor>());

    registerEditor(static_cast<TypeId>(qMetaTypeId<QMargins>()), new QStandardItemEditorCreator<PropertyMarginsEditor>());
    registerEditor(static_cast<TypeId>(qMetaTypeId<QMarginsF>()), new QStandardItemEditorCreator<PropertyMarginsFEditor>());
}

PropertyEditorFactory *PropertyEditorFactory::instance()
{
    static auto *s_instance = new PropertyEditorFactory;
    return s_instance;
}

QWidget *PropertyEditorFactory::createEditor(TypeId type, QWidget *parent) const
{
    if (type == QMetaType::Float) {
        /* coverity[mixed_enums] */
        type = QVariant::Double;
    }

    QWidget *w = QItemEditorFactory::createEditor(type, parent);
    if (!w)
        return nullptr;

    // the read-only view is still in the background usually, so transparency is not a good choice here
    w->setAutoFillBackground(true);
    return w;
}

QVector<int> PropertyEditorFactory::supportedTypes()
{
    return instance()->m_supportedTypes;
}

void PropertyEditorFactory::initBuiltInTypes()
{
    m_supportedTypes
        << QVariant::Bool
        << QVariant::Double
        << QVariant::Int
        << QVariant::UInt
        << QVariant::Date
        << QVariant::DateTime
        << QVariant::Time;

    m_supportedTypes << QMetaType::Float;
}

void PropertyEditorFactory::addEditor(PropertyEditorFactory::TypeId type, QItemEditorCreatorBase *creator, bool extended)
{
    registerEditor(type, creator);
    m_supportedTypes.push_back(type);
    if (extended) {
        auto it = std::lower_bound(m_extendedTypes.begin(), m_extendedTypes.end(), type);
        m_extendedTypes.insert(it, type);
    }
}

bool PropertyEditorFactory::hasExtendedEditor(int typeId)
{
    return std::binary_search(instance()->m_extendedTypes.constBegin(), instance()->m_extendedTypes.constEnd(), typeId);
}
