/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

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

#include <QApplication>
#include <QTranslator>
#include <QLabel>
#include <QVBoxLayout>
#include <QDir>
#include <QLibraryInfo>
#include <QPushButton>
#include <QFontDialog>

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        m_label1 = new QLabel(this);
        m_label2 = new QLabel(this);
        m_label3 = new QLabel(this);
        m_button = new QPushButton(this);

        m_layout = new QVBoxLayout;
        m_layout->addWidget(m_label1);
        m_layout->addWidget(m_label2);
        m_layout->addWidget(m_label3);
        m_layout->addWidget(m_button);
        setLayout(m_layout);

        connect(m_button, &QAbstractButton::clicked, this, &Widget::openFonts);

        retranslate();

        qApp->installEventFilter(this);
    }

    void retranslate()
    {
        m_label1->setText(tr("One"));
        m_label2->setText(tr("Two"));
        m_label3->setText(tr("Three"));
        m_button->setText(tr("&Fonts"));
    }

private slots:
    void openFonts()
    {
        QFontDialog dialog(this);
        dialog.exec();
    }

protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (event->type() == QEvent::LanguageChange)
            retranslate();
        return QWidget::eventFilter(object, event);
    }

private:
    QLabel *m_label1;
    QLabel *m_label2;
    QLabel *m_label3;
    QPushButton *m_button;
    QVBoxLayout *m_layout;
};

void loadTranslation(const QString &filename, const QString &name)
{
    Q_ASSERT(QDir().exists(filename));
    auto *translator = new QTranslator;
    translator->setObjectName(name);
    translator->load(filename);
    qApp->installTranslator(translator);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    loadTranslation(app.applicationDirPath() + "/translation.qm",
                    QStringLiteral("App translator"));
    loadTranslation(QLibraryInfo::location(QLibraryInfo::TranslationsPath)
                    +QStringLiteral("/qt_sv.qm"),
                    QStringLiteral("Qt translator"));

    Widget widget;
    widget.show();

    return app.exec();
}

#include "translator_test.moc"
