/*
  styleelementproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STYLEINSPECTOR_STYLEELEMENTPROXYMODEL_H
#define GAMMARAY_STYLEINSPECTOR_STYLEELEMENTPROXYMODEL_H

#include <QIdentityProxyModel>

#include <QSize>

namespace GammaRay {
class StyleElementProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit StyleModelProxy(QObject *parent = nullptr);

    virtual QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;

public slots:
    void setWidth(int width);
    void setHeight(int height);
    void setZoomFactor(int zoom);

private:
    QSize m_sizeHint;
};
}

#endif // GAMMARAY_STYLEELEMENTPROXYMODEL_H
