/*
  guisupport.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_GUISUPPORT_H
#define GAMMARAY_GUISUPPORT_H

#include <core/toolfactory.h>

#include <QIcon>

QT_BEGIN_NAMESPACE
class QWindow;
QT_END_NAMESPACE

namespace GammaRay {
class GuiSupport : public QObject
{
    Q_OBJECT
public:
    explicit GuiSupport(ProbeInterface *probe, QObject *parent = nullptr);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    bool eventFilter(QObject *watched, QEvent *event);
#endif

private:
    void registerMetaTypes();
    void registerVariantHandler();
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    void discoverObjects();
    void updateWindowTitle(QWindow *w);
    QIcon createIcon(const QIcon &oldIcon, QWindow *w=nullptr);
    void updateWindowIcon(QWindow *w);
    void restoreWindowIcon(QWindow *w);
    void restoreIconAndTitle();

    QHash<QWindow*, QIcon> m_originalIcons;
    QIcon m_originalAppIcon;
    QString m_titleSuffix;
#endif

private:
    ProbeInterface *m_probe;

private slots:
#ifdef QT4_MOC_WORKAROUND
#else
    void objectCreated(QObject *object);
#endif
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
