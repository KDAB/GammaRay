/*
  qtiviobjectmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef GAMMARAY_QTIVI_QTIVIOBJECTMODEL_H
#define GAMMARAY_QTIVI_QTIVIOBJECTMODEL_H

#include <common/objectmodel.h>

#include <QAbstractItemModel>
#include <QSet>
#include <QMetaProperty>

#include <memory>
#include <vector>

class QtIviObjectModelTest;

QT_BEGIN_NAMESPACE
class QIviDefaultPropertyOverrider;
QT_END_NAMESPACE

namespace GammaRay {

class Probe;
class ObjectId;

// The QtIviObjectModel is a model that expose QIviServiceObject and
// QIviAbstractFeature objects with any if their properties being plain
// Qt properties or QIviProperty.
class QtIviObjectModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class ::QtIviObjectModelTest;

public:
    enum Roles {
        ValueConstraintsRole = ObjectModel::UserRole + 1, // transmits the following constraints types
        RangeConstraints, // min / max
        MinRangeConstraints, // min
        MaxRangeConstraints, // max
        AvailableValuesConstraints, // list of possible values
        RawValue, // the raw variant value without any transformation
        IsIviProperty
    };

    enum Columns {
        NameColumn = 0, // The carrier label or property name
        ValueColumn,
        WritableColumn,
        OverrideColumn,
        TypeColumn,
        ColumnCount
    };

    explicit QtIviObjectModel(Probe *probe);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

private slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);
    void objectReparented(QObject *obj);
    void objectSelected(QObject *obj);
    void propertyChanged();

private:
    void emitRowDataChanged(const QModelIndex &index);
    int rowOfCarrier(const QObject *carrier) const;
    QModelIndex indexOfCarrier(const QObject *carrier, int column = 0 /*NameColumn*/) const;
    QModelIndex indexOfProperty(const QObject *carrier, const QByteArray &property, int column = 0 /*NameColumn*/) const;

    typedef std::shared_ptr<QT_PREPEND_NAMESPACE(QIviDefaultPropertyOverrider)> QIviDefaultPropertyOverriderPtr;
    struct ServiceZone {
        QString m_zone;
        QIviDefaultPropertyOverriderPtr m_service;
    };

    std::vector<ServiceZone> m_serviceCarriers;
    QSet<QObject *> m_handledObjects;
};

}

#endif
