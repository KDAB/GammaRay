/*
  execution.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_EXECUTION_H
#define GAMMARAY_EXECUTION_H

#include "gammaray_core_export.h"

#include <common/sourcelocation.h>

#include <QMetaType>
#include <QVector>

#include <memory>

namespace GammaRay {

/*! Functions to inspect the current program execution. */
namespace Execution
{
/*! Checks whether the given memory address is inside a read-only data section. */
GAMMARAY_CORE_EXPORT bool isReadOnlyData(const void *data);

/*! Returns @c true if we have the capability to generate stack traces. */
GAMMARAY_CORE_EXPORT bool stackTracingAvailable();

/*! Returns @c true if we have a way to obtain fast stack traces.
 *  That is, if we can defer stack frame resolution to later point in time.
 */
GAMMARAY_CORE_EXPORT bool hasFastStackTrace();

class TracePrivate;
/*! An instance of a backtrace.
 *  This is essentially just a list of addresses, and needs to
 *  be resolved to symbol names and source location for display.
 */
class GAMMARAY_CORE_EXPORT Trace {
public:
    Trace();
    Trace(const Trace &other);
    ~Trace();
    Trace& operator=(const Trace &other);

    bool empty() const;
    int size() const;
private:
    friend class TracePrivate;
    std::shared_ptr<TracePrivate> d;
};

/*! Create a backtrace.
 *  @param maxDepth The maximum amount of frames to trace
 *  @param skip The amount of frames to skip from the beginning. This is useful to
 *  exclude the code triggering the trace for example. Traces returned by this function
 *  already start at the calling frame, so you do not need to account for the internals
 *  of this here.
 */
GAMMARAY_CORE_EXPORT Trace stackTrace(int maxDepth, int skip = 0);

/*! A resolved frame in a stack trace. */
class GAMMARAY_CORE_EXPORT ResolvedFrame {
public:
    QString name;
    SourceLocation location;
};

/*! Resolve a single backtrace frame. */
GAMMARAY_CORE_EXPORT ResolvedFrame resolveOne(const Trace &trace, int index);
/*! Resolve an entire backtrace. */
GAMMARAY_CORE_EXPORT QVector<ResolvedFrame> resolveAll(const Trace &trace);

}

}

Q_DECLARE_METATYPE(GammaRay::Execution::Trace)

#endif // GAMMARAY_EXECUTION_H
