/*
  styleelementstatetablepage.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STYLEINSPECTOR_STYLEELEMENTSTATETABLEPAGE_H
#define GAMMARAY_STYLEINSPECTOR_STYLEELEMENTSTATETABLEPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
QT_END_NAMESPACE

namespace GammaRay {
class StyleInspectorInterface;

namespace Ui {
class StyleElementStateTablePage;
}

/**
 * Tab page for showing a style element x state table and corresponding config UI.
 */
class StyleElementStateTablePage : public QWidget
{
    Q_OBJECT
public:
    explicit StyleElementStateTablePage(QWidget *parent = nullptr);
    ~StyleElementStateTablePage() override;
    void setModel(QAbstractItemModel *model);

protected:
    void showEvent(QShowEvent *show) override;

private slots:
    void updateCellSize();

private:
    Ui::StyleElementStateTablePage *ui;
    StyleInspectorInterface *m_interface;
};
}

#endif // GAMMARAY_STYLEELEMENTSTATETABLEPAGE_H
