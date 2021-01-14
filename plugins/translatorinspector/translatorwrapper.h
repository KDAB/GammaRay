/*
  translatorwrapper.h

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

#ifndef TRANSLATORWRAPPER_H
#define TRANSLATORWRAPPER_H

#include <common/modelroles.h>

#include <QAbstractItemModel>
#include <QTranslator>

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class TranslatorWrapper;

class TranslationsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum CustomRoles {
        IsOverriddenRole = GammaRay::UserRole
    };

    explicit TranslationsModel(TranslatorWrapper *translator);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

    void resetTranslations(const QItemSelection &selection);
    QString translation(const char *context, const char *sourceText, const char *disambiguation,
                        const int n, const QString &default_);

    void resetAllUnchanged();

    TranslatorWrapper *translator() const
    {
        return m_translator;
    }

signals:
    void rowCountChanged();

private:
    friend class TranslatorWrapper;
    TranslatorWrapper *m_translator;

    struct Row
    {
        Row() = default;
        QByteArray context;
        QByteArray sourceText;
        QByteArray disambiguation;
        QString translation;
        bool isOverridden = false;
    };
    QVector<Row> m_nodes;

    QModelIndex findNode(const char *context, const char *sourceText, const char *disambiguation,
                         const int n, const bool create);
    void setTranslation(const QModelIndex &index, const QString &translation);
};

class TranslatorWrapper : public QTranslator
{
    Q_OBJECT

public:
    explicit TranslatorWrapper(QTranslator *wrapped, QObject *parent = nullptr);

    TranslationsModel *model() const { return m_model; }

    bool isEmpty() const override;
    QString translate(const char *context, const char *sourceText, const char *disambiguation,
                      int n) const override;
    QTranslator *translator() const;

private:
    QTranslator *m_wrapped;
    TranslationsModel *m_model;

    QString translateInternal(const char *context, const char *sourceText,
                              const char *disambiguation, int n) const;
};

class FallbackTranslator : public QTranslator
{
    Q_OBJECT

public:
    explicit FallbackTranslator(QObject *parent = nullptr);

    bool isEmpty() const override
    {
        return false;
    }

    QString translate(const char *context, const char *sourceText, const char *disambiguation,
                      int n) const override;

private:
#ifndef Q_NO_USING_KEYWORD
    // hide
    using QTranslator::load;
#endif
};
}

#endif
