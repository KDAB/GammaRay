/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KUSERFEEDBACK_SELECTIONRATIOSOURCE_H
#define KUSERFEEDBACK_SELECTIONRATIOSOURCE_H

#include "kuserfeedbackcore_export.h"
#include "abstractdatasource.h"

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
QT_END_NAMESPACE

namespace KUserFeedback {

class SelectionRatioSourcePrivate;

/*! Records the time ratio a given entry is selected via a QItemSelectionModel.
 *
 *  An example use-case would be the usage ratio of a applications
 *  views/modes selected using a model-based view sidebar (such as
 *  used in e.g. Kontact).
 *
 *  The default telemetry mode for this source is Provider::DetailedUsageStatistics.
 */
class KUSERFEEDBACKCORE_EXPORT SelectionRatioSource : public AbstractDataSource
{
public:
    /*! Create a new selection ratio data source.
     * @param selectionModel The selection to monitor.
     * @param sampleName This is the name of the database field this data source is
     * associated with.
     */
    explicit SelectionRatioSource(QItemSelectionModel *selectionModel, const QString &sampleName);

    /*! Determine which role to consider for the reported value.
     *  By default this is Qt::DisplayRole.
     */
    void setRole(int role);

    QString description() const override;
    /*! Set human-readable and translated description of the data provided by this source.
     *  @note This must be set before adding this source, sources without description are
     *  discarded.
     *  @param desc The description.
     */
    void setDescription(const QString &desc);

    QVariant data() override;
    void loadImpl(QSettings *settings) override;
    void storeImpl(QSettings *settings) override;
    void resetImpl(QSettings *settings) override;

private:
    Q_DECLARE_PRIVATE(SelectionRatioSource)
};

}

#endif // KUSERFEEDBACK_SELECTIONRATIOSOURCE_H
