/*
    Copyright (C) 2017 Volker Krause <vkrause@kde.org>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef KUSERFEEDBACK_OPENGLINFOSOURCE_P_H
#define KUSERFEEDBACK_OPENGLINFOSOURCE_P_H

#include <QVariantMap>

namespace KUserFeedback {

// Internal functions made accessible for unit tests
class OpenGLInfoSourcePrivate
{
public:
    static inline void parseGLVersion(const char *version, QVariantMap &m)
    {
        auto vendorVersion = QString::fromLocal8Bit(version);
        const auto idx = vendorVersion.indexOf(QLatin1Char(' '));

        QString glVersion;
        if (idx > 0) {
            glVersion = vendorVersion.left(idx);
            vendorVersion = vendorVersion.mid(idx + 1);
            if (!vendorVersion.isEmpty())
                m.insert(QStringLiteral("vendorVersion"), normalizeVendorVersionString(vendorVersion));
        } else {
            glVersion = vendorVersion;
        }
        // in case glGetIntegerv(GL_MAJOR_VERSION) failed...
        if (!m.contains(QLatin1String("version")) && !glVersion.isEmpty())
            m.insert(QStringLiteral("version"), glVersion);
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
                    m.insert(QStringLiteral("vendorVersion"), normalizeVendorVersionString(vendorVersion));
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

    static inline QString normalizeVendor(const char *vendor)
    {
        const auto v = QString::fromLocal8Bit(vendor);
        if (v.startsWith(QLatin1String("Intel ")))
            return QStringLiteral("Intel");
        return v;
    }

    static inline QString normalizeRenderer(const char *renderer)
    {
        auto r = QString::fromLocal8Bit(renderer);
        // remove trademark indicators
        r.remove(QLatin1String("(R)"), Qt::CaseInsensitive);
        r.remove(QLatin1String("(TM)"), Qt::CaseInsensitive);

        // remove vendor prefixes, we already have that in the vendor field
        if (r.startsWith(QLatin1String("Mesa DRI ")))
            r = r.mid(9);
        if (r.startsWith(QLatin1String("Intel ")))
            r = r.mid(6);
        if (r.startsWith(QLatin1String("NVIDIA ")))
            r = r.mid(7);

        // remove excessive details that could enable fingerprinting
        if (r.startsWith(QLatin1String("ANGLE ")) || r.startsWith(QLatin1String("Gallium ")) || r.startsWith(QLatin1String("AMD ")))
            r = stripDetails(r);

        // strip macOS adding " OpenGL Engine" at the end
        if (r.endsWith(QLatin1String(" OpenGL Engine")))
            r = r.left(r.size() - 14);

        return r.simplified();
    }

private:
    static inline QString normalizeVendorVersionString(const QString &s)
    {
        if (s.startsWith(QLatin1Char('(')) && s.endsWith(QLatin1Char(')')))
            return s.mid(1, s.size() - 2);
        if (s.startsWith(QLatin1String("- ")))
            return s.mid(2);
        return s;
    }

    static inline QString stripDetails(const QString &s)
    {
        auto idx = s.indexOf(QLatin1String(" ("));
        if (idx < 1 || !s.endsWith(QLatin1Char(')')))
            return s;
        return s.left(idx);
    }
};

}

#endif // KUSERFEEDBACK_OPENGLINFOSOURCE_P_H
