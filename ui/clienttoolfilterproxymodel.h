/*
  clienttoolfilterproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CLIENTTOOLFILTERPROXYMODEL_H
#define GAMMARAY_CLIENTTOOLFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace GammaRay {

class ClientToolFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ClientToolFilterProxyModel(QObject *parent = nullptr);
    ~ClientToolFilterProxyModel() override;

    bool filterInactiveTools() const;

public slots:
    void setFilterInactiveTools(bool enable);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    bool m_filterInactiveTools;
};

}

#endif // GAMMARAY_CLIENTTOOLFILTERPROXYMODEL_H
