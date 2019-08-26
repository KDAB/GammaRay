# <a name="title"></a> [![Logo](ui/resources/gammaray/ui/light/pixmaps/gammaray-trademark.png)](http://www.kdab.com/gammaray)

Introduction [![Packaging status](https://repology.org/badge/tiny-repos/gammaray.svg)](https://repology.org/metapackage/gammaray) [![Windows Build status](https://ci.appveyor.com/api/projects/status/bff5abu59vm3i73x/branch/master?svg=true)](https://ci.appveyor.com/project/KDAB/gammaray/branch/master)
=======

GammaRay is a software introspection tool for Qt applications developed by KDAB.
Leveraging the QObject introspection mechanism it allows you to observe and
manipulate your application at runtime. This works both locally on your
workstation and remotely on an embedded target.

Augmenting your instruction-level debugger, GammaRay allows you to work on a
much higher level, with the same concepts as the frameworks you use. This is
especially useful for the more complex Qt frameworks such as model/view, state
machines or scene graphs.

Among other things GammaRay can:

 * Browse the QObject tree with live updates.
 * View and edit object properties.
 * View and invoke slots of a QObject, and monitor its signals.
 * List all QObject inbound and outbound signal/slot connections.
 * Provide a layout information overlay for QWidget and QtQuick2 applications.
 * Inspect all QPainter operations used to draw a specific widget.
 * Browse the QtQuick2 item tree and scenegraph.
 * Inspect shaders and geometry data of QtQuick2 items.
 * Plot object lifetime and emitted signals.
 * Browse the QAbstractProxyModel hierarchy and inspect intermediate
   results in a proxy model chain.
 * Visual live inspection of QStateMachines.
 * Browse the item tree of any QGraphicsView scene.
 * Show a live preview of QGraphicsView items, including showing their coordinate system,
   transformation origin, rotate/zoom/pan, etc.
 * Intercept translations and change them at runtime.
 * Inspect all building blocks of a QStyle.
 * Show all QTimers and their statistics (number of wakeups, wakeup time, ...)
 * Browse all QTextDocuments, along with the ability to edit them and view
   their internal structures.
 * Act as a complete java script debugger, attachable to any QScriptEngine
  (including the usually not accessible one used by QtQuick1 internally).
 * Perform HTML/CSS/DOM/JS introspection/editing/profiling on any QWebPage,
   thanks to QWebInspector.
 * Browse the QResource tree and its content.
 * Show all registered meta types.
 * Show all installed fonts.
 * Show all available codecs.

Contact
=======
* Feel free to visit us on IRC: Channel is #gammaray on Freenode (irc://irc.freenode.net/gammaray)
* Or send mail to our mailing list: https://mail.kdab.com/mailman/listinfo/gammaray-interest

Get Involved
============
If you want to contribute, please check out: https://github.com/KDAB/GammaRay/wiki/Get-Involved

KDAB will happily accept external contributions, but substantial
contributions will require a signed Copyright Assignment Agreement.
Contact info@kdab.com for more information.

License
=======
The GammaRay Software is (C) 2010-2019 Klarälvdalens Datakonsult AB (KDAB),
and is available under the terms of the GPL version 2 (or any later version,
at your option).  See LICENSE.GPL.txt for license details.

Commercial use is also permitted as described in ReadMe-commercial.txt.

About KDAB
==========
GammaRay is supported and maintained by Klarälvdalens Datakonsult AB (KDAB).

The KDAB Group is the global No.1 software consultancy for Qt, C++ and
OpenGL applications across desktop, embedded and mobile platforms.

The KDAB Group provides consulting and mentoring for developing Qt applications
from scratch and in porting from all popular and legacy frameworks to Qt.
We continue to help develop parts of Qt and are one of the major contributors
to the Qt Project. We can give advanced or standard trainings anywhere
around the globe on Qt as well as C++, OpenGL, 3D and more.

If you would like to have a custom plugin for GammaRay to visualize, profile
or debug your own specific components or applications, get in touch with us
via https://www.kdab.com/contact. KDAB engineers have a lot of experience
writing GammaRay plugins and can be contracted to help you get yours working
and improve your development efficiency.

GammaRay and the GammaRay logo are registered trademarks of Klarälvdalens Datakonsult AB
in the European Union, the United States and/or other countries.  Other product and
company names and logos may be trademarks or registered trademarks of their respective companies.
