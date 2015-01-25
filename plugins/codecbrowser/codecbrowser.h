/*
  codecbrowser.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#ifndef GAMMARAY_CODECBROWSER_CODECBROWSER_H
#define GAMMARAY_CODECBROWSER_CODECBROWSER_H

#include <core/toolfactory.h>

class QItemSelection;
class QItemSelectionModel;

namespace GammaRay {

class SelectedCodecsModel;

namespace Ui {
  class CodecBrowser;
}

class CodecBrowser : public QObject
{
  Q_OBJECT
  public:
    explicit CodecBrowser(ProbeInterface *probe, QObject *parent = 0);

  public slots:
    void textChanged(const QString &text);

  private slots:
    void updateCodecs(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    SelectedCodecsModel *m_selectedCodecsModel;
    QItemSelectionModel *m_codecSelectionModel;
};

class CodecBrowserFactory : public QObject, public StandardToolFactory<QObject, CodecBrowser>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.CodecBrowser")
  public:
    explicit CodecBrowserFactory(QObject *parent = 0) : QObject(parent)
    {
    }

    virtual inline QString name() const
    {
      return tr("Text Codecs");
    }
};

}

#endif // GAMMARAY_CODECBROWSER_H
