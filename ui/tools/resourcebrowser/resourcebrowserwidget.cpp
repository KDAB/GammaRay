/*
  resourcebrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#include "resourcebrowserwidget.h"

#include "ui_resourcebrowserwidget.h"
#include "resourcebrowserclient.h"
#include "clientresourcemodel.h"

#include <ui/searchlinecontroller.h>
#include <3rdparty/qt/resourcemodel.h>
#include <common/objectbroker.h>

#include <QBuffer>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QImageReader>
#include <QMenu>
#include <QScrollBar>
#include <QTimer>
#include <QTextBlock>

using namespace GammaRay;

static QObject *createResourceBrowserClient(const QString & /*name*/, QObject *parent)
{
    return new ResourceBrowserClient(parent);
}

ResourceBrowserWidget::ResourceBrowserWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ResourceBrowserWidget)
    , m_stateManager(this)
    , m_interface(0)
{
    ObjectBroker::registerClientObjectFactoryCallback<ResourceBrowserInterface *>(
        createResourceBrowserClient);
    m_interface = ObjectBroker::object<ResourceBrowserInterface *>();
    connect(m_interface, SIGNAL(resourceDeselected()), this, SLOT(resourceDeselected()));
    connect(m_interface, SIGNAL(resourceSelected(QByteArray,int,int)), this,
            SLOT(resourceSelected(QByteArray,int,int)));
    connect(m_interface, SIGNAL(resourceDownloaded(QString,QByteArray)), this,
            SLOT(resourceDownloaded(QString,QByteArray)));

    ui->setupUi(this);
    auto resModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ResourceModel"));
    ClientResourceModel *model = new ClientResourceModel(this);
    model->setSourceModel(resModel);
    ui->treeView->header()->setObjectName("resourceTreeViewHeader");
    ui->treeView->setExpandNewContent(true);
    ui->treeView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    ui->treeView->setDeferredHidden(3, true);
    ui->treeView->setModel(model);
    ui->treeView->setSelectionModel(ObjectBroker::selectionModel(ui->treeView->model()));
    new SearchLineController(ui->searchLine, resModel);

    connect(ui->treeView, SIGNAL(newContentExpanded()), SLOT(setupLayout()));

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(handleCustomContextMenu(QPoint)));

    ui->resourceLabel->setText(tr("Select a Resource to Preview"));
    ui->stackedWidget->setCurrentWidget(ui->contentLabelPage);
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    ui->textBrowser->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
#endif
}

ResourceBrowserWidget::~ResourceBrowserWidget()
{
}

void ResourceBrowserWidget::selectResource(const QString &sourceFilePath, int line, int column)
{
    m_interface->selectResource(sourceFilePath, line, column);
}

void ResourceBrowserWidget::setupLayout()
{
    // now the view was setup properly and we can mess with the splitter to resize
    // the widgets for nicer display

    int viewWidth = ui->treeView->columnWidth(0)
                    +ui->treeView->columnWidth(1)
                    +ui->treeView->columnWidth(2)
                    +ui->treeView->contentsMargins().left()
                    +ui->treeView->contentsMargins().right()
                    + ui->treeView->verticalScrollBar()->width();
    const int totalWidth = ui->mainSplitter->width();
    const int minPreviewWidth = 150;
    if (totalWidth > viewWidth + minPreviewWidth) {
        m_stateManager.setDefaultSizes(ui->mainSplitter,
                                       UISizeVector() << viewWidth
                                                      << (totalWidth - viewWidth
                                           - ui->mainSplitter->handleWidth()));
        m_stateManager.restoreState();
    }
}

void ResourceBrowserWidget::resourceDeselected()
{
    ui->resourceLabel->setText(tr("Select a Resource to Preview"));
    ui->stackedWidget->setCurrentWidget(ui->contentLabelPage);
}

