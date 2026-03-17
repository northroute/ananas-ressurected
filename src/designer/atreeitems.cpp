/****************************************************************************
** $Id: atreeitems.cpp,v 1.2 2008/11/08 20:16:35 leader Exp $
**
** Header file of the Ananas visual tree object
** of Ananas Designer applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
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

#include "atreeitems.h"
#include "alog.h"

ananasListViewItem::ananasListViewItem( QTreeWidget *parent, aCfg * cfgmd, aCfgItem cfgobj,
										 const QString &name )
: QTreeWidgetItem( parent )
{
	obj = cfgobj;
	md = cfgmd;
	if ( name.isNull() ) setText( 0, md->attr( obj, mda_name ) );
	else setText( 0, name );
	id = md->id(obj);
}

ananasListViewItem::ananasListViewItem( ananasListViewItem *parent, ananasListViewItem *after,
										 aCfg * cfgmd, aCfgItem cfgobj, const QString &name )
: QTreeWidgetItem( parent, after )
{
	obj = cfgobj;
	md = cfgmd;
	if ( name.isNull() ) setText( 0, md->attr( obj, mda_name ) );
	else setText( 0, name );
	id = md->id(obj);
}

ananasListViewItem::ananasListViewItem( QTreeWidget *parent, QTreeWidgetItem *after, aCfg * cfgmd, aCfgItem cfgobj, const QString &name )
: QTreeWidgetItem( parent, after )
{
	obj = cfgobj;
	md = cfgmd;
	if ( name.isNull() ) setText( 0, md->attr( obj, mda_name ) );
	else setText( 0, name );
	id = md->id(obj);
}

void ananasListViewItem::clearTree()
{
    while (childCount() > 0) {
        delete takeChild(0);
    }
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

void ananasListViewItem::moveUp()
{
    ananasListViewItem *prev = previousSibling();
    if (!prev) return; // no previous item

    aCfgItem item = prev->obj;

    if (obj.isNull())
    {
        aLog::print(aLog::Error,
                    QObject::tr(" Ananas List View Item %1 is null").arg(md->attr(obj, mda_name)));
        return;
    }

    if (md->swap(obj, item))
    {
        QTreeWidgetItem *p = parent();
        if (p) {
            int idx = p->indexOfChild(this);
            if (idx > 0) {
                p->takeChild(idx);
                p->insertChild(idx - 1, this);
            }
        }
        aLog::print(aLog::Debug, QObject::tr("Ananas List View Item swaping"));
    }
    else
    {
        aLog::print(aLog::Error, QObject::tr("Ananas List View Item swaping"));
    }
}

void ananasListViewItem::moveDown()
{
    ananasListViewItem *next = nextSibling();
    if (!next) return; // no next item

    aCfgItem item = next->obj;
    if (item.isNull())
    {
        aLog::print(aLog::Error,
                    QObject::tr(" Ananas List View Item %1 is null").arg(md->attr(obj, mda_name)));
        return;
    }

    if (md->swap(obj, item))
    {
        QTreeWidgetItem *p = parent();
        if (p) {
            int idx = p->indexOfChild(this);
            if (idx >= 0 && idx < p->childCount() - 1) {
                p->takeChild(idx);
                p->insertChild(idx + 1, this);
            }
        }
        aLog::print(aLog::Debug, QObject::tr("Ananas List View Item swaping"));
    }
    else
    {
        aLog::print(aLog::Error, QObject::tr("Ananas List View Item swaping"));
    }
}

ananasListViewItem *ananasListViewItem::previousSibling()
{
    QTreeWidgetItem *p = parent();
    if (!p) return 0;

    int idx = p->indexOfChild(this);
    if (idx > 0)
        return static_cast<ananasListViewItem*>(p->child(idx - 1));

    return 0;
}

ananasListViewItem *ananasListViewItem::nextSibling()
{
    QTreeWidgetItem *p = parent();
    if (!p) return 0;

    int idx = p->indexOfChild(this);
    if (idx < p->childCount() - 1)
        return static_cast<ananasListViewItem*>(p->child(idx + 1));

    return 0;
}

void ananasListViewItem::okRename(int col)
{
    if (id && !obj.isNull() && col == 0) {
        QString t = text(0).trimmed();
        setText(0, t);
        md->setAttr(obj, mda_name, t);
    }
}

/*
ananasListViewItem*
ananasListViewItem::getLastChild( QListViewItem * parent )
{
	QListViewItem *item, *nextitem;
	item = parent->firstChild();
	while( item )
	{
		nextitem = item->nextSibling();
		if ( nextitem )
			item = nextitem;
		else
			return (ananasListViewItem*) item;
	}
	return 0;
};
*/

ananasListViewItem *ananasListViewItem::getLastChild()
{
    int count = childCount();
    if (count > 0)
        return static_cast<ananasListViewItem*>(child(count - 1));
    return 0;
}


ananasTreeView::ananasTreeView(QWidget *parent, aCfg *cfgmd)
    : QTreeWidget(parent)
{
    md = cfgmd;

    setColumnCount(1);
    header()->hide();
    setSortingEnabled(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void ananasTreeView::ContextMenuAdd(QMenu *m)
{
    m->addAction(tr("&Rename"), this, SLOT(itemRename()), QKeySequence(Qt::CTRL + Qt::Key_R));
    m->addAction(tr("&Edit"), this, SLOT(itemEdit()), QKeySequence(Qt::CTRL + Qt::Key_E));
    m->addAction(tr("&Delete"), this, SLOT(itemDelete()), QKeySequence(Qt::CTRL + Qt::Key_D));
    m->addAction(tr("&MoveUp"), this, SLOT(itemMoveUp()), QKeySequence(Qt::CTRL + Qt::Key_U));
    m->addAction(tr("&MoveDown"), this, SLOT(itemMoveDown()), QKeySequence(Qt::CTRL + Qt::Key_M));
    m->addAction(tr("&SaveItem"), this, SLOT(itemSave()));
    m->addAction(tr("&LoadItem"), this, SLOT(itemLoad()));
    m->addSeparator();
}

void ananasTreeView::deleteItem()
{
    ananasListViewItem *i = static_cast<ananasListViewItem*>(currentItem());
    if (i && i->id) {
        md->remove(i->obj);
        delete i;
    }
}

void ananasTreeView::moveUpItem()
{
    ananasListViewItem *i = static_cast<ananasListViewItem*>(currentItem());
    if (i && i->id) {
        i->moveUp();
    }
}

void ananasTreeView::moveDownItem()
{
    ananasListViewItem *i = static_cast<ananasListViewItem*>(currentItem());
    if (i && i->id) {
        i->moveDown();
    }
}

void ananasTreeView::renameItem()
{
    ananasListViewItem *i = static_cast<ananasListViewItem*>(currentItem());
    if (i)
        editItem(i, 0);
}
