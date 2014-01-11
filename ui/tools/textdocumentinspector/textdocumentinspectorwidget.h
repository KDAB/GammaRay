/*
  textdocumentinspectorwidget.h

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

#ifndef GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTINSPECTORWIDGET_H
#define GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTINSPECTORWIDGET_H

#include <QPointer>
#include <QTextDocument>
#include <QWidget>

class QItemSelection;

namespace GammaRay {

class TextDocumentModel;
class TextDocumentFormatModel;

namespace Ui {
  class TextDocumentInspectorWidget;
}

class TextDocumentInspectorWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit TextDocumentInspectorWidget(QWidget *parent = 0);
    ~TextDocumentInspectorWidget();

  private slots:
    void documentSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void documentElementSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void documentContentChanged();

  private:
    QScopedPointer<Ui::TextDocumentInspectorWidget> ui;
    QPointer<QTextDocument> m_currentDocument;
};

}

#endif // GAMMARAY_TEXTDOCUMENTINSPECTOR_H
