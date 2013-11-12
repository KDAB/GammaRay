/*
 * Copyright (C) 2007-2008 Omat Holding B.V. <info@omat.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kfilterproxysearchline.h"

#include <QHBoxLayout>
#include <QTimer>
#include <QSortFilterProxyModel>

#include <qlineedit.h>
//#include <klocale.h>

/**
 * Private class that helps to provide binary compatibility between releases.
 * @internal
 */
//@cond PRIVATE
class KFilterProxySearchLine::Private {
public:
    Private( KFilterProxySearchLine* parent) :
                                q(parent), proxy(0), searchLine(0)
    {
    timer = new QTimer( q );
    timer->setSingleShot( true );
    connect( timer, SIGNAL( timeout() ), q, SLOT( slotSearchLineActivate() ) );
    }
    QTimer* timer;
    KFilterProxySearchLine* q;
    QSortFilterProxyModel* proxy;
    QLineEdit* searchLine;

    void slotSearchLineChange( const QString& newText );
    void slotSearchLineActivate();
};

void KFilterProxySearchLine::Private::slotSearchLineChange( const QString& )
{
    timer->start( 300 );
}

void KFilterProxySearchLine::Private::slotSearchLineActivate()
{
    if ( !proxy )
        return;

    proxy->setFilterKeyColumn( -1 );
    proxy->setFilterCaseSensitivity( Qt::CaseInsensitive );
    proxy->setFilterFixedString( searchLine->text() );
}
//@endcond

KFilterProxySearchLine::KFilterProxySearchLine( QWidget* parent )
        : QWidget( parent ), d( new Private( this ) )
{
    d->searchLine = new QLineEdit( this );
//    d->searchLine->setClearButtonShown( true );
#if QT_VERSION >= 0x040700
    d->searchLine->setPlaceholderText(tr("Search"));
#endif

    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setMargin( 0 );
    layout->addWidget( d->searchLine );

    connect( d->searchLine, SIGNAL( textChanged( const QString& ) ),
             SLOT( slotSearchLineChange( const QString& ) ) );
}

KFilterProxySearchLine::~KFilterProxySearchLine()
{
    delete d;
}

void KFilterProxySearchLine::setText( const QString& text )
{
    d->searchLine->setText( text );
}

void KFilterProxySearchLine::setProxy( QSortFilterProxyModel* proxy ) 
{
    d->proxy = proxy;
}

QLineEdit* KFilterProxySearchLine::lineEdit() const
{
    return d->searchLine;
}

#include "moc_kfilterproxysearchline.cpp"
