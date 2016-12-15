/*
  qtivipropertymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef GAMMARAY_QTIVI_QTIVIPROPERTYMODEL_H
#define GAMMARAY_QTIVI_QTIVIPROPERTYMODEL_H

#include "qtivipropertyoverrider.h"

#include <common/objectmodel.h>

#include <QAbstractItemModel>
#include <QHash>
#include <QString>

#include <vector>

class QIviProperty;

namespace GammaRay {

class Probe;

class QtIviPropertyModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum {
        ValueConstraintsRole = ObjectModel::UserRole + 1, // transmits the following constraints types

        RangeConstraints = 0, // min / max
        AvailableValuesConstraints // list of possible values
    };

    explicit QtIviPropertyModel(Probe *probe);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QMap<int, QVariant> itemData(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

private slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);
    void objectReparented(QObject *obj);
    void propertyValueChanged(const QVariant &value);
    void availabilityChanged();

public:
    struct IviProperty // public so we can have a free function in the .cpp file dealing with it
    {
        explicit IviProperty(QIviProperty *value, const QMetaProperty &metaProperty);
        IviProperty();
        QString name;
        QIviProperty *value;
        bool notWritableInPractice;
        QtIviPropertyOverrider overrider;
    };

private:
    int indexOfPropertyCarrier(const QObject *carrier) const;

    struct IviPropertyCarrier
    {
        QObject *carrier;
        std::vector<IviProperty> iviProperties;
        int indexOfProperty(const QIviProperty *property) const;
    };

    /// property tree model
    // "property carriers" are objects that have QIviProperty (static Qt meta) properties

    std::vector<IviPropertyCarrier> m_propertyCarriers;
    // m_seenObjects is not strictly needed currently but it helps debugging and will become more useful
    // when some of the current simplifying assumptions about object relationships must be discarded.
    QHash<QObject *, bool> m_seenObjects; // bool meaning: whether it has properties of type QtIVIProperty *
};

}

#endif
