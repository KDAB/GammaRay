# <a name="title"></a> [![Logo](ui/resources/gammaray/ui/light/pixmaps/gammaray-trademark@2x.png)](https://www.kdab.com/gammaray)

GammaRay is a software introspection tool for Qt applications developed by KDAB.
Leveraging the QObject introspection mechanism it allows you to observe and
manipulate your application at runtime. This works both locally on your
workstation and remotely on an embedded target.

Augmenting your instruction-level debugger, GammaRay allows you to work on a
much higher level, with the same concepts as the frameworks you use. This is
especially useful for the more complex Qt frameworks such as model/view, state
machines or scene graphs.

Among other things GammaRay can:

* Browse the `QObject` tree with live updates.
* View and edit object properties.
* View and invoke slots of a `QObject`, and monitor its signals.
* List all `QObject` inbound and outbound signal/slot connections.
* Provide a layout information overlay for `QWidget` and `QtQuick2` applications.
* Inspect all `QPainter` operations used to draw a specific widget.
* Browse the `QtQuick2` item tree and scenegraph.
* Inspect shaders and geometry data of QtQuick2 items.
* Plot object lifetime and emitted signals.
* Browse the `QAbstractProxyModel` hierarchy and inspect intermediate results in a proxy model chain.
* Visual live inspection of QStateMachines.
* Browse the item tree of any `QGraphicsView` scene.
* Show a live preview of `QGraphicsView` items, including showing their coordinate system,
  transformation origin, rotate/zoom/pan, etc.
* Intercept translations and change them at runtime.
* Inspect all building blocks of a `QStyle`.
* Show all QTimers and their statistics (number of wakeups, wakeup time, ...)
* Browse all QTextDocuments, along with the ability to edit them and view their internal structures.
* Act as a complete java script debugger, attachable to any `QScriptEngine`
  (including the usually not accessible one used by QtQuick1 internally).
* Perform HTML/CSS/DOM/JS introspection/editing/profiling on any QWebPage, thanks to QWebInspector.
* Browse the QResource tree and its content.
* Show all registered meta types.
* Show all installed fonts.
* Show all available codecs.

## Building GammaRay

[See installation notes](INSTALL.md)

## Contact

* See our official home page: <https://www.kdab.com/gammaray>
* Visit us on GitHub: <https://github.com/KDAB/GammaRay>
* Email info@kdab.com for questions about copyright, licensing or commercial support.

Stay up-to-date with KDAB product announcements:

* [KDAB Newsletter](https://news.kdab.com)
* [KDAB Blogs](https://www.kdab.com/category/blogs)
* [KDAB on Twitter](https://twitter.com/KDABQt)

## Get Involved

If you want to contribute, please check out: <https://github.com/KDAB/GammaRay/wiki/Get-Involved>

Please submit your contributions or issue reports from our GitHub space at
<https://github.com/KDAB/GammaRay>.

KDAB will happily accept external contributions; however, **all** contributions
require a signed KDAB Copyright Assignment Agreement.

* Individual contributors (non-employees) are required to electronically agree
  to the KDAB CLA using the GitHub cla-assistant hook.

* Employees, representing their company, must email a completed and signed
  [KDAB Copyright Assignment Agreement](https://github.com/KDAB/GammaRay/blob/master/docs/GammaRay-CopyrightAssignmentForm.pdf)
  to info@kdab.com.

This is needed so we can continue to dual-license GammaRay.
Contact info@kdab.com for more information.

Thanks to our [contributors](CONTRIBUTORS.txt).

## License

The GammaRay Software is © Klarälvdalens Datakonsult AB (KDAB), and is
available under the terms of the GPL version 2 (or any later version,
at your option).  See [GPL-2.0-or-later.txt](LICENSES/GPL-2.0-or-later.txt)
for license details.

Contact KDAB at <info@kdab.com> to inquire about commercial licensing.

For terms of redistribution, refer to the corresponding license agreement.

Note that this software relies on 3rd party MIT-licensed projects
and various other freely distributable files some of which are
unused only for testing and not part of the application itself.

For convenience, GammaRay source bundles (.tar.gz or .zip files) contain code for
the [graphviz library](https://gitlab.com/graphviz/graphviz) which is licensed
according to the [Eclipse Public License v1.0](https://www.eclipse.org/legal/epl/epl-v10.html).

## About KDAB

GammaRay is supported and maintained by Klarälvdalens Datakonsult AB (KDAB).

The [KDAB](https://www.kdab.com) Group is a globally recognized provider for software
consulting, development and training, specializing in embedded devices and complex
cross-platform desktop applications. In addition to being leading experts in Qt, C++ and 3D
technologies for over two decades, KDAB provides deep expertise across the stack,
including Linux, Rust and modern UI frameworks. With 100+ employees from 20 countries
and offices in Sweden, Germany, USA, France and UK, KDAB serves clients around the world.

Please visit <https://www.kdab.com> to meet the people who write code like this.

Blogs and publications: <https://www.kdab.com/resources>

Videos (Tutorials and more): <https://www.youtube.com/@KDABtv>

Software Developer Training for Qt, Modern C++, Rust, OpenGL and more: <https://training.kdab.com>

Software Consulting and Development Services for Embedded and Desktop Applications <https://www.kdab.com/services/>

If you would like to have a custom plugin for GammaRay to visualize, profile
or debug your own specific components or applications, get in touch with us
via <https://www.kdab.com/contact>.  KDAB engineers know how to write GammaRay
plugins and can be contracted to help you get yours working and improve your
development efficiency.

GammaRay and the GammaRay logo are registered trademarks of Klarälvdalens Datakonsult AB
in the European Union, the United States and/or other countries.  Other product and
company names and logos may be trademarks or registered trademarks of their respective companies.