void ResourceBrowserWidget::resourceSelected(const QByteArray &contents, int line, int column)
{
    // try to decode as an image first, fall back to text otherwise
    auto rawData = contents;
    QBuffer buffer(&rawData);
    buffer.open(QBuffer::ReadOnly);
    QImageReader reader(&buffer);
    const auto img = reader.read();
    if (!img.isNull()) {
        ui->resourceLabel->setPixmap(QPixmap::fromImage(img));
        ui->stackedWidget->setCurrentWidget(ui->contentLabelPage);
        return;
    }

    // avoid re-highlighting the existing content when switching the syntax
    ui->textBrowser->clear();

    // get the file name for syntax highlighting
    QString fileName;
    const auto selection = ui->treeView->selectionModel()->selectedRows();
    if (!selection.isEmpty())
        fileName = selection.at(0).data().toString();
    ui->textBrowser->setFileName(fileName);

    // TODO: make encoding configurable
    ui->textBrowser->setPlainText(contents);

    QTextDocument *document = ui->textBrowser->document();
    QTextCursor cursor(document->findBlockByLineNumber(line - 1));
    if (!cursor.isNull()) {
        if (column >= 1)
            cursor.setPosition(cursor.position() + column - 1);
        ui->textBrowser->setTextCursor(cursor);
    }
    ui->textBrowser->setFocus();

    ui->stackedWidget->setCurrentWidget(ui->contentTextPage);
}

void ResourceBrowserWidget::resourceDownloaded(const QString &targetFilePath,
                                               const QByteArray &contents)
{
    QFile file(targetFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Unable to write resource content to %s", qPrintable(targetFilePath));
        return;
    }

    file.write(contents);
    file.close();
}

static QStringList collectDirectories(const QModelIndex &index, const QString &baseDirectory)
{
    QStringList result;

    const QAbstractItemModel *model = index.model();
    const QString directoryPath = index.data(ResourceModel::FilePathRole).toString();

    const QString relativeDirectory = directoryPath.mid(baseDirectory.size());
    result << relativeDirectory;

    for (int row = 0; row < model->rowCount(index); ++row) {
        const QModelIndex childIndex = model->index(row, 0, index);
        if (model->hasChildren(childIndex))
            result += collectDirectories(childIndex, baseDirectory);
    }

    return result;
}

static QStringList collectFiles(const QModelIndex &index, const QString &baseDirectory)
{
    QStringList result;

    const QAbstractItemModel *model = index.model();
    for (int row = 0; row < model->rowCount(index); ++row) {
        const QModelIndex childIndex = model->index(row, 0, index);
        if (model->hasChildren(childIndex)) {
            result += collectFiles(childIndex, baseDirectory);
        } else {
            const QString filePath = childIndex.data(ResourceModel::FilePathRole).toString();
            const QString relativeFilePath = filePath.mid(baseDirectory.size());
            result << relativeFilePath;
        }
    }

    return result;
}

void ResourceBrowserWidget::handleCustomContextMenu(const QPoint &pos)
{
    const QModelIndex selectedIndex = ui->treeView->indexAt(pos);
    if (!selectedIndex.isValid())
        return;

    QMenu menu;
    menu.addAction(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("Save As..."));
    if (!menu.exec(ui->treeView->viewport()->mapToGlobal(pos)))
        return;

    if (selectedIndex.model()->hasChildren(selectedIndex)) {
        const QString sourceDirectory = selectedIndex.data(ResourceModel::FilePathRole).toString();
        const QString targetDirectory = QFileDialog::getExistingDirectory(this, tr("Save As"));

        // create local target directory tree
        foreach (const QString &directoryPath, collectDirectories(selectedIndex, sourceDirectory)) {
            if (directoryPath.isEmpty())
                continue;

            QDir dir(targetDirectory + '/' + directoryPath);
            dir.mkpath(QStringLiteral("."));
        }

        // request all resource files
        foreach (const QString &filePath, collectFiles(selectedIndex, sourceDirectory))
            m_interface->downloadResource(sourceDirectory + filePath, targetDirectory + filePath);

    } else {
        const QString sourceFilePath = selectedIndex.data(ResourceModel::FilePathRole).toString();
        const QString sourceFileName = sourceFilePath.mid(sourceFilePath.lastIndexOf('/') + 1);

        const QString targetFilePath = QFileDialog::getSaveFileName(this, tr(
                                                                        "Save As"), sourceFileName);
        if (targetFilePath.isEmpty())
            return;

        m_interface->downloadResource(sourceFilePath, targetFilePath);
    }
}
