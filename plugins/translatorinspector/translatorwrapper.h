/*
  translatorwrapper.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

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

#include <QAbstractItemModel>
#include <QTranslator>

namespace GammaRay {
class TranslatorWrapper;

class TranslationsModel : public QAbstractListModel
{
  Q_OBJECT

  public:
    explicit TranslationsModel(TranslatorWrapper *translator);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role) Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    void resetTranslations(const QModelIndex &first, const QModelIndex &last);
    QString translation(const QByteArray &context, const QByteArray &sourceText,
                        const QByteArray &disambiguation, const int n,
                        const QString &default_);

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
      Row() : isOverriden(false) {}
      QByteArray context;
      QByteArray sourceText;
      QByteArray disambiguation;
      QString translation;
      bool isOverriden;
    };
    QVector<Row> m_nodes;

    QModelIndex findNode(const QByteArray &context, const QByteArray &sourceText,
                         const QByteArray &disambiguation, const int n,
                         const bool create);
    void setTranslation(const QModelIndex &index, const QString &translation);
};

class TranslatorWrapper : public QTranslator
{
  Q_OBJECT

  public:
    explicit TranslatorWrapper(QObject *parent = 0);
    explicit TranslatorWrapper(QTranslator *wrapped, QObject *parent = 0);

    TranslationsModel *model() const { return m_model; }

    bool isEmpty() const Q_DECL_OVERRIDE;
    QString translate(const char *context, const char *sourceText,
                      const char *disambiguation, int n) const Q_DECL_OVERRIDE;
    const QTranslator *translator() const;

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
    explicit FallbackTranslator(QObject *parent = 0);

    bool isEmpty() const Q_DECL_OVERRIDE
    {
      return false;
    }

    QString translate(const char *context, const char *sourceText,
                      const char *disambiguation, int n) const Q_DECL_OVERRIDE;

  private:
#ifndef Q_NO_USING_KEYWORD
    // hide
    using QTranslator::load;
#endif
};

}

#endif
