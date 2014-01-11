/*
  mimetypes.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MIMETYPES_H
#define GAMMARAY_MIMETYPES_H

#include <core/toolfactory.h>

#include <QMimeData>
#include <QMimeDatabase>

class QStandardItemModel;
class QStandardItem;

namespace GammaRay {

class MimeTypes : public QObject
{
  Q_OBJECT
  public:
    explicit MimeTypes(ProbeInterface *probe, QObject *parent = 0);
    ~MimeTypes();

  private:
    QStandardItemModel *m_model;
};

class MimeTypesFactory : public QObject, public StandardToolFactory<QObject, MimeTypes>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit MimeTypesFactory(QObject *parent) : QObject(parent)
    {
    }

    virtual inline QString name() const
    {
      return tr("Mime Types");
    }
};

}

#endif // GAMMARAY_MIMETYPES_H
