/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "selectionratiosource.h"
#include "abstractdatasource_p.h"
#include "logging_p.h"

#include <QDebug>
#include <QHash>
#include <QItemSelectionModel>
#include <QSettings>
#include <QStringList>
#include <QTime>
#include <QElapsedTimer>

#include <memory>

using namespace KUserFeedback;

namespace KUserFeedback {
class SelectionRatioSourcePrivate : public AbstractDataSourcePrivate
{
public:
    SelectionRatioSourcePrivate();
    ~SelectionRatioSourcePrivate();

    void selectionChanged();
    QString selectedValue() const;

    QItemSelectionModel *model;
    QMetaObject::Connection monitorConnection;
    QString description;
    QString previousValue;
    QElapsedTimer lastChangeTime;
    QHash<QString, int> ratioSet; // data we are currently tracking
    QHash<QString, int> baseRatioSet; // data loaded from storage
    int role;
};

}

SelectionRatioSourcePrivate::SelectionRatioSourcePrivate()
    : model(nullptr)
    , role(Qt::DisplayRole)
{
}

SelectionRatioSourcePrivate::~SelectionRatioSourcePrivate()
{
    QObject::disconnect(monitorConnection);
}

void SelectionRatioSourcePrivate::selectionChanged()
{
    if (!previousValue.isEmpty() && lastChangeTime.elapsed() > 1000) {
        ratioSet[previousValue] += lastChangeTime.elapsed() / 1000;
    }

    lastChangeTime.start();
    previousValue = selectedValue();
}

QString SelectionRatioSourcePrivate::selectedValue() const
{
    const auto idxs = model->selectedIndexes();
    if (!model->hasSelection() || idxs.isEmpty())
        return QString();
    Q_ASSERT(!idxs.isEmpty());
    const auto &idx = idxs.at(0);
    return idx.data(role).toString();
}

SelectionRatioSource::SelectionRatioSource(QItemSelectionModel* selectionModel, const QString& sampleName)
    : AbstractDataSource(sampleName, Provider::DetailedUsageStatistics, new SelectionRatioSourcePrivate)
{
    Q_D(SelectionRatioSource);

    d->model = selectionModel;
    Q_ASSERT(selectionModel);

    d->monitorConnection = QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged, [this]() {
        Q_D(SelectionRatioSource);
        d->selectionChanged();
    });
    d->lastChangeTime.start();
    d->selectionChanged();
}

void SelectionRatioSource::setRole(int role)
{
    Q_D(SelectionRatioSource);
    d->role = role;
}

QString SelectionRatioSource::description() const
{
    Q_D(const SelectionRatioSource);
    return d->description;
}

void SelectionRatioSource::setDescription(const QString& desc)
{
    Q_D(SelectionRatioSource);
    d->description = desc;
}

QVariant SelectionRatioSource::data()
{
    Q_D(SelectionRatioSource);
    d->selectionChanged();

    QVariantMap m;
    int total = 0;
    for (auto it = d->ratioSet.constBegin(); it != d->ratioSet.constEnd(); ++it)
        total += it.value() + d->baseRatioSet.value(it.key());
    if (total <= 0)
        return m;

    for (auto it = d->ratioSet.constBegin(); it != d->ratioSet.constEnd(); ++it) {
        double currentValue = it.value() + d->baseRatioSet.value(it.key());
        QVariantMap v;
        v.insert(QStringLiteral("property"), currentValue / (double)(total));
        m.insert(it.key(), v);
    }
    return m;
}

void SelectionRatioSource::loadImpl(QSettings *settings)
{
    Q_D(SelectionRatioSource);
    foreach (const auto &value, settings->childKeys()) {
        const auto amount = std::max(settings->value(value, 0).toInt(), 0);
        d->baseRatioSet.insert(value, amount);
        if (!d->ratioSet.contains(value))
            d->ratioSet.insert(value, 0);
    }
}

void SelectionRatioSource::storeImpl(QSettings *settings)
{
    Q_D(SelectionRatioSource);
    d->selectionChanged();

    // note that a second process can have updated the data meanwhile!
    for (auto it = d->ratioSet.begin(); it != d->ratioSet.end(); ++it) {
        if (it.value() == 0)
            continue;
        const auto oldValue = std::max(settings->value(it.key(), 0).toInt(), 0);
        const auto newValue = oldValue + it.value();
        settings->setValue(it.key(), newValue);
        *it = 0;
        d->baseRatioSet.insert(it.key(), newValue);
    }
}

void SelectionRatioSource::resetImpl(QSettings* settings)
{
    Q_D(SelectionRatioSource);
    d->baseRatioSet.clear();
    d->ratioSet.clear();
    settings->remove(QString());
}
