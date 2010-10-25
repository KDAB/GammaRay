#ifndef ENDOSCOPE_METHODINVOCATIONDIALOG_H
#define ENDOSCOPE_METHODINVOCATIONDIALOG_H

#include "ui_methodinvocationdialog.h"
#include <kdialog.h>
#include <qmetaobject.h>
#include <QPointer>

namespace Endoscope {

class MethodArgumentModel;


class MethodInvocationDialog : public KDialog
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
