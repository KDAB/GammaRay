/**************************************************************************
**
** This code is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "processfiltermodel.h"

bool operator<(const ProcData &p1, const ProcData &p2)
{
    return p1.name < p2.name;
}

ProcessListFilterModel::ProcessListFilterModel(QObject *parent)
  : QSortFilterProxyModel(parent),
    m_model(new QStandardItemModel(this))
{
    QStringList columns;
    columns << tr("Process ID")
            << tr("Name")
            << tr("State");
    m_model->setHorizontalHeaderLabels(columns);
    setSourceModel(m_model);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setFilterKeyColumn(1);
}

bool ProcessListFilterModel::lessThan(const QModelIndex &left,
    const QModelIndex &right) const
{
    const QString l = sourceModel()->data(left).toString();
    const QString r = sourceModel()->data(right).toString();
    if (left.column() == 0)
        return l.toInt() < r.toInt();
    return l < r;
}

QString ProcessListFilterModel::processIdAt(const QModelIndex &index) const
{
    if (index.isValid()) {
        const QModelIndex index0 = mapToSource(index);
        QModelIndex siblingIndex = index0.sibling(index0.row(), 0);
        if (const QStandardItem *item = m_model->itemFromIndex(siblingIndex))
            return item->text();
    }
    return QString();
}

QString ProcessListFilterModel::executableForPid(const QString &pid) const
{
    const int rowCount = m_model->rowCount();
    for (int r = 0; r < rowCount; r++) {
        const QStandardItem *item = m_model->item(r, 0);
        if (item->text() == pid) {
            QString name = item->data(ProcessImageRole).toString();
            if (name.isEmpty())
                name = item->data(ProcessNameRole).toString();
            return name;
        }
    }
    return QString();
}

void ProcessListFilterModel::populate
    (QList<ProcData> processes, const QString &excludePid)
{
    qStableSort(processes);

    if (const int rowCount = m_model->rowCount())
        m_model->removeRows(0, rowCount);

    QStandardItem *root  = m_model->invisibleRootItem();
    foreach (const ProcData &proc, processes) {
        QList<QStandardItem *> row;
        row.append(new QStandardItem(proc.ppid));
        QString name = proc.image.isEmpty() ? proc.name : proc.image;
        row.back()->setData(name, ProcessImageRole);
        row.append(new QStandardItem(proc.name));
        row.back()->setToolTip(proc.image);
        row.append(new QStandardItem(proc.state));

        if (proc.ppid == excludePid)
            foreach (QStandardItem *item, row)
                item->setEnabled(false);
        root->appendRow(row);
    }
}