/*
  guisupport.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_GUISUPPORT_H
#define GAMMARAY_GUISUPPORT_H

#include <core/toolfactory.h>

#include <QIcon>
#include <QSet>

QT_BEGIN_NAMESPACE
class QWindow;
QT_END_NAMESPACE

namespace GammaRay {
class GuiSupport : public QObject
{
    Q_OBJECT
public:
    explicit GuiSupport(Probe *probe, QObject *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    static void registerMetaTypes();
    static void registerVariantHandler();
    void discoverObjects();
    static QObject *targetObject(QObject *object);
    QIcon createIcon(const QIcon &oldIcon, QWindow *w = nullptr);
    void updateWindowIcon(QWindow *w = nullptr);
    void updateWindowTitle(QWindow *w);
    void restoreWindowIcon(QWindow *w = nullptr);
    void restoreWindowTitle(QWindow *w);

    struct IconAndTitleOverriderData
    {
        struct Icons
        {
            explicit Icons(const QIcon &originalIcon = QIcon(),
                           const QIcon &gammarayIcon = QIcon())
                : originalIcon(originalIcon)
                , gammarayIcon(gammarayIcon)
            {
            }

            QIcon originalIcon;
            QIcon gammarayIcon;
        };

        QSet<QObject *> updatingObjectsIcon;
        QSet<QObject *> updatingObjectsTitle;
        QHash<QObject *, Icons> objectsIcons;
        QString titleSuffix;
    } m_iconAndTitleOverrider;

private:
    Probe *m_probe;

private slots:
    void objectCreated(QObject *object);
    void restoreIconAndTitle();
};

class GuiSupportFactory : public QObject, public StandardToolFactory<QObject, GuiSupport>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_guisupport.json")
public:
    explicit GuiSupportFactory(QObject *parent = nullptr);
};
}

#endif // GAMMARAY_GUISUPPORT_H
