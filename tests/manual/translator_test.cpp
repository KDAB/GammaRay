/*
  translator_test.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Dalheimer <jan.dalheimer@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    Q_ASSERT(translator->load(filename));
    qApp->installTranslator(translator);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    loadTranslation(app.applicationDirPath() + "/translation.qm",
                    QStringLiteral("App translator"));
    loadTranslation(QLibraryInfo::location(QLibraryInfo::TranslationsPath)
                        + QStringLiteral("/qt_sv.qm"),
                    QStringLiteral("Qt translator"));

    Widget widget;
    widget.show();

    return app.exec();
}

#include "translator_test.moc"
