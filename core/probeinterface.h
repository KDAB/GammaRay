/*
  probeinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
/**
  @file
  This file is part of the GammaRay Plugin API and declares the ProbeInterface abstract base class.

  @brief
  Declares the ProbeInterface abstract base class.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_PROBEINTERFACE_H
#define GAMMARAY_PROBEINTERFACE_H

#include <QPoint>

QT_BEGIN_NAMESPACE
class QObject;
class QAbstractItemModel;
class QString;
QT_END_NAMESPACE

namespace GammaRay {
struct SignalSpyCallbackSet;

/**
 * @brief An abstract interface for accessing the core GammaRay probe.
 *
 * The ProbeInterface is an abstract interface that allows one to access
 * the core GammaRay probe without linking to it.
 */
class ProbeInterface
{
public:
    ProbeInterface() {}
    virtual inline ~ProbeInterface()
    {
    }

    /**
     * Returns the object list model.
     * @return a pointer to a QAbstractItemModel instance.
     */
    virtual QAbstractItemModel *objectListModel() const = 0;

    /**
     * Returns the object tree model.
     * @return a pointer to a QAbstractItemModel instance.
     */
    virtual QAbstractItemModel *objectTreeModel() const = 0;

    /**
     * Determines if the specified QObject belongs to the GammaRay Probe or Window.
     *
     * These objects should not be tracked or shown to the user,
     * hence must be explictly filtered.
     * @param object is a pointer to a QObject instance.
     *
     * @return true if the specified QObject belongs to the GammaRay Probe
     * or Window; false otherwise.
     */
    virtual bool filterObject(QObject *object) const = 0;

    /**
     * Returns the probe QObject for connecting signals.
     * @return a pointer to a QObject instance.
     */
    virtual QObject *probe() const = 0;

    /**
     * Register a model for remote usage.
     * @param objectName unique identifier for the model, typically in reverse domain notation.
     */
    virtual void registerModel(const QString &objectName, QAbstractItemModel *model) = 0;

    /**
     * Install a global event filter.
     * Use this rather than installing the filter manually on QCoreApplication,
     * this will filter out GammaRay-internal events and objects already for you.
     */
    virtual void installGlobalEventFilter(QObject *filter) = 0;

    /**
     * Returns @c true if we haven't been able to track all objects from startup, ie. usually
     * when attaching at runtime.
     * If this is the case, we try to discover QObjects by walking the hierarchy, starting
     * from known singletons, and by watching out for unknown receivers of events.
     * This is far from complete obviously, and plug-ins can help finding more objects, using
     * specific knowledge about the types they are responsible for.
     *
     * Connect to the objectAdded(QObject*) signal on probe(), and call discoverObject(QObject*)
     * for "your" objects.
     *
     * @since 2.5
     */
    virtual bool needsObjectDiscovery() const = 0;

    /**
     * Notify the probe about QObjects your plug-in can discover by using information about
     * the types it can handle.
     * Only use this if needsObjectDiscovery() returns @c true to maximise performance.
     *
     * @see needsObjectDiscovery()
     * @since 2.0
     */
    virtual void discoverObject(QObject *object) = 0;

    /**
     * Notify the probe about the user selecting one of "your" objects via in-app interaction.
     * If you know the exact position the user interacted with, pass that in as @p pos.
     *
     * @since 2.0
     */
    virtual void selectObject(QObject *object, const QPoint &pos = QPoint()) = 0;

    virtual void selectObject(QObject *object, const QString &toolId,
                              const QPoint &pos = QPoint()) = 0;

    /**
     * Notify the probe about the user selecting one of "your" objects property via in-app interaction.
     *
     * @since 3.0
     */
    virtual void selectObjectProperty(QObject *object, const QByteArray &property) = 0;
    virtual void selectObjectProperty(QObject *object, const QByteArray &property, const QString &toolId) = 0;

    /**
     * Notify the probe about the user selecting one of "your" objects.
     *
     * @since 2.1
     */
    virtual void selectObject(void *object, const QString &typeName) = 0;

    /**
     * Register a signal spy callback set.
     * Signal indexes provided as arguments are mapped to method indexes, ie. argument semantics
     * are the same with Qt4 and Qt5.
     *
     * @since 2.2
     */
    virtual void registerSignalSpyCallbackSet(const SignalSpyCallbackSet &callbacks) = 0;

private:
    Q_DISABLE_COPY(ProbeInterface)
};
}

#endif
