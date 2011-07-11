
#include <QtDeclarative>

// #if defined(QMLJSDEBUGGER)
#include <qt_private/qdeclarativedebughelper_p.h>
// #endif

#include "qmlprofilertool.h"

using namespace QmlProfiler::Internal;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QmlProfilerTool tool(0);

    tool.connectClient(3768);

    return app.exec();
}