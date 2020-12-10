/*
  propertymatrixmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Tobias Koenig <tobias.koenig@kdab.com>

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

#include "propertymatrixmodel.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QMatrix>
#endif
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

    switch (m_matrix.type()) {
    case QVariant::Vector2D:
        return 2;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    case QVariant::Matrix:
#endif
    case QVariant::Transform:
    case QVariant::Vector3D:
    case QVariant::Quaternion:
        return 3;

    case QVariant::Matrix4x4:
    case QVariant::Vector4D:
        return 4;

    default:
        return 0;
    }
}

int PropertyMatrixModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    switch (m_matrix.type()) {
    case QVariant::Vector2D:
    case QVariant::Vector3D:
    case QVariant::Vector4D:
    case QVariant::Quaternion:
        return 1;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    case QVariant::Matrix:
        return 2;
#endif

    case QVariant::Transform:
        return 3;

    case QVariant::Matrix4x4:
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

    switch (m_matrix.type()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    case QVariant::Matrix:
    {
        const QMatrix value = m_matrix.value<QMatrix>();
        switch (index.row() << 4 | index.column()) {
        case 0x00:
            return value.m11();
        case 0x01:
            return value.m12();
        case 0x10:
            return value.m21();
        case 0x11:
            return value.m22();
        case 0x20:
            return value.dx();
        case 0x21:
            return value.dy();
        }

        break;
    }
#endif

    case QVariant::Transform:
    {
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

    case QVariant::Matrix4x4:
    {
        const QMatrix4x4 value = m_matrix.value<QMatrix4x4>();
        return value(index.row(), index.column());
    }

    case QVariant::Vector2D:
    {
        const QVector2D value = m_matrix.value<QVector2D>();
        switch (index.row()) {
        case 0:
            return value.x();
        case 1:
            return value.y();
        }

        break;
    }

    case QVariant::Vector3D:
    {
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

    case QVariant::Vector4D:
    {
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

    case QVariant::Quaternion:
    {
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

    switch (m_matrix.type()) {
    case QVariant::Vector2D:
    {
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

    case QVariant::Vector3D:
    {
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

    case QVariant::Vector4D:
    {
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

    case QVariant::Quaternion:
    {
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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    case QVariant::Matrix:
    {
        QMatrix value = m_matrix.value<QMatrix>();

        switch (index.row() << 4 | index.column()) {
        case 0x00:
            value.setMatrix(floatData, value.m12(), value.m21(), value.m22(), value.dx(),
                            value.dy());
            break;
        case 0x01:
            value.setMatrix(value.m11(), floatData, value.m21(), value.m22(), value.dx(),
                            value.dy());
            break;
        case 0x10:
            value.setMatrix(value.m11(), value.m12(), floatData, value.m22(), value.dx(),
                            value.dy());
            break;
        case 0x11:
            value.setMatrix(value.m11(), value.m12(), value.m21(), floatData, value.dx(),
                            value.dy());
            break;
        case 0x20:
            value.setMatrix(value.m11(), value.m12(), value.m21(), value.m22(), floatData,
                            value.dy());
            break;
        case 0x21:
            value.setMatrix(value.m11(), value.m12(), value.m21(), value.m22(),
                            value.dx(), floatData);
            break;
        }

        m_matrix = value;
        break;
    }
#endif

    case QVariant::Transform:
    {
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

    case QVariant::Matrix4x4:
    {
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
        switch (m_matrix.type()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        case QVariant::Matrix:
            switch (section) {
            case 0:
                return tr("m_1");
            case 1:
                return tr("m_2");
            }
            break;
#endif

        case QVariant::Matrix4x4:
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

        case QVariant::Transform:
            switch (section) {
            case 0:
                return tr("m_1");
            case 1:
                return tr("m_2");
            case 2:
                return tr("m_3");
            }
            break;

        case QVariant::Vector2D:
        case QVariant::Vector3D:
        case QVariant::Vector4D:
            return QString();
        default:
            break;
        }
    } else {
        switch (m_matrix.type()) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        case QVariant::Matrix:
            switch (section) {
            case 0:
                return tr("m1_");
            case 1:
                return tr("m2_");
            case 2:
                return tr("d x/y");
            }
            break;
#endif

        case QVariant::Matrix4x4:
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

        case QVariant::Transform:
            switch (section) {
            case 0:
                return tr("m1_");
            case 1:
                return tr("m2_");
            case 2:
                return tr("m3_");
            }
            break;

        case QVariant::Vector2D:
            switch (section) {
            case 0:
                return tr("x");
            case 1:
                return tr("y");
            }
            break;

        case QVariant::Vector3D:
            switch (section) {
            case 0:
                return tr("x");
            case 1:
                return tr("y");
            case 2:
                return tr("z");
            }
            break;

        case QVariant::Vector4D:
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

        case QVariant::Quaternion:
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
