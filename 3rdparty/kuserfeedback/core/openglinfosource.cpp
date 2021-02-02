/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "openglinfosource.h"
#include "openglinfosource_p.h"

#include <QVariant>
#ifndef QT_NO_OPENGL
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QSurfaceFormat>
#include <QWindow>
#endif

using namespace KUserFeedback;

OpenGLInfoSource::OpenGLInfoSource()
    : AbstractDataSource(QStringLiteral("opengl"), Provider::DetailedSystemInformation)
{
}

QString OpenGLInfoSource::description() const
{
    return tr("Information about type, version and vendor of the OpenGL stack.");
}

QVariant OpenGLInfoSource::data()
{
    QVariantMap m;

#ifndef QT_NO_OPENGL
    QOpenGLContext context;
    if (context.create()) {
        QWindow window;
        window.setSurfaceType(QSurface::OpenGLSurface);
        window.create();
        context.makeCurrent(&window);
        QOpenGLFunctions functions(&context);
        m.insert(QStringLiteral("vendor"), OpenGLInfoSourcePrivate::normalizeVendor(reinterpret_cast<const char*>(functions.glGetString(GL_VENDOR))));
        m.insert(QStringLiteral("renderer"), OpenGLInfoSourcePrivate::normalizeRenderer(reinterpret_cast<const char*>(functions.glGetString(GL_RENDERER))));

        switch (context.openGLModuleType()) {
            case QOpenGLContext::LibGL:
            {
                m.insert(QStringLiteral("type"), QStringLiteral("GL"));

#if defined(GL_MAJOR_VERSION) && defined(GL_MINOR_VERSION)
                int major = 0, minor = 0;
                functions.glGetIntegerv(GL_MAJOR_VERSION, &major);
                functions.glGetIntegerv(GL_MINOR_VERSION, &minor);
                // e.g. macOS legacy profiles return 0.0 here...
                if (major > 0)
                    m.insert(QStringLiteral("version"), QString(QString::number(major) + QLatin1Char('.') + QString::number(minor)));
#endif

                OpenGLInfoSourcePrivate::parseGLVersion(reinterpret_cast<const char*>(functions.glGetString(GL_VERSION)), m);
                OpenGLInfoSourcePrivate::parseGLSLVersion(reinterpret_cast<const char*>(functions.glGetString(GL_SHADING_LANGUAGE_VERSION)), m);
                break;
            }
            case QOpenGLContext::LibGLES:
            {
                m.insert(QStringLiteral("type"), QStringLiteral("GLES"));
                OpenGLInfoSourcePrivate::parseGLESVersion(reinterpret_cast<const char*>(functions.glGetString(GL_VERSION)), m);
                OpenGLInfoSourcePrivate::parseESGLSLVersion(reinterpret_cast<const char*>(functions.glGetString(GL_SHADING_LANGUAGE_VERSION)), m);
                break;
            }
        }

        switch (context.format().profile()) {
            case QSurfaceFormat::NoProfile:
                break;
            case QSurfaceFormat::CoreProfile:
                m.insert(QStringLiteral("profile"), QStringLiteral("core"));
                break;
            case QSurfaceFormat::CompatibilityProfile:
                m.insert(QStringLiteral("profile"), QStringLiteral("compat"));
                break;
        }

        return m;
    }
#endif

    m.insert(QStringLiteral("type"), QStringLiteral("none"));
    return m;
}

QString OpenGLInfoSource::name() const
{
    return tr("OpenGL information");
}
