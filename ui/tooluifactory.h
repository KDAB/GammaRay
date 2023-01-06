/*
  tooluifactory.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TOOLUIFACTORY_H
#define GAMMARAY_TOOLUIFACTORY_H

#include "gammaray_ui_export.h"

#include <QMetaType>
#include <QStringList>
#include <QtPlugin>

namespace GammaRay {
/*!
 * An abstract interface for creating the UI parts of probe tools.
 *
 * The ToolUiFactory class is an abstract base class for creating UIs for probe tools
 * for GammaRay. The unique identifier used for the UI must match the one of the corresponding
 * probe tool.
 */
class GAMMARAY_UI_EXPORT ToolUiFactory
{
public:
    ToolUiFactory();
    virtual ~ToolUiFactory();

    /*!
     * Unique id of this tool, must match the id of a the corresponding probe tool.
     * @return a QString containing the tool id.
     */
    virtual QString id() const = 0;

    /*!
     * Human readable name of this tool.
     * You do not need to override this usually, the plugin loader will fill this in.
     * @return a QString containing the tool name.
     */
    virtual QString name() const;

    /*!
     * Return @c true if this tool supports remoting, @c false otherwise.
     * The default implementation returns @c true.
     */
    virtual bool remotingSupported() const;

    /*!
     * Create the UI part of this tool.
     * @param parentWidget The parent widget for the visual elements of this tool.
     * @return a pointer to the created QwWidget.
     */
    virtual QWidget *createWidget(QWidget *parentWidget) = 0;

    /*!
     * Initialize UI related stuff for this tool. This function is called on loading
     * the plugin, before the widget itself is needed. Use createWidget to create
     * the actual widget.
     */
    virtual void initUi();

private:
    Q_DISABLE_COPY(ToolUiFactory)
};

/*!
 * A templated convenience ToolUiFactory applicable for most use-cases.
 */
template<typename ToolUi>
class StandardToolUiFactory : public ToolUiFactory
{
public:
    QString id() const override
    {
        return QString(); // TODO is this a problem??
    }

    QWidget *createWidget(QWidget *parentWidget) override
    {
        return new ToolUi(parentWidget);
    }
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::ToolUiFactory, "com.kdab.GammaRay.ToolUiFactory/1.0")
QT_END_NAMESPACE
Q_DECLARE_METATYPE(GammaRay::ToolUiFactory *)

#endif
