/*
  materialshadermodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "materialshadermodel.h"

#include <core/metaenum.h>

#include <QFile>
#include <QSGMaterial>
#include <private/qsgmaterialshader_p.h>

using namespace GammaRay;

#define S(x) { QOpenGLShader:: x, #x }
static const MetaEnum::Value<QOpenGLShader::ShaderTypeBit> qopengl_shader_type[] = {
    S(Vertex),
    S(Fragment),
    S(Geometry),
    S(TessellationControl),
    S(TessellationEvaluation),
    S(Compute)
};
#undef S

class SGMaterialShaderThief : public QSGMaterialShader
{
public:
    using QSGMaterialShader::vertexShader;
    using QSGMaterialShader::fragmentShader;

    const QHash<QOpenGLShader::ShaderType, QStringList>& getShaderSources()
    {
        return d_func()->m_sourceFiles;
    }
};



MaterialShaderModel::MaterialShaderModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_shader(nullptr)
    , m_shaderFileCount(0)
{
}

MaterialShaderModel::~MaterialShaderModel() = default;

void MaterialShaderModel::setMaterialShader(QSGMaterialShader* shader)
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
        switch (row) {
            case 0:
                return reinterpret_cast<SGMaterialShaderThief*>(m_shader)->vertexShader();
            case 1:
                return reinterpret_cast<SGMaterialShaderThief*>(m_shader)->fragmentShader();
        }
        return QByteArray();
    }

    const auto fileName = shaderFileForRow(row);
    QFile shaderFile(fileName);
    if (!shaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return QByteArray();
    return shaderFile.readAll();
}

int MaterialShaderModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !m_shader)
        return 0;
    return m_shaderFileCount == 0 ? 2 : m_shaderFileCount;
}

QVariant MaterialShaderModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_shader || role != Qt::DisplayRole)
        return QVariant();

    if (m_shaderFileCount > 0) {
        const auto &files = reinterpret_cast<SGMaterialShaderThief*>(m_shader)->getShaderSources();
        int idx = index.row();
        for (auto it = files.begin(); it != files.end(); ++it) {
            if (idx < it.value().size())
                return QString(MetaEnum::flagsToString(it.key(), qopengl_shader_type) + QLatin1String(": ") + it.value().at(idx));
            idx -= it.value().size();
        }
        Q_ASSERT(false);
    } else {
        return MetaEnum::flagsToString((1 << index.row()), qopengl_shader_type);
    }

    return QVariant();
}

int MaterialShaderModel::shaderFileCount(QSGMaterialShader *shader)
{
    Q_ASSERT(shader);
    const auto &files = reinterpret_cast<SGMaterialShaderThief*>(shader)->getShaderSources();
    int fileCount = 0;
    for (auto it = files.begin(); it != files.end(); ++it)
        fileCount += it.value().size();
    return fileCount;
}

QString MaterialShaderModel::shaderFileForRow(int row) const
{
    Q_ASSERT(m_shader);
    Q_ASSERT(m_shaderFileCount > 0);
    Q_ASSERT(row < m_shaderFileCount);

    const auto &files = reinterpret_cast<SGMaterialShaderThief*>(m_shader)->getShaderSources();
    for (auto it = files.begin(); it != files.end(); ++it) {
        if (row < it.value().size())
            return it.value().at(row);
        row -= it.value().size();
    }

    Q_ASSERT(false);
    return QString();
}
