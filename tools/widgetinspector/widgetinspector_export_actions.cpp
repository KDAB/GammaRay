/**
 * @file widgetinspector_export_actions.cpp
 * dlopen hack to avoid dependencies on QtSvg and QtDesigner in the main probe.
 */

#include <QFormBuilder>
#include <QPainter>
#include <QSvgGenerator>
#include <QWidget>

extern "C" {

Q_DECL_EXPORT void gammaray_save_widget_to_svg(QWidget* widget, const QString &fileName)
{
  QSvgGenerator svg;
  svg.setFileName(fileName);
  svg.setSize(widget->size());
  svg.setViewBox(QRect(QPoint(0,0), widget->size()));
  QPainter painter(&svg);
  widget->render(&painter);
  painter.end();
}

Q_DECL_EXPORT void gammaray_save_widget_to_ui(QWidget* widget, const QString &fileName)
{
  QFile file(fileName);
  if (file.open(QFile::WriteOnly)) {
    QFormBuilder formBuilder;
    formBuilder.save(&file, widget);
  }
}

}