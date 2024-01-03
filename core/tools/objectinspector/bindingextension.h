/*
  bindingextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_BINDINGEXTENSION_H
#define GAMMARAY_BINDINGEXTENSION_H

// Own
#include <core/propertycontrollerextension.h>
#include "gammaray_core_export.h"

// Qt
#include <QObject>
#include <QPointer>

// Std
#include <memory>
#include <vector>

namespace GammaRay {

class BindingModel;
class BindingNode;

class GAMMARAY_CORE_EXPORT BindingExtension : public QObject, public PropertyControllerExtension
{
    Q_OBJECT
public:
    explicit BindingExtension(PropertyController *controller);
    ~BindingExtension() override;

    bool setQObject(QObject *object) override;

    BindingModel *model() const;

private slots:
    void propertyChanged();
    void clear();

private:
    QPointer<QObject> m_object;
    std::vector<std::unique_ptr<BindingNode>> m_bindings;

    BindingModel *m_bindingModel;
};
}

#endif // GAMMARAY_BINDINGEXTENSION_H
