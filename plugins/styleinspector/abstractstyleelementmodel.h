/*
  abstractstyleelementmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STYLEINSPECTOR_ABSTRACTSTYLEELEMENTMODEL_H
#define GAMMARAY_STYLEINSPECTOR_ABSTRACTSTYLEELEMENTMODEL_H

#include <QAbstractTableModel>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QStyle;
QT_END_NAMESPACE

namespace GammaRay {
/**
 * Base class for all models showing style elements.
 */
class AbstractStyleElementModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AbstractStyleElementModel(QObject *parent = nullptr);

    void setStyle(QStyle *style);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

protected:
    virtual QVariant doData(int row, int column, int role) const = 0;
    virtual int doColumnCount() const = 0;
    virtual int doRowCount() const = 0;

    /** Returns @c true if we are looking at the primary style of the application
     *  ie. the one set in QApplication. This takes proxy styles into account.
     */
    bool isMainStyle() const;

    /*! Returns the style of which we display the values, ie. the one set via
     *  setStyle, or if present its wrapping DynamicProxyStyle.
     */
    QStyle *effectiveStyle() const;

protected:
    QPointer<QStyle> m_style;
};
}

#endif // GAMMARAY_ABSTRACTSTYLEELEMENTMODEL_H
