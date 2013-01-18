/*
  kjobmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_KJOBMODEL_H
#define GAMMARAY_KJOBMODEL_H

#include <QAbstractItemModel>
#include <QVector>

class KJob;

namespace GammaRay {

class KJobModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    explicit KJobModel(QObject *parent = 0);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

  private slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);
    void jobResult(KJob *job);
    void jobFinished(KJob *obj);
    void jobInfo(KJob *job, const QString &plainMessage);

  private:
    int indexOfJob(QObject *obj) const;

    struct KJobInfo {
      KJob *job;
      QString name;
      QString type;
      QString statusText;
      enum {
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
