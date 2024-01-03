/*
  abstractbindingprovider.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_ABSTRACTBINDINGPROVIDER_H
#define GAMMARAY_ABSTRACTBINDINGPROVIDER_H

// Own
#include "gammaray_core_export.h"

// Qt
#include <QString>

// Std
#include <memory>
#include <vector>

QT_BEGIN_NAMESPACE
class QObject;
QT_END_NAMESPACE

namespace GammaRay {

class BindingNode;

class GAMMARAY_CORE_EXPORT AbstractBindingProvider
{
public:
    AbstractBindingProvider();
    virtual ~AbstractBindingProvider();
    virtual std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(BindingNode *binding) const = 0;
    virtual std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject *obj) const = 0;
    virtual bool canProvideBindingsFor(QObject *object) const = 0;

private:
    Q_DISABLE_COPY(AbstractBindingProvider)
};

}

#endif // GAMMARAY_ABSTRACTBINDINGPROVIDER_H
