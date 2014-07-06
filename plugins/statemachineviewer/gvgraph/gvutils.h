/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_STATEMACHINEVIEWER_GVUTILS_H
#define GAMMARAY_STATEMACHINEVIEWER_GVUTILS_H

#include <QString>

#include <graphviz/types.h> //must include after QString to avoid possible
                            //conflicting declarations for strcmp(), memcmp()...

namespace GammaRay {

namespace GVUtils {

/// The agopen method for opening a graph
#ifdef WITH_CGRAPH
extern Agraph_t *_agopen(const QString &name, Agdesc_t kind, Agdisc_t *disc);
#else
extern Agraph_t *_agopen(const QString &name, int kind);
#endif

/// Add an alternative value parameter to the method for getting an object's attribute
QString _agget(void *object, const QString &attr, const QString& alt = QString());

Agsym_t *_agnodeattr(Agraph_t *object, const QString &attr,
                     const QString &alt = QString());
Agsym_t *_agedgeattr(Agraph_t *object, const QString &attr,
                     const QString &alt = QString());

int _gvLayout(GVC_t *gvc, graph_t *g, const char *engine);

Agnode_t *_agnode(Agraph_t *graph, const QString &attr, bool create = true);
Agedge_t *_agedge(Agraph_t *graph, Agnode_t *tail, Agnode_t *head,
                  const QString &name = QString(), bool create = true);
Agraph_t *_agsubg(Agraph_t *graph, const QString &attr, bool create = true);

/// Directly use agsafeset which always works, contrarily to agset
int _agset(void* object, const QString& attr, const QString& value);

}

}

#endif
