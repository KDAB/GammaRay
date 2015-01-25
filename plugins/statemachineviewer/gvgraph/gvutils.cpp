/*
  gvutils.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "gvutils.h"

#include <graphviz/gvc.h>
#ifdef WITH_CGRAPH
#  include <graphviz/cgraph.h>
#else
#  include <graphviz/graph.h>
#endif

using namespace GammaRay;

#ifdef WITH_CGRAPH
Agraph_t *GVUtils::_agopen(const QString &name, Agdesc_t kind, Agdisc_t *disc)
#else
Agraph_t *GVUtils::_agopen(const QString &name, int kind)
#endif
{
#ifdef WITH_CGRAPH
  return agopen(const_cast<char *>(qPrintable(name)), kind, disc);
#else
  return agopen(const_cast<char *>(qPrintable(name)), kind);
#endif
}

QString GVUtils::_agget(void* object, const QString &attr, const QString &alt)
{
  const QString str = agget(object, const_cast<char *>(qPrintable(attr)));
  if(str.isEmpty()) {
    return alt;
  } else {
    return str;
  }
}

Agsym_t* GVUtils::_agnodeattr(Agraph_t *object, const QString &attr, const QString &alt)
{
#ifdef WITH_CGRAPH
  return agattr(object, AGNODE,
#else
  return agnodeattr(object,
#endif
                const_cast<char *>(qPrintable(attr)),
                const_cast<char *>(qPrintable(alt)));
}

Agsym_t* GVUtils::_agedgeattr(Agraph_t *object, const QString &attr, const QString &alt)
{
#ifdef WITH_CGRAPH
  return agattr(object, AGEDGE,
#else
  return agedgeattr(object,
#endif
                const_cast<char *>(qPrintable(attr)),
                const_cast<char *>(qPrintable(alt)));
}

Agnode_t* GVUtils::_agnode(Agraph_t* graph, const QString& attr, bool create)
{
#ifdef WITH_CGRAPH
  return agnode(graph, const_cast<char*>(qPrintable(attr)), create);
#else
  Q_UNUSED(create);
  return agnode(graph, const_cast<char*>(qPrintable(attr)));
#endif
}

Agedge_t* GVUtils::_agedge(Agraph_t *graph, Agnode_t *tail, Agnode_t *head,
                           const QString &name, bool create)
{
#ifdef WITH_CGRAPH
  return agedge(graph, tail, head, const_cast<char*>(qPrintable(name)), create);
#else
  Q_UNUSED(name);
  Q_UNUSED(create);
  return agedge(graph, tail, head);
#endif
}

Agraph_t* GVUtils::_agsubg(Agraph_t *graph, const QString &attr, bool create)
{
#ifdef WITH_CGRAPH
  return agsubg(graph, const_cast<char*>(qPrintable(attr)), create);
#else
  Q_UNUSED(create);
  return agsubg(graph, const_cast<char*>(qPrintable(attr)));
#endif
}

int GVUtils::_agset(void *object, const QString &attr, const QString &value)
{
  return agsafeset(object, const_cast<char *>(qPrintable(attr)),
                   const_cast<char *>(qPrintable(value)), const_cast<char *>(""));
}

int GVUtils::_gvLayout(GVC_t* gvc, graph_t* g, const char* engine)
{
  return gvLayout(gvc, g, const_cast<char*>(engine));
}
