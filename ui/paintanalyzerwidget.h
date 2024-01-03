/*
  paintanalyzerwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_WIDGETINSPECTOR_PAINTANALYZERWIDGET_H
#define GAMMARAY_WIDGETINSPECTOR_PAINTANALYZERWIDGET_H

#include "gammaray_ui_export.h"

#include <QWidget>

namespace GammaRay {
namespace Ui {
class PaintAnalyzerWidget;
}
class PaintAnalyzerInterface;

/**
 * A widget to look at the command list in a QPaintBuffer.
 */
class GAMMARAY_UI_EXPORT PaintAnalyzerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PaintAnalyzerWidget(QWidget *parent = nullptr);
    ~PaintAnalyzerWidget() override;

    void setBaseName(const QString &name);

private slots:
    void detailsChanged();
    void commandContextMenu(QPoint pos);
    void stackTraceContextMenu(QPoint pos);

private:
    QScopedPointer<Ui::PaintAnalyzerWidget> ui;
    PaintAnalyzerInterface *m_iface;
};
}

#endif
