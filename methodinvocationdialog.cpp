#include "methodinvocationdialog.h"
#include "methodargumentmodel.h"
#include <QMessageBox>

using namespace Endoscope;

Q_DECLARE_METATYPE( Qt::ConnectionType )

MethodInvocationDialog::MethodInvocationDialog(QWidget* parent) :
  KDialog(parent),
  m_argumentModel( new MethodArgumentModel( this ) )
{
  setAttribute( Qt::WA_DeleteOnClose );

  setButtons( Ok | Cancel );
  setButtonGuiItem( Ok, KGuiItem( tr( "Invoke" ), KIcon( "system-run" ) ) );

  ui.setupUi( mainWidget() );

  ui.connectionTypeComboBox->addItem( tr( "Auto" ), Qt::AutoConnection );
  ui.connectionTypeComboBox->addItem( tr( "Direct" ), Qt::DirectConnection );
  ui.connectionTypeComboBox->addItem( tr( "Queued" ), Qt::QueuedConnection );

  ui.argumentView->setModel( m_argumentModel );
}

void MethodInvocationDialog::setMethod( QObject *object, const QMetaMethod& method)
{
  m_object = object;
  m_method = method;
  m_argumentModel->setMethod( method );
}

void MethodInvocationDialog::accept()
{
  if ( !m_object ) {
    QMessageBox::warning( this,
                          tr( "Invocation Failed" ),
                          tr( "Invalid object, probably got deleted in the meantime." ) );
    QDialog::reject();
    return;
  }

  const Qt::ConnectionType connectionType = ui.connectionTypeComboBox->itemData( ui.connectionTypeComboBox->currentIndex() ).value<Qt::ConnectionType>();
  const QVector<SafeArgument> args = m_argumentModel->arguments();

  const bool result = m_method.invoke( m_object.data(), connectionType,
    args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9] );

  if ( !result ) {
    QMessageBox::warning( this,
                          tr( "Invocation Failed" ),
                          tr( "Invocation failed, possibly due to mismatching/invalid arguments." ) );
  }

  QDialog::accept();
}

#include "methodinvocationdialog.moc"
