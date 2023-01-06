/*
  loggingcategorymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_LOGGINGCATEGORYMODEL_H
#define GAMMARAY_LOGGINGCATEGORYMODEL_H

#include <QAbstractTableModel>
#include <QLoggingCategory>
#include <QVector>

namespace GammaRay {
void categoryFilter(QLoggingCategory *category);

class LoggingCategoryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LoggingCategoryModel(QObject *parent = nullptr);
    ~LoggingCategoryModel() override;

    Q_INVOKABLE QByteArray exportLoggingConfig(bool all, bool forFile);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void addCategorySignal(QLoggingCategory *category);

private:
    void addCategorySlot(QLoggingCategory *category);
    void addCategory(QLoggingCategory *category);

    struct CategoryWithDefaultValues
    {
        QLoggingCategory *category;
        bool wasDebugEnabled;
        bool wasInfoEnabled;
        bool wasWarningEnabled;
        bool wasCriticalEnabled;
    };
    QVector<CategoryWithDefaultValues> m_categories;
    QLoggingCategory::CategoryFilter m_previousFilter;

    friend void categoryFilter(QLoggingCategory *);
    static LoggingCategoryModel *m_instance;
};
}

#endif // GAMMARAY_LOGGINGCATEGORYMODEL_H
