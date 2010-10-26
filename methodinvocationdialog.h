#ifndef ENDOSCOPE_METHODINVOCATIONDIALOG_H
#define ENDOSCOPE_METHODINVOCATIONDIALOG_H

#include "ui_methodinvocationdialog.h"
#include <qmetaobject.h>
#include <QDialog>
#include <QPointer>

namespace Endoscope {

class MethodArgumentModel;


class MethodInvocationDialog : public QDialog
{
  Q_OBJECT
  public:
    MethodInvocationDialog( QWidget* parent = 0 );
    void setMethod( QObject* object, const QMetaMethod &method );
    void accept();

  private:
    QMetaMethod m_method;
    QPointer<QObject> m_object;
    Ui::MethodInvocationDialog ui;
    MethodArgumentModel *m_argumentModel;
};

}

#endif // ENDOSCOPE_METHODINVOCATIONDIALOG_H
