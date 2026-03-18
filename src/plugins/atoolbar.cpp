/****************************************************************************
** $Id: atoolbar.cpp,v 1.1 2008/11/05 21:16:29 leader Exp $
**
** Code file of the Ananas Tool bar of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
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

/******************************************************************
 ******************************************************************/

//#include <qimage.h>
#include "atoolbar.h"


aToolBar::aToolBar(aCfg *cfg, aCfgItem &obj, aEngine *e, QMainWindow *parent) : QToolBar(parent)
{
	setObjectName("aToolBar");

	md = cfg;
	en = e;

	ReadTool(obj);
}

aToolBar::~aToolBar(){
}

void aToolBar::ReadTool(aCfgItem &obj)
{
    aCfgItem aobj, apix;
    QString aKey;
    long pid;

    aobj = md->firstChild(obj);

    while (!aobj.isNull())
    {
        aKey = md->sText(aobj, md_key);
        pid = md->id(aobj);

        apix = md->findChild(
            md->find(
                md->text(md->findChild(aobj, md_comaction, 0)).toLong()
            ),
            md_active_picture,
            0
        );

        QPixmap pix(md->binary(apix));

        QAction *a = new QAction(
            QIcon(pix),
            md->attr(aobj, mda_name),
            this
        );

        if (!aKey.isEmpty())
            a->setShortcut(QKeySequence(aKey));

        a->setObjectName(md->attr(aobj, mda_name));

        actions.insert(pid, a);

        this->addAction(a);

        connect(a, SIGNAL(triggered()), this, SLOT(on_Item()));

        aobj = md->nextSibling(aobj);
    }
}

void aToolBar::on_Item()
{
    QMap<int, QAction*>::iterator it;

    for (it = actions.begin(); it != actions.end(); ++it)
    {
        if (it.value() == sender())
        {
            en->on_MenuBar(it.key());
            break;
        }
    }
}
