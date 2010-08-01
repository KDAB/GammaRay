#include "methodinvocationdialog.h"
#include "methodargumentmodel.h"
#include <KLocalizedString>
#include <KMessageBox>

using namespace Endoscope;

Q_DECLARE_METATYPE( Qt::ConnectionType )

MethodInvocationDialog::MethodInvocationDialog(QWidget* parent) :
  KDialog(parent),
  m_argumentModel( new MethodArgumentModel( this ) )
{
  setAttribute( Qt::WA_DeleteOnClose );

  setButtons( Ok | Cancel );
  setButtonGuiItem( Ok, KGuiItem( i18n( "Invoke" ), KIcon( "system-run" ) ) );

  ui.setupUi( mainWidget() );

  ui.connectionTypeComboBox->addItem( i18n( "Auto" ), Qt::AutoConnection );
  ui.connectionTypeComboBox->addItem( i18n( "Direct" ), Qt::DirectConnection );
  ui.connectionTypeComboBox->addItem( i18n( "Queued" ), Qt::QueuedConnection );

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
    KMessageBox::error( this, i18n( "Invalid object, probably got deleted in the meantime." ),
                        i18n( "Invocation Failed" ) );
    QDialog::reject();
    return;
  }

  const Qt::ConnectionType connectionType = ui.connectionTypeComboBox->itemData( ui.connectionTypeComboBox->currentIndex() ).value<Qt::ConnectionType>();
  const QVector<QSharedPointer<GenericSafeArgument> > args = m_argumentModel->arguments();

  const bool result = m_method.invoke( m_object.data(), connectionType,
    *args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6], *args[7], *args[8], *args[9] );

  if ( !result ) {
    KMessageBox::error( this, i18n( "Invocation failed, possibly due to mismatching/invalid arguments." ),
                        i18n( "Invocation Failed" ) );
  }

  QDialog::accept();
}

#include "methodinvocationdialog.moc"
