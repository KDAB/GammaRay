/*
  translatorinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

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

#ifndef TRANSLATORINSPECTOR_H
#define TRANSLATORINSPECTOR_H

#include "translatorinspectorinterface.h"

#include <core/toolfactory.h>

#include <QAbstractItemModel>
#include <QTranslator>

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
class QItemSelection;
class QAbstractProxyModel;
QT_END_NAMESPACE

namespace GammaRay {
class TranslatorsModel;
class TranslatorWrapper;
class FallbackTranslator;

class TranslatorInspector : public TranslatorInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::TranslatorInspectorInterface)

public:
    explicit TranslatorInspector(Probe *probe, QObject *parent = nullptr);

public slots:
    void sendLanguageChangeEvent() override;
    void resetTranslations() override;

private slots:
    void selectionChanged(const QItemSelection &selection);
    void objectSelected(QObject *obj);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    void registerMetaTypes();

    QItemSelectionModel *m_selectionModel;
    QItemSelectionModel *m_translationsSelectionModel;
    TranslatorsModel *m_translatorsModel;
    QAbstractProxyModel *m_translationsModel;
    Probe *m_probe;
    TranslatorWrapper *m_fallbackWrapper;
};

class TranslatorInspectorFactory : public QObject,
    public StandardToolFactory<QTranslator, TranslatorInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_translatorinspector.json")

public:
    explicit TranslatorInspectorFactory(QObject *parent = nullptr)
        : QObject(parent) {}
};
}

#endif
