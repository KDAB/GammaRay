/*
  paintanalyzer.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
