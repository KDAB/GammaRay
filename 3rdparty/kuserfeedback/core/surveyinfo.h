/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef KUSERFEEDBACK_SURVEYINFO_H
#define KUSERFEEDBACK_SURVEYINFO_H

#include "kuserfeedbackcore_export.h"

#include <QMetaType>
#include <QSharedDataPointer>
#include <qobjectdefs.h>
#include <QUrl>
#include <QUuid>


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
    Q_GADGET
    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(QUrl url READ url)
    Q_PROPERTY(QString target READ target)
    Q_PROPERTY(QUuid uuid READ uuid)
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
    static SurveyInfo fromJson(const QJsonObject &obj);
    ///@endcond
private:
    QSharedDataPointer<SurveyInfoData> d;
};

}

Q_DECLARE_METATYPE(KUserFeedback::SurveyInfo)

#endif // KUSERFEEDBACK_SURVEYINFO_H
