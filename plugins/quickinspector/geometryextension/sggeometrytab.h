/*
  sggeometrytab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_SGGEOMETRYTAB_H
#define GAMMARAY_QUICKINSPECTOR_SGGEOMETRYTAB_H

#include <QWidget>
#include <QModelIndex>
#include <memory>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
QT_END_NAMESPACE

namespace GammaRay {
class Ui_SGGeometryTab;
class PropertyWidget;

class SGGeometryTab : public QWidget
{
    Q_OBJECT

public:
    explicit SGGeometryTab(PropertyWidget *parent);
    ~SGGeometryTab() override;

private:
    void setObjectBaseName(const QString &baseName);

private:
    std::unique_ptr<Ui_SGGeometryTab> m_ui;
    QAbstractItemModel *m_vertexModel;
    QAbstractItemModel *m_adjacencyModel;
};
}

#endif // SGGEOMETRYTAB_H
