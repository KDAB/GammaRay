/*
  materialshadermodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

#if QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)
#define S(x)                 \
    {                        \
        QOpenGLShader::x, #x \
    }
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
    using QSGMaterialShader::fragmentShader;
    using QSGMaterialShader::vertexShader;

    const QHash<QOpenGLShader::ShaderType, QStringList> &getShaderSources()
    {
        return d_func()->m_sourceFiles;
    }
};
#endif // QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)

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
        switch (row) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        case 0:
            return reinterpret_cast<SGMaterialShaderThief *>(m_shader)->vertexShader();
        case 1:
            return reinterpret_cast<SGMaterialShaderThief *>(m_shader)->fragmentShader();
#endif
        }
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (QSGMaterialShaderPrivate *p = QSGMaterialShaderPrivate::get(m_shader)) {
        int i = 0;
        const auto &shaderFiles = p->shaderFileNames;
        for (const auto &[type, _] : shaderFiles.asKeyValueRange()) {
            if (i == index.row()) {
                switch (type) {
                case QShader::Stage::VertexStage:
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
#else
    if (m_shaderFileCount > 0) {
        const auto &files = reinterpret_cast<SGMaterialShaderThief *>(m_shader)->getShaderSources();
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
#endif

    return QVariant();
}

int MaterialShaderModel::shaderFileCount(QSGMaterialShader *shader)
{
    Q_ASSERT(shader);
    int fileCount = 0;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (QSGMaterialShaderPrivate *p = QSGMaterialShaderPrivate::get(shader)) {
        return p->shaderFileNames.size();
    }
#else
    const auto &files = reinterpret_cast<SGMaterialShaderThief *>(shader)->getShaderSources();
    for (auto it = files.begin(); it != files.end(); ++it)
        fileCount += it.value().size();
#endif
    return fileCount;
}

QString MaterialShaderModel::shaderFileForRow(int row) const
{
    Q_ASSERT(m_shader);
    Q_ASSERT(m_shaderFileCount > 0);
    Q_ASSERT(row < m_shaderFileCount);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
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
#else
    const auto &files = reinterpret_cast<SGMaterialShaderThief *>(m_shader)->getShaderSources();
    for (auto it = files.begin(); it != files.end(); ++it) {
        if (row < it.value().size())
            return it.value().at(row);
        row -= it.value().size();
    }
#endif

    Q_ASSERT(false);
    return QString();
}
