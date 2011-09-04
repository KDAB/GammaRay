/*
  textdocumentinspector.h

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef ENDOSCOPE_TEXTDOCUMENTINSPECTOR_H
#define ENDOSCOPE_TEXTDOCUMENTINSPECTOR_H

#include <toolfactory.h>
#include <qwidget.h>
#include <qtextdocument.h>

class QItemSelection;

namespace Endoscope {

class TextDocumentModel;
class TextDocumentFormatModel;

namespace Ui { class TextDocumentInspector; }

class TextDocumentInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit TextDocumentInspector(ProbeInterface *probe, QWidget *parent = 0);

  private slots:
    void documentSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void documentElementSelected(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    QScopedPointer<Ui::TextDocumentInspector> ui;
    TextDocumentModel *m_textDocumentModel;
    TextDocumentFormatModel *m_textDocumentFormatModel;
};

class TextDocumentInspectorFactory :
    public QObject, public StandardToolFactory<QTextDocument,TextDocumentInspector>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolFactory)
  public:
    inline QString name() const { return tr("Text Documents"); }
};

}

#endif // ENDOSCOPE_TEXTDOCUMENTINSPECTOR_H
