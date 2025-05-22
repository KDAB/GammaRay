/*
  materialshadermodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "materialshadermodel.h"

#include <core/metaenum.h>

#include <QFile>
#include <QSGMaterial>
#include <private/qsgmaterialshader_p.h>

using namespace GammaRay;

MaterialShaderModel::MaterialShaderModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_shader(nullptr)
    , m_shaderFileCount(0)
{
}

MaterialShaderModel::~MaterialShaderModel() = default;

void MaterialShaderModel::setMaterialShader(QSGMaterialShader *shader)
{
    if (m_shader) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
        m_shader = nullptr;
        endRemoveRows();
    }

    if (shader) {
        m_shaderFileCount = shaderFileCount(shader);
        beginInsertRows(QModelIndex(), 0, m_shaderFileCount == 0 ? 1 : m_shaderFileCount - 1);
        m_shader = shader;
        endInsertRows();
    }
}

QByteArray MaterialShaderModel::shaderForRow(int row) const
{
    if (row < 0 || row >= rowCount() || !m_shader)
        return QByteArray();

    if (m_shaderFileCount == 0) {
        return QByteArray();
    }

    const auto fileName = shaderFileForRow(row);
    QFile shaderFile(fileName);
    if (!shaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return QByteArray();
    return shaderFile.readAll();
}

int MaterialShaderModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_shader)
        return 0;
    return m_shaderFileCount == 0 ? 2 : m_shaderFileCount;
}

QVariant MaterialShaderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_shader || role != Qt::DisplayRole)
        return QVariant();

    if (QSGMaterialShaderPrivate *p = QSGMaterialShaderPrivate::get(m_shader)) {
        int i = 0;
        const auto &shaderFiles = p->shaderFileNames;
        for (auto it = shaderFiles.cbegin(); it != shaderFiles.cend(); ++it) {
            auto type = it.key();
            if (i == index.row()) {
                switch (type) {
                case QShader::VertexStage:
                    return QString::fromLatin1("Vertex");
                case QShader::TessellationControlStage:
                    return QString::fromLatin1("TessellationControlStage");
                case QShader::TessellationEvaluationStage:
                    return QString::fromLatin1("TessellationEvaluationStage");
                case QShader::GeometryStage:
                    return QString::fromLatin1("GeometryStage");
                case QShader::FragmentStage:
                    return QString::fromLatin1("FragmentStage");
                case QShader::ComputeStage:
                    return QString::fromLatin1("ComputeStage");
                }
            }
            i++;
        }
    }

    return QVariant();
}

int MaterialShaderModel::shaderFileCount(QSGMaterialShader *shader)
{
    Q_ASSERT(shader);
    int fileCount = 0;
    if (QSGMaterialShaderPrivate *p = QSGMaterialShaderPrivate::get(shader)) {
        return p->shaderFileNames.size();
    }
    return fileCount;
}

QString MaterialShaderModel::shaderFileForRow(int row) const
{
    Q_ASSERT(m_shader);
    Q_ASSERT(m_shaderFileCount > 0);
    Q_ASSERT(row < m_shaderFileCount);

    if (QSGMaterialShaderPrivate *p = QSGMaterialShaderPrivate::get(m_shader)) {
        const auto &shaderFiles = p->shaderFileNames;
        int i = 0;
        for (const auto &sf : shaderFiles) {
            if (i == row)
                return sf;
            i++;
        }
        return {};
    }

    Q_ASSERT(false);
    return QString();
}
