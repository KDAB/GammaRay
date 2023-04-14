/*
  codecbrowser.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CODECBROWSER_CODECBROWSER_H
#define GAMMARAY_CODECBROWSER_CODECBROWSER_H

#include <core/toolfactory.h>

QT_BEGIN_NAMESPACE
class QItemSelection;
class QItemSelectionModel;
QT_END_NAMESPACE

namespace GammaRay {
class SelectedCodecsModel;

namespace Ui {
class CodecBrowser;
}

class CodecBrowser : public QObject
{
    Q_OBJECT
public:
    explicit CodecBrowser(Probe *probe, QObject *parent = nullptr);

public slots:
    void textChanged(const QString &text);

private slots:
    void updateCodecs(const QItemSelection &selected, const QItemSelection &deselected);

private:
    SelectedCodecsModel *m_selectedCodecsModel;
    QItemSelectionModel *m_codecSelectionModel;
};

class CodecBrowserFactory : public QObject, public StandardToolFactory<QObject, CodecBrowser>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_codecbrowser.json")
public:
    explicit CodecBrowserFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_CODECBROWSER_H
