/*
  loggingcategorymodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <core/tools/messagehandler/loggingcategorymodel.h>

#include <QAbstractItemModelTester>
#include <QLoggingCategory>
#include <QTest>

#include <memory>

using namespace GammaRay;

static QLoggingCategory::CategoryFilter s_previousFilter = nullptr;
static QStringList s_filteredByPrevious;

// a filter installed before the model, which the model has to keep calling
static void previousFilter(QLoggingCategory *category)
{
    s_filteredByPrevious.push_back(QString::fromUtf8(category->categoryName()));
    if (s_previousFilter)
        s_previousFilter(category);
}

class LoggingCategoryModelTest : public QObject
{
    Q_OBJECT
private:
    static int rowsFor(const QAbstractItemModel &model, const char *name)
    {
        int count = 0;
        for (int row = 0; row < model.rowCount(); ++row)
            count += model.index(row, 0).data().toString() == QLatin1String(name);
        return count;
    }

    static QModelIndex find(const QAbstractItemModel &model, const char *name)
    {
        for (int row = 0; row < model.rowCount(); ++row) {
            const auto index = model.index(row, 0);
            if (index.data().toString() == QLatin1String(name))
                return index;
        }
        return {};
    }

private slots:
    void init()
    {
        m_category = std::make_unique<QLoggingCategory>("gammaray.test.existing");
        m_model = std::make_unique<LoggingCategoryModel>();
        m_tester = std::make_unique<QAbstractItemModelTester>(m_model.get());
    }

    void cleanup()
    {
        m_tester.reset();
        m_model.reset();
        m_category.reset();
        QLoggingCategory::setFilterRules(QString());
    }

    // the categories that exist already, and the ones created later
    void testCategories()
    {
        QTRY_COMPARE(rowsFor(*m_model, "gammaray.test.existing"), 1);
        QCOMPARE(m_model->columnCount(), 5);
        QCOMPARE(m_model->headerData(2, Qt::Horizontal).toString(), QStringLiteral("Info"));

        QLoggingCategory created("gammaray.test.created");
        QTRY_COMPARE(rowsFor(*m_model, "gammaray.test.created"), 1);
    }

    void testCheckStates_data()
    {
        QTest::addColumn<int>("column");
        QTest::addColumn<int>("type");
        QTest::newRow("debug") << 1 << int(QtDebugMsg);
        QTest::newRow("warning") << 3 << int(QtWarningMsg);
        QTest::newRow("critical") << 4 << int(QtCriticalMsg);
    }

    // every message type can be switched on and off
    void testCheckStates()
    {
        QFETCH(int, column);
        QFETCH(int, type);
        QTRY_VERIFY(find(*m_model, "gammaray.test.existing").isValid());
        const auto index = find(*m_model, "gammaray.test.existing").sibling(find(*m_model, "gammaray.test.existing").row(), column);
        QVERIFY(index.flags() & Qt::ItemIsUserCheckable);

        const auto msgType = QtMsgType(type);
        m_category->setEnabled(msgType, true);
        QCOMPARE(index.data(Qt::CheckStateRole).toInt(), int(Qt::Checked));
        QVERIFY(m_model->setData(index, Qt::Unchecked, Qt::CheckStateRole));
        QVERIFY(!m_category->isEnabled(msgType));
        QCOMPARE(index.data(Qt::CheckStateRole).toInt(), int(Qt::Unchecked));
        QVERIFY(m_model->setData(index, Qt::Checked, Qt::CheckStateRole));
        QVERIFY(m_category->isEnabled(msgType));

        // the name can't be checked
        QVERIFY(!(index.sibling(index.row(), 0).flags() & Qt::ItemIsUserCheckable));
        QVERIFY(!m_model->setData(index.sibling(index.row(), 0), Qt::Checked, Qt::CheckStateRole));
    }

    // only the changes, or all, and either as rules or in the format of a file
    void testExportConfig()
    {
        QTRY_VERIFY(find(*m_model, "gammaray.test.existing").isValid());
        const auto index = find(*m_model, "gammaray.test.existing");
        const bool debug = m_category->isDebugEnabled();
        QVERIFY(m_model->setData(index.sibling(index.row(), 1), debug ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole));

        const auto changes = m_model->exportLoggingConfig(false, false);
        QCOMPARE(changes, QByteArray("gammaray.test.existing.debug=") + (debug ? "false;" : "true;"));

        const auto file = m_model->exportLoggingConfig(true, true);
        QVERIFY(file.startsWith("[Rules]\n"));
        for (const char *line : { "gammaray.test.existing.info=", "gammaray.test.existing.warning=", "gammaray.test.existing.critical=" })
            QVERIFY(file.contains(QByteArray("\n") + line));
    }

    // the model doesn't replace a filter installed before it
    void testPreviousFilter()
    {
        m_tester.reset();
        m_model.reset();
        s_previousFilter = QLoggingCategory::installFilter(previousFilter);
        m_model = std::make_unique<LoggingCategoryModel>();
        s_filteredByPrevious.clear();

        QLoggingCategory created("gammaray.test.previous");
        QVERIFY(s_filteredByPrevious.contains(QStringLiteral("gammaray.test.previous")));
        QTRY_COMPARE(rowsFor(*m_model, "gammaray.test.previous"), 1);

        // and puts it back again
        m_model.reset();
        QVERIFY(QLoggingCategory::installFilter(s_previousFilter) == &previousFilter);
        s_previousFilter = nullptr;
    }

private:
    std::unique_ptr<QLoggingCategory> m_category;
    std::unique_ptr<LoggingCategoryModel> m_model;
    std::unique_ptr<QAbstractItemModelTester> m_tester;
};

QTEST_GUILESS_MAIN(LoggingCategoryModelTest)

#include "loggingcategorymodeltest.moc"
