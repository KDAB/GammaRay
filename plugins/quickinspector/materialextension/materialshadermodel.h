/*
  materialshadermodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_MATERIALSHADERMODEL_H
#define GAMMARAY_MATERIALSHADERMODEL_H

#include <QAbstractListModel>

QT_BEGIN_NAMESPACE
class QSGMaterialShader;
QT_END_NAMESPACE

namespace GammaRay {

class MaterialShaderModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MaterialShaderModel(QObject *parent = nullptr);
    ~MaterialShaderModel() override;

    void setMaterialShader(QSGMaterialShader *shader);
    QByteArray shaderForRow(int row) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    static int shaderFileCount(QSGMaterialShader *shader);
    QString shaderFileForRow(int row) const;

    QSGMaterialShader *m_shader;
    int m_shaderFileCount;
};
}

#endif // GAMMARAY_MATERIALSHADERMODEL_H
