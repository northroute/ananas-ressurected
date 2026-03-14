/****************************************************************************
** $Id: atreeitems.cpp,v 1.2 2008/12/05 21:11:54 leader Exp $
**
** Header file of the Ananas visual tree object
** of Ananas Designer applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
** Copyright (C) 2003-2005 Grigory Panov <gr1313 at mail dot ru>, Yoshkar-Ola.
**
** This file is part of the Designer application  of the Ananas
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

//#include <qlistview.h>
//#include <qheader.h>
//#include <qpopupmenu.h>

#include "atreeitems.h"

ananasListViewItem::ananasListViewItem(QTreeWidget *parent,
                                       aCfg *cfgmd,
                                       aCfgItem cfgobj,
                                       const QString &name)
    : QTreeWidgetItem(parent)
{
    obj = cfgobj;
    md = cfgmd;

    if (name.isNull())
        setText(0, md->attr(obj, mda_name));
    else
        setText(0, name);

    id = md->id(obj);
}

ananasListViewItem::ananasListViewItem(ananasListViewItem *parent,
                                       ananasListViewItem *after,
                                       aCfg *cfgmd,
                                       aCfgItem cfgobj,
                                       const QString &name)
    : QTreeWidgetItem(parent)
{
    obj = cfgobj;
    md = cfgmd;

    if (name.isNull())
        setText(0, md->attr(obj, mda_name));
    else
        setText(0, name);

    id = md->id(obj);

    if (after)
    {
        QTreeWidgetItem *p = parent;
        int idx = p->indexOfChild(after);
        p->insertChild(idx + 1, this);
    }
}

ananasListViewItem::ananasListViewItem(QTreeWidget *parent, QTreeWidgetItem *after,
                                       aCfg *cfgmd, aCfgItem cfgobj, const QString &name)
    : QTreeWidgetItem(parent)
{
    obj = cfgobj;
    md = cfgmd;

    if (name.isNull())
        setText(0, md->attr(obj, mda_name));
    else
        setText(0, name);

    id = md->id(obj);

    if (after) {
        QTreeWidgetItem *p = parent->invisibleRootItem();
        int idx = p->indexOfChild(after);
        p->insertChild(idx + 1, this);
    }
}

void ananasListViewItem::clearTree()
{
    while (childCount() > 0)
        delete child(0);
}

/*
void
ananasListViewItem::moveItem ( QListViewItem * after )
{
CHECK_POINT
	aCfgItem temp = obj, temp2 = ((ananasListViewItem*)after)->obj;
	QListViewItem::moveItem( after );
	obj = temp;
	((ananasListViewItem*)after)->obj = temp2;
}
*/

ananasListViewItem *ananasListViewItem::previousSibling()
{
    QTreeWidgetItem *p = parent();
    if (!p) return 0;

    int idx = p->indexOfChild(this);
    if (idx <= 0) return 0;

    return (ananasListViewItem*) p->child(idx - 1);
}

ananasListViewItem *ananasListViewItem::nextSibling()
{
    QTreeWidgetItem *p = parent();
    if (!p) return 0;

    int idx = p->indexOfChild(this);
    if (idx < 0 || idx + 1 >= p->childCount()) return 0;

    return (ananasListViewItem*) p->child(idx + 1);
}


ananasListViewItem *ananasListViewItem::getLastChild()
{
    int n = childCount();
    if (n == 0)
        return 0;

    return (ananasListViewItem*) child(n - 1);
}


ananasTreeView::ananasTreeView(QWidget *parent, aCfg *cfgmd) : QTreeWidget(parent)
{
    md = cfgmd;

    setColumnCount(1);
    header()->hide();

    setSortingEnabled(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
}


void
ananasTreeView::ContextMenuAdd( QMenu * m )
{
	//m->insertSeparator();
};




