/*
  paintbufferviewer.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_WIDGETINSPECTOR_PAINTBUFFERVIEWER_H
#define GAMMARAY_WIDGETINSPECTOR_PAINTBUFFERVIEWER_H

#include "gammaray_ui_export.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
QT_END_NAMESPACE

namespace GammaRay {
class WidgetInspectorInterface;

namespace Ui {
class PaintBufferViewer;
}

/**
 * A widget to look at the command list in a QPaintBuffer.
 */
class GAMMARAY_UI_EXPORT PaintBufferViewer : public QDialog
{
    Q_OBJECT
public:
    explicit PaintBufferViewer(const QString &name, QWidget *parent = nullptr);
    ~PaintBufferViewer() override;

private:
    QScopedPointer<Ui::PaintBufferViewer> ui;
};
}

#endif
