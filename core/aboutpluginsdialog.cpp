/*
  aboutpluginsdialog.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "aboutpluginsdialog.h"
#include "pluginmanager.h"

#include "include/toolfactory.h"

#include <QAbstractTableModel>
#include <QGroupBox>
#include <QHeaderView>
#include <QTableView>
#include <QVBoxLayout>

using namespace GammaRay;

class ErrorModel : public QAbstractTableModel
{
  public:
    explicit ErrorModel(PluginLoadErrors &errors, QObject *parent = 0)
      : QAbstractTableModel(parent), m_errors(errors)
    {
    }

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
      Q_UNUSED(parent);
      return 3;
    }

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
      Q_UNUSED(parent);
      return m_errors.size();
    }

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const
    {
      if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
          return tr("Plugin Name");
        case 1:
          return tr("Plugin File");
        case 2:
          return tr("Error Message");
        }
      }
      return QAbstractTableModel::headerData(section, orientation, role);
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
      if (!index.isValid()) {
        return QVariant();
      }

      const int row = index.row();
      const int column = index.column();
      if (role == Qt::DisplayRole) {
        switch (column) {
        case 0:
          return m_errors[row].pluginName();
        case 1:
          return m_errors[row].pluginFile;
        case 2:
          return m_errors[row].errorString;
        }
      }
      return QVariant();
    }

  private:
    PluginLoadErrors m_errors;
};

class ToolModel : public QAbstractTableModel
{
  public:
    explicit ToolModel(const QVector<ToolFactory*>& tools, QObject* parent = 0)
      : QAbstractTableModel(parent), m_tools(tools)
    {
    }

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
      Q_UNUSED(parent);
      return 3;
    }

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
      Q_UNUSED(parent);
      return m_tools.size();
    }

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const
    {
      if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
          return tr("Id");
        case 1:
          return tr("Name");
        case 2:
          return tr("Supported types");
        }
      }
      return QAbstractTableModel::headerData(section, orientation, role);
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
      if (!index.isValid()) {
        return QVariant();
      }

      const int row = index.row();
      const int column = index.column();
      if (role == Qt::DisplayRole) {
        ToolFactory *factory = m_tools[row];
        switch (column) {
        case 0:
          return factory->id();
        case 1:
          return factory->name();
        case 2:
          return factory->supportedTypes();
        }
      }
      return QVariant();
    }

  private:
    QVector<ToolFactory*> m_tools;
};

AboutPluginsDialog::AboutPluginsDialog(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  QLayout *layout = 0;
  QVBoxLayout *vbox = new QVBoxLayout(this);

  {
    QVector<ToolFactory*> tools = PluginManager::instance()->plugins();
    ToolModel *toolModel = new ToolModel(tools, this);
    QTableView *toolView = new QTableView(this);
    toolView->setShowGrid(false);
    toolView->setSelectionBehavior(QAbstractItemView::SelectRows);
    toolView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    toolView->verticalHeader()->hide();
    toolView->setModel(toolModel);

    QGroupBox *toolBox = new QGroupBox(tr("Loaded Plugins"), this);
    layout = new QHBoxLayout(toolBox);
    layout->addWidget(toolView);
    vbox->addWidget(toolBox);

    toolBox->setEnabled(toolModel->rowCount() > 0);
  }

  {
    PluginLoadErrors errors = PluginManager::instance()->errors();
    ErrorModel *errorModel = new ErrorModel(errors, this);
    QTableView *errorView = new QTableView(this);
    errorView->setShowGrid(false);
    errorView->setSelectionBehavior(QAbstractItemView::SelectRows);
    errorView->setModel(errorModel);
    errorView->verticalHeader()->hide();
    errorView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    QGroupBox *errorBox = new QGroupBox(tr("Failed Plugins"), this);
    layout = new QHBoxLayout(errorBox);
    layout->addWidget(errorView);
    vbox->addWidget(errorBox);

    errorBox->setEnabled(errorModel->rowCount() > 0);
  }

  setWindowTitle(tr("GammaRay: Plugin Info"));
}

#include "aboutpluginsdialog.moc"
