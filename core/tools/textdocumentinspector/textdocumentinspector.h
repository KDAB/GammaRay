/*
  textdocumentinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTINSPECTOR_H
#define GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTINSPECTOR_H

#include "toolfactory.h"

#include <QPointer>
#include <QTextDocument>

class QItemSelection;

namespace GammaRay {

class TextDocumentModel;
class TextDocumentFormatModel;

class TextDocumentInspector : public QObject
{
  Q_OBJECT
  public:
    explicit TextDocumentInspector(ProbeInterface *probe, QObject *parent = 0);

  private slots:
    void documentSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void documentElementSelected(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    TextDocumentModel *m_textDocumentModel;
    TextDocumentFormatModel *m_textDocumentFormatModel;
};

class TextDocumentInspectorFactory
  : public QObject, public StandardToolFactory<QTextDocument,TextDocumentInspector>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit TextDocumentInspectorFactory(QObject *parent) : QObject(parent)
    {
    }

    inline QString name() const
    {
      return tr("Text Documents");
    }
};

}

#endif // GAMMARAY_TEXTDOCUMENTINSPECTOR_H
