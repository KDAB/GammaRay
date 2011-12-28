#include "launchpage.h"
#include "ui_launchpage.h"

#include <QCompleter>
#include <QFileSystemModel>
#include <qfiledialog.h>
#include <qstringlistmodel.h>

using namespace GammaRay;

LaunchPage::LaunchPage(QWidget* parent) : QWidget(parent),
  ui(new Ui::LaunchPage),
  m_argsModel(new QStringListModel(this))
{
  ui->setupUi(this);
  connect(ui->progSelectButton, SIGNAL(clicked()), SLOT(showFileDialog()));
  connect(ui->addArgButton, SIGNAL(clicked()), SLOT(addArgument()));
  connect(ui->removeArgButton, SIGNAL(clicked()), SLOT(removeArgument()));

  ui->argsBox->setModel(m_argsModel);

  QCompleter *pathCompleter = new QCompleter(this);
  QFileSystemModel *fsModel = new QFileSystemModel(this);
  fsModel->setRootPath(QDir::rootPath());
  pathCompleter->setModel(fsModel);
  ui->progEdit->setCompleter(pathCompleter);
}

LaunchPage::~LaunchPage()
{
  delete ui;
}

QStringList LaunchPage::launchArguments() const
{
  QStringList l;
  l.push_back(ui->progEdit->text());
  l.append(m_argsModel->stringList());
  return l;
}

void LaunchPage::showFileDialog()
{
  // TODO: add *.exe filter on Windows
  const QString exeFilePath = QFileDialog::getOpenFileName(this, tr("Executable to Launch"), ui->progEdit->text());
  if (exeFilePath.isEmpty())
    return;

  // TODO: check if this is an executable and using a Qt version etc we have a probe for
  ui->progEdit->setText(exeFilePath);
}

void LaunchPage::addArgument()
{
  m_argsModel->insertRows(m_argsModel->rowCount(), 1);
  const QModelIndex newIndex = m_argsModel->index(m_argsModel->rowCount() - 1, 0);
  ui->argsBox->edit(newIndex);
}

void LaunchPage::removeArgument()
{
  // TODO check if there's a selection at all and update button state accordingly
  m_argsModel->removeRows(ui->argsBox->currentIndex().row(), 1);
}

#include "launchpage.moc"
