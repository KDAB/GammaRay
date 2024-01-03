/*
  translatorsmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef TRANSLATORSMODEL_H
#define TRANSLATORSMODEL_H

#include <QAbstractTableModel>

namespace GammaRay {
class TranslatorWrapper;

class TranslatorsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TranslatorsModel(QObject *parent = nullptr);

    enum ExtraRoles
    {
        ObjectIdRole = Qt::UserRole + 1
    };

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

    TranslatorWrapper *translator(const QModelIndex &index) const;

public slots:
    void registerTranslator(GammaRay::TranslatorWrapper *translator);
    void unregisterTranslator(GammaRay::TranslatorWrapper *translator);

private slots:
    void sourceDataChanged();

private:
    QList<TranslatorWrapper *> m_translators;
};
}

#endif
