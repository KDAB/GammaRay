/*
  propertymatrixmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tobias Koenig <tobias.koenig@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertymatrixmodel.h"

#include <QMatrix4x4>
#include <QQuaternion>
#include <QTransform>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

using namespace GammaRay;

PropertyMatrixModel::PropertyMatrixModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void PropertyMatrixModel::setMatrix(const QVariant &matrix)
{
    beginResetModel();
    m_matrix = matrix;
    endResetModel();
}

QVariant PropertyMatrixModel::matrix() const
{
    return m_matrix;
}

int PropertyMatrixModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    switch (m_matrix.typeId()) {
    case QMetaType::QVector2D:
        return 2;

    case QMetaType::QTransform:
    case QMetaType::QVector3D:
    case QMetaType::QQuaternion:
        return 3;

    case QMetaType::QMatrix4x4:
    case QMetaType::QVector4D:
        return 4;

    default:
        return 0;
    }
}

int PropertyMatrixModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    switch (m_matrix.typeId()) {
    case QMetaType::QVector2D:
    case QMetaType::QVector3D:
    case QMetaType::QVector4D:
    case QMetaType::QQuaternion:
        return 1;

    case QMetaType::QTransform:
        return 3;

    case QMetaType::QMatrix4x4:
        return 4;

    default:
        return 0;
    }
}

QVariant PropertyMatrixModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    switch (m_matrix.typeId()) {
    case QMetaType::QTransform: {
        const QTransform value = m_matrix.value<QTransform>();
        switch (index.row() << 4 | index.column()) {
        case 0x00:
            return value.m11();
        case 0x01:
            return value.m12();
        case 0x02:
            return value.m13();
        case 0x10:
            return value.m21();
        case 0x11:
            return value.m22();
        case 0x12:
            return value.m23();
        case 0x20:
            return value.m31();
        case 0x21:
            return value.m32();
        case 0x22:
            return value.m33();
        }

        break;
    }

    case QMetaType::QMatrix4x4: {
        const QMatrix4x4 value = m_matrix.value<QMatrix4x4>();
        return value(index.row(), index.column());
    }

    case QMetaType::QVector2D: {
        const QVector2D value = m_matrix.value<QVector2D>();
        switch (index.row()) {
        case 0:
            return value.x();
        case 1:
            return value.y();
        }

        break;
    }

    case QMetaType::QVector3D: {
        const QVector3D value = m_matrix.value<QVector3D>();
        switch (index.row()) {
        case 0:
            return value.x();
        case 1:
            return value.y();
        case 2:
            return value.z();
        }

        break;
    }

    case QMetaType::QVector4D: {
        const QVector4D value = m_matrix.value<QVector4D>();
        switch (index.row()) {
        case 0:
            return value.x();
        case 1:
            return value.y();
        case 2:
            return value.z();
        case 3:
            return value.w();
        }

        break;
    }

    case QMetaType::QQuaternion: {
        float pitch, yaw, roll;

        const QQuaternion value = m_matrix.value<QQuaternion>();
        value.getEulerAngles(&pitch, &yaw, &roll);
        switch (index.row()) {
        case 0:
            return pitch;
        case 1:
            return yaw;
        case 2:
            return roll;
        }

        break;
    }

    default:
        break;
    }

    return QVariant();
}

bool PropertyMatrixModel::setData(const QModelIndex &index, const QVariant &data, int role)
{
    if (!index.isValid())
        return false;

    if (role != Qt::EditRole)
        return false;

    bool ok = false;
    float floatData = data.toFloat(&ok);

    if (!ok)
        return false;

    switch (m_matrix.typeId()) {
    case QMetaType::QVector2D: {
        QVector2D value = m_matrix.value<QVector2D>();
        switch (index.row()) {
        case 0:
            value.setX(floatData);
            break;
        case 1:
            value.setY(floatData);
            break;
        }

        m_matrix = value;
        break;
    }

    case QMetaType::QVector3D: {
        QVector3D value = m_matrix.value<QVector3D>();
        switch (index.row()) {
        case 0:
            value.setX(floatData);
            break;
        case 1:
            value.setY(floatData);
            break;
        case 2:
            value.setZ(floatData);
            break;
        }

        m_matrix = value;
        break;
    }

    case QMetaType::QVector4D: {
        QVector4D value = m_matrix.value<QVector4D>();
        switch (index.row()) {
        case 0:
            value.setX(floatData);
            break;
        case 1:
            value.setY(floatData);
            break;
        case 2:
            value.setZ(floatData);
            break;
        case 3:
            value.setW(floatData);
            break;
        }

        m_matrix = value;
        break;
    }

    case QMetaType::QQuaternion: {
        float pitch, yaw, roll;

        const QQuaternion value = m_matrix.value<QQuaternion>();
        value.getEulerAngles(&pitch, &yaw, &roll);
        switch (index.row()) {
        case 0:
            pitch = floatData;
            break;
        case 1:
            yaw = floatData;
            break;
        case 2:
            roll = floatData;
            break;
        }

        m_matrix = QQuaternion::fromEulerAngles(pitch, yaw, roll);
        break;
    }

    case QMetaType::QTransform: {
        QTransform value = m_matrix.value<QTransform>();

        switch (index.row() << 4 | index.column()) {
        case 0x00:
            value.setMatrix(floatData, value.m12(), value.m13(), value.m21(),
                            value.m22(), value.m23(), value.m31(), value.m32(), value.m33());
            break;
        case 0x01:
            value.setMatrix(value.m11(), floatData, value.m13(), value.m21(),
                            value.m22(), value.m23(), value.m31(), value.m32(), value.m33());
            break;
        case 0x02:
            value.setMatrix(value.m11(), value.m12(), floatData, value.m21(),
                            value.m22(), value.m23(), value.m31(), value.m32(), value.m33());
            break;
        case 0x10:
            value.setMatrix(value.m11(), value.m12(), value.m13(), floatData,
                            value.m22(), value.m23(), value.m31(), value.m32(), value.m33());
            break;
        case 0x11:
            value.setMatrix(value.m11(), value.m12(), value.m13(),
                            value.m21(), floatData, value.m23(), value.m31(),
                            value.m32(), value.m33());
            break;
        case 0x12:
            value.setMatrix(value.m11(), value.m12(), value.m13(), value.m21(),
                            value.m22(), floatData, value.m31(), value.m32(), value.m33());
            break;
        case 0x20:
            value.setMatrix(value.m11(), value.m12(), value.m13(), value.m21(),
                            value.m22(), value.m23(), floatData, value.m32(), value.m33());
            break;
        case 0x21:
            value.setMatrix(value.m11(), value.m12(), value.m13(), value.m21(),
                            value.m22(), value.m23(), value.m31(), floatData, value.m33());
            break;
        case 0x22:
            value.setMatrix(value.m11(), value.m12(), value.m13(), value.m21(),
                            value.m22(), value.m23(), value.m31(), value.m32(), floatData);
            break;
        }

        m_matrix = value;
        break;
    }

    case QMetaType::QMatrix4x4: {
        QMatrix4x4 value = m_matrix.value<QMatrix4x4>();

        value(index.row(), index.column()) = floatData;

        m_matrix = value;
        break;
    }

    default:
        // TODO: should we assert here?
        break;
    }

    emit dataChanged(index, index);

    return true;
}

QVariant PropertyMatrixModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QAbstractTableModel::headerData(section, orientation, role);

    if (orientation == Qt::Horizontal) {
        switch (m_matrix.typeId()) {

        case QMetaType::QMatrix4x4:
            switch (section) {
            case 0:
                return tr("m_1");
            case 1:
                return tr("m_2");
            case 2:
                return tr("m_3");
            case 3:
                return tr("m_4");
            }
            break;

        case QMetaType::QTransform:
            switch (section) {
            case 0:
                return tr("m_1");
            case 1:
                return tr("m_2");
            case 2:
                return tr("m_3");
            }
            break;

        case QMetaType::QVector2D:
        case QMetaType::QVector3D:
        case QMetaType::QVector4D:
            return QString();
        default:
            break;
        }
    } else {
        switch (m_matrix.typeId()) {

        case QMetaType::QMatrix4x4:
            switch (section) {
            case 0:
                return tr("m1_");
            case 1:
                return tr("m2_");
            case 2:
                return tr("m3_");
            case 3:
                return tr("m4_");
            }
            break;

        case QMetaType::QTransform:
            switch (section) {
            case 0:
                return tr("m1_");
            case 1:
                return tr("m2_");
            case 2:
                return tr("m3_");
            }
            break;

        case QMetaType::QVector2D:
            switch (section) {
            case 0:
                return tr("x");
            case 1:
                return tr("y");
            }
            break;

        case QMetaType::QVector3D:
            switch (section) {
            case 0:
                return tr("x");
            case 1:
                return tr("y");
            case 2:
                return tr("z");
            }
            break;

        case QMetaType::QVector4D:
            switch (section) {
            case 0:
                return tr("x");
            case 1:
                return tr("y");
            case 2:
                return tr("z");
            case 3:
                return tr("w");
            }
            break;

        case QMetaType::QQuaternion:
            switch (section) {
            case 0:
                return tr("pitch");
            case 1:
                return tr("yaw");
            case 2:
                return tr("roll");
            }
            break;
        default:
            break;
        }
    }

    return QString();
}

Qt::ItemFlags PropertyMatrixModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}
