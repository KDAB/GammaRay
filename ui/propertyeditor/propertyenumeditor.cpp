/*
  propertyenumeditor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "propertyenumeditor.h"

#include <common/enumrepository.h>
#include <common/objectbroker.h>

using namespace GammaRay;

#include <QAbstractItemModel>
#include <QDebug>
#include <QEvent>
#include <QListView>
#include <QStylePainter>

namespace GammaRay {
class PropertyEnumEditorModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PropertyEnumEditorModel(QObject *parent = nullptr);
    ~PropertyEnumEditorModel() override;

    EnumValue value() const;
    void setValue(const EnumValue &value);
    void updateValue(int value);

    EnumDefinition definition() const;
    void setDefinition(const EnumDefinition &def);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;


private:

    EnumValue m_value;
    EnumDefinition m_def;
};

}

PropertyEnumEditorModel::PropertyEnumEditorModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

PropertyEnumEditorModel::~PropertyEnumEditorModel() = default;

EnumValue PropertyEnumEditorModel::value() const
{
    return m_value;
}

void PropertyEnumEditorModel::setValue(const EnumValue &value)
{
    beginResetModel();
    m_value = value;
    auto repo = ObjectBroker::object<EnumRepository*>();
    m_def = repo->definition(value.id());
    endResetModel();
}

void PropertyEnumEditorModel::updateValue(int value)
{
    Q_ASSERT(m_value.isValid());
    Q_ASSERT(m_def.isValid());

    m_value.setValue(value);
}

EnumDefinition PropertyEnumEditorModel::definition() const
{
    return m_def;
}

void PropertyEnumEditorModel::setDefinition(const EnumDefinition &def)
{
    beginResetModel();
    m_def = def;
    endResetModel();
}

int PropertyEnumEditorModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_def.elements().size();
}

QVariant PropertyEnumEditorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        return m_def.elements().at(index.row()).name();
    } else if (role == Qt::CheckStateRole && m_def.isFlag()) {
        const auto elem = m_def.elements().at(index.row());
        if (elem.value() == 0)
            return m_value.value() == 0 ? Qt::Checked : Qt::Unchecked;
        return (elem.value() & m_value.value()) == elem.value() ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

Qt::ItemFlags PropertyEnumEditorModel::flags(const QModelIndex &index) const
{
    const auto f = QAbstractListModel::flags(index);
    if (index.isValid() && m_def.isFlag() && m_def.elements().at(index.row()).value() != 0) // 0 values can't be toggled
        return f | Qt::ItemIsUserCheckable;
    return f;
}

bool PropertyEnumEditorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || !m_def.isFlag())
        return false;

    if (role == Qt::CheckStateRole) {
        const auto elem = m_def.elements().at(index.row());
        if (value.toInt() == Qt::Checked)
            m_value.setValue(m_value.value() | elem.value());
        else if (value.toInt() == Qt::Unchecked)
            m_value.setValue(m_value.value() & ~elem.value());
        emit dataChanged(this->index(0,0), this->index(rowCount() - 1, 0)); // mask flags can change multiple rows
        return true;
    }
    return QAbstractListModel::setData(index, value, role);
}

PropertyEnumEditor::PropertyEnumEditor(QWidget* parent) :
    QComboBox(parent),
    m_model(new PropertyEnumEditorModel(this))
{
    setModel(m_model);
    connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(update())); // FIXME: Clazy sais 4 overloads for update, but I find not a single one...

    auto repo = ObjectBroker::object<EnumRepository*>();
    connect(repo, &EnumRepository::definitionChanged, this, &PropertyEnumEditor::definitionChanged);

    setEnabled(false);
    connect(this, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &PropertyEnumEditor::slotCurrentIndexChanged);
}

PropertyEnumEditor::~PropertyEnumEditor() = default;

EnumValue PropertyEnumEditor::enumValue() const
{
    return m_model->value();
}

void PropertyEnumEditor::setEnumValue(const EnumValue& value)
{
    m_model->setValue(value);
    updateCurrentIndex();
    setupView();
}

void PropertyEnumEditor::definitionChanged(int id)
{
    if (!m_model->value().isValid() || id != m_model->value().id())
        return;

    auto repo = ObjectBroker::object<EnumRepository*>();
    const auto def = repo->definition(id);
    m_model->setDefinition(def);
    updateCurrentIndex();
    setupView();
}

void PropertyEnumEditor::updateCurrentIndex()
{
    const auto def = m_model->definition();
    if (!def.isValid() || !m_model->value().isValid())
        return;

    if (!def.isFlag()) {
        for (int i = 0; i < def.elements().size(); ++i) {
            if (def.elements().at(i).value() == m_model->value().value()) {
                setCurrentIndex(i);
                break;
            }
        }
    }
}

void PropertyEnumEditor::setupView()
{
    const auto def = m_model->definition();
    if (!def.isValid())
        return;
    setEnabled(true);

    if (def.isFlag() && view()->metaObject() != &QListView::staticMetaObject) {
        // the default view doesn't show check boxes (at least with the Fusion style)
        auto v = new QListView(this);
        setView(v);
        v->installEventFilter(this);
        v->viewport()->installEventFilter(this);
    }
}

void PropertyEnumEditor::slotCurrentIndexChanged(int index)
{
    const auto def = m_model->definition();
    if (!def.isValid() || def.isFlag() || index < 0)
        return;
    m_model->updateValue(def.elements().at(index).value());
}

void PropertyEnumEditor::paintEvent(QPaintEvent* event)
{
    const auto def = m_model->definition();

    if (def.isValid() && !def.isFlag()) {
        QComboBox::paintEvent(event);
        return;
    }

    // for flags we paint the combined value name as text
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));
    QStyleOptionComboBox opt;
    initStyleOption(&opt);

    if (!def.isValid())
        opt.currentText = tr("Loading...");
    else
        opt.currentText = def.valueToString(m_model->value());

    painter.drawComplexControl(QStyle::CC_ComboBox, opt);
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

bool PropertyEnumEditor::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == view() || watched == view()->viewport()) {
        if (event->type() == QEvent::MouseButtonRelease && m_model->definition().isFlag()) {
            const auto state = view()->currentIndex().data(Qt::CheckStateRole).toInt();
            m_model->setData(view()->currentIndex(), state == Qt::Checked ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);
            return true;
        }
    }
    return QComboBox::eventFilter(watched, event);
}

#include "propertyenumeditor.moc"
