/*
  probeabi.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROBEABI_H
#define GAMMARAY_PROBEABI_H

#include "gammaray_launcher_export.h"

#include <QSharedDataPointer>

namespace GammaRay {
class ProbeABIPrivate;

/*! Describes a probe ABI.
 *  The probe ABI is everything that determines if a probe is compatible
 *  with a given target or not.
 */
class GAMMARAY_LAUNCHER_EXPORT ProbeABI
{
public:
    ProbeABI();
    ProbeABI(const ProbeABI &other);
    ~ProbeABI();

    ProbeABI &operator=(const ProbeABI &other);

    /*! Qt version. */
    int majorQtVersion() const;
    int minorQtVersion() const;
    void setQtVersion(int major, int minor);
    bool hasQtVersion() const;

    /*! Processor architecture. */
    QString architecture() const;
    void setArchitecture(const QString &architecture);

    /*! Debug vs. release. */
    bool isDebug() const;
    void setIsDebug(bool debug);
    /*! Returns @c true if debug vs. release is changing the ABI. */
    static bool isDebugRelevant();

    /*! Compiler ABI is currently only relevant for MSVC vs. Mingw. */
    QString compiler() const;
    void setCompiler(const QString &compiler);

    /*! The compiler ABI version is currently only relevant for MSVC */
    QString compilerVersion() const;
    void setCompilerVersion(const QString &compilerVersion);

    /*! Returns @c true if the version is changing the ABI. */
    bool isVersionRelevant() const;

    /*! Returns if this probe ABI is fully specified. */
    bool isValid() const;

    /*! Checks if this is ABI is compatible with @p referenceABI. */
    bool isCompatible(const ProbeABI &referenceABI) const;

    bool operator==(const ProbeABI &rhs) const;

    /*! @brief Orders probes by Qt version.
     *  This is useful to pick the best matching one of multiple compatible ones.
     */
    bool operator<(const ProbeABI &rhs) const;

    /*! Conversion from and to probe ABI identifiers. */
    QString id() const;
    static ProbeABI fromString(const QString &id);

    /*! User-visible name of this ABI. */
    QString displayString() const;

private:
    QSharedDataPointer<ProbeABIPrivate> d;
};
}

#endif // GAMMARAY_PROBEABI_H
