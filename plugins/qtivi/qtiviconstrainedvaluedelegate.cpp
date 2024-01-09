/*
  qtiviconstrainedvaluedelegate.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "qtiviconstrainedvaluedelegate.h"

#include "qtivipropertymodel.h"

#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QSpinBox>

using namespace GammaRay;

// The difference between this class and QComboBox is that the "user" property, from which the value to
// send to the model is taken when the delegate is closed, is not currentText like in QComboBox,
// but (a "forwarded copy" of) currentData(). The origin of that data is the second argument to
// QComboBox::addItem(const QString &text, const QVariant &userData) and similar methods.
class SplitDataComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QVariant modelData READ currentData USER true) // clazy:exclude=qproperty-without-notify

public:
    explicit SplitDataComboBox(QWidget *parent = nullptr)
        : QComboBox(parent)
    {
    }
};


QtIviConstrainedValueDelegate::QtIviConstrainedValueDelegate(QObject *parent)
    : PropertyEditorDelegate(parent)
{
}

QtIviConstrainedValueDelegate::~QtIviConstrainedValueDelegate() = default;

QWidget *QtIviConstrainedValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                                     const QModelIndex &index) const
{
    int constraintsType = -1;
    QVariantList constraintValues;

    const QVariant constraintsContainer = index.data(QtIviPropertyModel::ValueConstraintsRole);
    if (constraintsContainer.isValid() && constraintsContainer.canConvert<QVariantList>()) {
        QVariantList constraints = constraintsContainer.value<QVariantList>();
        if (!constraints.isEmpty()) {
            const QVariant type = constraints.takeFirst();
            if (type.isValid() && type.canConvert<uint>()) {
                constraintsType = type.toUInt();
                constraintValues = std::move(constraints);
            }
        }
    }

    // Conceptually, what we are doing below should probably be done in setEditorData(), but then we'd
    // have to repeat most of the work above. The current approach should also work because constraints
    // don't change very often (or at all, currently).

    switch (constraintsType) {
    case QtIviPropertyModel::RangeConstraints:
        if (constraintValues.size() == 2) {
            QWidget *editor = PropertyEditorDelegate::createEditor(parent, option, index);

            if (QSpinBox *const intSb = qobject_cast<QSpinBox *>(editor)) {
                intSb->setMinimum(constraintValues.at(0).toInt());
                intSb->setMaximum(constraintValues.at(1).toInt());
            } else if (QDoubleSpinBox *const doubleSb = qobject_cast<QDoubleSpinBox *>(editor)) {
                doubleSb->setMinimum(constraintValues.at(0).toDouble());
                doubleSb->setMaximum(constraintValues.at(1).toDouble());
            } else {
                qWarning() << "Unhandled numeric editing delegate, cannot apply value constraints!";
            }
            return editor;
        }
        break;
    case QtIviPropertyModel::AvailableValuesConstraints: {
        auto *combo = new SplitDataComboBox(parent);
        combo->setAutoFillBackground(true);
        for (int i = 0; i + 1 < constraintValues.size(); i += 2) {
            combo->addItem(constraintValues.at(i).toString(), constraintValues.at(i + 1));
            if (constraintValues.at(i + 1) == index.data(Qt::EditRole))
                combo->setCurrentIndex(i / 2);
        }
        return combo;
    } break;
    default:
        qWarning() << "Unhandled type of value constraints" << constraintsType;
        break;
    }

    return PropertyEditorDelegate::createEditor(parent, option, index);
}

#include "qtiviconstrainedvaluedelegate.moc"
