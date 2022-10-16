/*
  classesiconsrepository.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CLASSESICONSREPOSITORY_H
#define GAMMARAY_CLASSESICONSREPOSITORY_H

#include "gammaray_common_export.h"

#include <QObject>
#include <QVector>

namespace GammaRay {

/*! Collection of classes icons id/file path and on-demand client/server transfer of those. */
class GAMMARAY_COMMON_EXPORT ClassesIconsRepository : public QObject
{
    Q_OBJECT

public:
    ~ClassesIconsRepository() override;

    /*! Request the icon file path for the given icon id.
     *  On the client side the response can be invalid on first request,
     *  as transfer from the server is asynchronous. Listen to the
     *  indexReceived() signal to be notified for its availability
     *  in that case.
     */
    virtual QString filePath(int id) const;

    using ConstIterator = QVector<QString>::const_iterator;

    ConstIterator constBegin();
    ConstIterator constEnd();

signals:
    /*! Notification of complete index availability.
     *  @see filePath()
     */
    void indexChanged();

    //! @cond internal
    void indexResponse(const QVector<QString> &index);

protected:
    explicit ClassesIconsRepository(QObject *parent = nullptr);

    QVector<QString> index() const;
    void setIndex(const QVector<QString> &index);

protected slots:
    virtual void requestIndex() = 0;
    //! @endcond

private:
    QVector<QString> m_iconsIndex;
};

}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::ClassesIconsRepository, "com.kdab.GammaRay.ClassesIconsRepository/1.0")
QT_END_NAMESPACE

#endif // GAMMARAY_CLASSESICONSREPOSITORY_H
