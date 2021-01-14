/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
