/*
  kjobmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_KJOBTRACKER_KJOBMODEL_H
#define GAMMARAY_KJOBTRACKER_KJOBMODEL_H

#include <QAbstractItemModel>
#include <QVector>

class KJob;

namespace GammaRay {
class KJobModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit KJobModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

public slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);

private slots:
    void jobResult(KJob *job);
    void jobFinished(KJob *obj);
    void jobInfo(KJob *job, const QString &plainMessage);

private:
    int indexOfJob(QObject *obj) const;

    struct KJobInfo
    {
        KJob *job;
        QString name;
        QString type;
        QString statusText;
        enum
        {
            Running,
            Finished,
            Error,
            Killed,
            Deleted
        } state;
    };
    QVector<KJobInfo> m_data;
};
}

#endif // GAMMARAY_KJOBMODEL_H
