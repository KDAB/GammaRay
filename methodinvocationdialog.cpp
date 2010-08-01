#include "methodinvocationdialog.h"
#include "methodargumentmodel.h"
#include <KLocalizedString>

using namespace Endoscope;

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
  QDialog::accept();
}

#include "methodinvocationdialog.moc"
