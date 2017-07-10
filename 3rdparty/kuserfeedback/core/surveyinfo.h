/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KUSERFEEDBACK_SURVEYINFO_H
#define KUSERFEEDBACK_SURVEYINFO_H

#include "kuserfeedbackcore_export.h"

#include <QMetaType>
#include <QSharedDataPointer>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <qobjectdefs.h>
#include <QUrl>
#include <QUuid>
#endif


QT_BEGIN_NAMESPACE
class QJsonObject;
class QUrl;
class QUuid;
QT_END_NAMESPACE

namespace KUserFeedback {

class SurveyInfoData;

/*! Information about a survey request.
 *  This class is implicitly shared.
 */
class KUSERFEEDBACKCORE_EXPORT SurveyInfo
{
#ifndef QT4_MOC_WORKAROUND
    Q_GADGET
    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(QUrl url READ url)
    Q_PROPERTY(QString target READ target)
    Q_PROPERTY(QUuid uuid READ uuid)
#endif
public:
    /*! Create an empty, invalid survey request. */
    SurveyInfo();
    /*! Copy constructor. */
    SurveyInfo(const SurveyInfo&);
    ~SurveyInfo();
    /*! Assignment operator. */
    SurveyInfo& operator=(const SurveyInfo&);

    /*! Returns @c true if this survey has all necessary information to actually execute it. */
    bool isValid() const;

    /*! Internal global unique id of the survey.
     *  Used to locally check if a user has completed the survey already.
     */
    QUuid uuid() const;

    /*! The URL to the survey website. */
    QUrl url() const;

    /*! The survey targeting expression. */
    QString target() const;

    ///@cond internal
    void setUuid(const QUuid &id);
    void setUrl(const QUrl &url);
    void setTarget(const QString &target);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    static SurveyInfo fromJson(const QJsonObject &obj);
#endif
    ///@endcond
private:
    QSharedDataPointer<SurveyInfoData> d;
};

}

Q_DECLARE_METATYPE(KUserFeedback::SurveyInfo)

#endif // KUSERFEEDBACK_SURVEYINFO_H
