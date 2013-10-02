
#include <QApplication>

#include "variantinspector.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  VariantInspector vi;

  QHash<QString, int> mapping;
  mapping.insert("One", 1);
  mapping.insert("Two", 2);

  vi.setProperty("dynamicProperty", QVariant::fromValue(mapping));

  return app.exec();
}
