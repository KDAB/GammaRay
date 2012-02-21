/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_GVUTILS_H
#define GAMMARAY_GVUTILS_H

#include <graphviz/types.h>
#include <graphviz/graph.h>
#include <graphviz/gvc.h>

#include <QString>
#include <qglobal.h>

namespace GammaRay {

/// The agopen method for opening a graph
static inline Agraph_t *_agopen(QString name, int kind)
{
  return agopen(const_cast<char *>(qPrintable(name)), kind);
}

/// Add an alternative value parameter to the method for getting an object's attribute
static inline QString _agget(void *object, QString attr, QString alt=QString())
{
  const QString str = agget(object, const_cast<char *>(qPrintable(attr)));
  if(str.isEmpty()) {
    return alt;
  } else {
    return str;
  }
}

static inline Agsym_t *_agnodeattr(Agraph_t *object, QString attr, QString alt=QString())
{
  return agnodeattr(object,
                    const_cast<char *>(qPrintable(attr)),
                    const_cast<char *>(qPrintable(alt)));
}

static inline Agsym_t *_agedgeattr(Agraph_t *object, QString attr, QString alt=QString())
{
  return agedgeattr(object,
                    const_cast<char *>(qPrintable(attr)),
                    const_cast<char *>(qPrintable(alt)));
}

static inline int _gvLayout(GVC_t *gvc, graph_t *g, const char *engine)
{
  return gvLayout(gvc, g, engine);
}

static inline Agnode_t *_agnode(Agraph_t *graph, const QString &attr)
{
  return agnode(graph, const_cast<char*>(qPrintable(attr)));
}

static inline Agraph_t *_agsubg(Agraph_t *graph, const QString &attr)
{
  return agsubg(graph, const_cast<char*>(qPrintable(attr)));
}

/// Directly use agsafeset which always works, contrarily to agset
static inline int _agset(void *object, QString attr, QString value)
{
  return agsafeset(object, const_cast<char *>(qPrintable(attr)),
                   const_cast<char *>(qPrintable(value)), const_cast<char *>(""));
}

}

#endif
