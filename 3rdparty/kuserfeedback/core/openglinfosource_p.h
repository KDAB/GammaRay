/*
    Copyright (C) 2017 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef USERFEEDBACK_OPENGLINFOSOURCE_P_H
#define USERFEEDBACK_OPENGLINFOSOURCE_P_H

#include <QVariantMap>

namespace UserFeedback {

// Internal functions made accessible for unit tests
class OpenGLInfoSourcePrivate
{
public:
    static inline void parseGLVersion(const char *version, QVariantMap &m)
    {
        auto vendorVersion = QString::fromLocal8Bit(version);
        const auto idx = vendorVersion.indexOf(QLatin1Char(' '));
        if (idx > 0) {
            vendorVersion = vendorVersion.mid(idx + 1);
            if (!vendorVersion.isEmpty())
                m.insert(QStringLiteral("vendorVersion"), vendorVersion);
        }
    }

    static inline void parseGLESVersion(const char *version, QVariantMap &m)
    {
        auto rawVersion = QString::fromLocal8Bit(version);
        if (!rawVersion.startsWith(QLatin1String("OpenGL ES "))) {
            m.insert(QStringLiteral("version"), QStringLiteral("unknown"));
            m.insert(QStringLiteral("vendorVersion"), rawVersion);
        } else {
            rawVersion = rawVersion.mid(10);
            const auto idx = rawVersion.indexOf(QLatin1Char(' '));
            if (idx > 0) {
                const auto vendorVersion = rawVersion.mid(idx + 1);
                if (!vendorVersion.isEmpty())
                    m.insert(QStringLiteral("vendorVersion"), normalizeVendorString(vendorVersion));
                m.insert(QStringLiteral("version"), rawVersion.left(idx));
            } else {
                m.insert(QStringLiteral("version"), rawVersion);
            }
        }
    }

    static inline void parseGLSLVersion(const char *version, QVariantMap &m)
    {
        auto v = QString::fromLocal8Bit(version);
        const auto idx = v.indexOf(QLatin1Char(' '));
        if (idx > 0) {
            const auto glslVersion = v.left(idx);
            if (!glslVersion.isEmpty())
                m.insert(QStringLiteral("glslVersion"), glslVersion);
            else
                m.insert(QStringLiteral("glslVersion"), v);
        } else {
            m.insert(QStringLiteral("glslVersion"), v);
        }
    }

    static inline void parseESGLSLVersion(const char *version, QVariantMap &m)
    {
        auto v = QString::fromLocal8Bit(version);
        if (!v.startsWith(QLatin1String("OpenGL ES GLSL ES "))) {
            m.insert(QStringLiteral("glslVersion"), v);
        } else {
            v = v.mid(18);
            const auto idx = v.indexOf(QLatin1Char(' '));
            if (idx > 0) {
                const auto glslVersion = v.left(idx);
                if (!glslVersion.isEmpty())
                    m.insert(QStringLiteral("glslVersion"), glslVersion);
                else
                    m.insert(QStringLiteral("glslVersion"), v);
            } else {
                m.insert(QStringLiteral("glslVersion"), v);
            }
        }
    }

private:
    static inline QString normalizeVendorString(const QString &s)
    {
        if (!s.startsWith(QLatin1Char('(')) || !s.endsWith(QLatin1Char(')')))
            return s;
        return s.mid(1, s.size() - 2);
    }
};

}

#endif // USERFEEDBACK_OPENGLINFOSOURCE_P_H
