/****************************************************************************
** $Id: ametadataio.h,v 1.5 2008/12/24 16:19:38 leader Exp $
**
** Header file of the Ananas configuration objects of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2008 Valeriy Grazhdankin.  All rights reserved.
**
** This file is part of the Library of the Ananas
** automation accounting system.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.leaderit.ru/page=ananas or email sales@leaderit.ru
** See http://www.leaderit.ru/gpl/ for GPL licensing information.
**
** Contact org@leaderit.ru if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef AMETADATAIO_H
#define AMETADATAIO_H

#include <QObject>
#include "ametadata.h"

#include <QDateTime>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QDomDocument>

class AMetaDataIO: public QObject
{
Q_OBJECT
public:
    AMetaDataIO();
    ~AMetaDataIO();
    virtual int read( const QString &name, AMetaData *md = 0);
    virtual int write( const QString &name, AMetaData *md = 0);
    virtual AMetaObject *createObject( const QString &className, AMetaObject *parent = 0 ); 
    //virtual void storeGroup( AMetaObjectGroup *g );
//    virtual void storeGroupBegin( AMetaObjectGroup *g );

signals:
    void message( int msg_class, const QString & msg );
};

#endif
