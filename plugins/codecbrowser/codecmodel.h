/*
  codecmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CODECBROWSER_CODECMODEL_H
#define GAMMARAY_CODECBROWSER_CODECMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QTextCodec>

namespace GammaRay {
class AllCodecsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AllCodecsModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QList<QByteArray> m_codecs;
};

class SelectedCodecsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SelectedCodecsModel(QObject *parent = nullptr);

    void setCodecs(const QStringList &codecs);
    QStringList currentCodecs() const;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

public slots:
    void updateText(const QString &text);

private:
    QStringList m_codecs;
    QString m_text;
};
}

#endif
