/*
  propertyeditorfactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

    addEditor(QMetaType::QColor, new QStandardItemEditorCreator<PropertyColorEditor>());
    addEditor(QMetaType::QByteArray, new QStandardItemEditorCreator<PropertyByteArrayEditor>(), true);
    addEditor(QMetaType::QFont, new QStandardItemEditorCreator<PropertyFontEditor>());
    addEditor(QMetaType::QPalette, new QStandardItemEditorCreator<PropertyPaletteEditor>(), true);
    addEditor(QMetaType::QPoint, new QStandardItemEditorCreator<PropertyPointEditor>());
    addEditor(QMetaType::QPointF, new QStandardItemEditorCreator<PropertyPointFEditor>());
    addEditor(QMetaType::QRect, new QStandardItemEditorCreator<PropertyRectEditor>());
    addEditor(QMetaType::QRectF, new QStandardItemEditorCreator<PropertyRectFEditor>());
    addEditor(QMetaType::QSize, new QStandardItemEditorCreator<PropertySizeEditor>());
    addEditor(QMetaType::QSizeF, new QStandardItemEditorCreator<PropertySizeFEditor>());
    addEditor(QMetaType::QString, new QStandardItemEditorCreator<PropertyTextEditor>(), true);
    addEditor(QMetaType::QTransform, new QStandardItemEditorCreator<PropertyMatrixEditor>());
    addEditor(QMetaType::QMatrix4x4, new QStandardItemEditorCreator<PropertyMatrixEditor>());
    addEditor(QMetaType::QVector2D, new QStandardItemEditorCreator<PropertyMatrixEditor>());
    addEditor(QMetaType::QVector3D, new QStandardItemEditorCreator<PropertyMatrixEditor>());
    addEditor(QMetaType::QVector4D, new QStandardItemEditorCreator<PropertyMatrixEditor>());
    addEditor(QMetaType::QQuaternion, new QStandardItemEditorCreator<PropertyMatrixEditor>());

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
        type = QMetaType::Double;
    }

    QWidget *w = QItemEditorFactory::createEditor(type, parent);
    if (!w)
        return nullptr;

    // the read-only view is still in the background usually, so transparency is not a good choice here
    w->setAutoFillBackground(true);
    w->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    return w;
}

QVector<int> PropertyEditorFactory::supportedTypes()
{
    return instance()->m_supportedTypes;
}

void PropertyEditorFactory::initBuiltInTypes()
{
    m_supportedTypes
        << QMetaType::Bool
        << QMetaType::Double
        << QMetaType::Int
        << QMetaType::UInt
        << QMetaType::QDate
        << QMetaType::QDateTime
        << QMetaType::QTime;

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
