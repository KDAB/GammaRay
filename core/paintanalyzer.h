/*
  paintanalyzer.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PAINTANALYZER_H
#define GAMMARAY_PAINTANALYZER_H

#include "gammaray_core_export.h"

#include <common/objectid.h>
#include <core/objectinstance.h>

#include <common/paintanalyzerinterface.h>

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
class QPaintDevice;
class QRectF;
class QSortFilterProxyModel;
QT_END_NAMESPACE

namespace GammaRay {
class AggregatedPropertyModel;
class PaintBuffer;
class PaintBufferModel;
class RemoteViewServer;
class StackTraceModel;

/** Inspects individual operations on a QPainter. */
class GAMMARAY_CORE_EXPORT PaintAnalyzer : public PaintAnalyzerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::PaintAnalyzerInterface)
public:
    explicit PaintAnalyzer(const QString &name, QObject *parent = nullptr);
    ~PaintAnalyzer() override;

    /** Reset the paint analyzer state for providing a new analysis asynchronously. */
    void reset();

    // call the following 4 methods in this order to trigger a paint analysis
    void beginAnalyzePainting();
    void setBoundingRect(const QRectF &boundingBox);
    QPaintDevice *paintDevice() const;
    void endAnalyzePainting();


    /** Returns @c true if paint analysis is available (needs access to Qt private headers at compile time). */
    static bool isAvailable();

    /**
     * Marks all following paint operations to origin from the given QWidget/QQuickItem
     * until this is called with another object.
     */
    void setOrigin(const ObjectId &obj);

signals:
    /** Polling for updated analysis. */
    void requestUpdate();

private slots:
    void repaint();

private:
    PaintBufferModel *m_paintBufferModel;
    QSortFilterProxyModel *m_paintBufferFilter;
    QItemSelectionModel *m_selectionModel;
    PaintBuffer *m_paintBuffer;
    RemoteViewServer *m_remoteView;
    AggregatedPropertyModel *m_argumentModel;
    ObjectInstance m_currentArgument;
    StackTraceModel *m_stackTraceModel;
};
}

#endif // GAMMARAY_PAINTANALYZER_H
