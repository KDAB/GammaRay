/*
  translatorsmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    void registerTranslator(TranslatorWrapper *translator);
    void unregisterTranslator(TranslatorWrapper *translator);

private slots:
    void sourceDataChanged();

private:
    QList<TranslatorWrapper *> m_translators;
};
}

#endif
