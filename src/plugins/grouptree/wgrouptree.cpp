/****************************************************************************
** $Id: wgrouptree.cpp,v 1.1 2008/11/05 21:16:30 leader Exp $
**
** Header file of the document plugin of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
**
** This file is part of the Ananas Plugins of the Ananas
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

#include "wgrouptree.h"
#include "awidget.h"

wGroupTreeItem::wGroupTreeItem(QTreeWidget *parent, const QString &name)
    : QTreeWidgetItem(parent)
{
    level = -1;
    id = 0;

    setText(0, name);
}


wGroupTreeItem::wGroupTreeItem(wGroupTreeItem *parent,
                               wGroupTreeItem *after,
                               const QString &name,
                               int newlevel,
                               ANANAS_UID newid)
    : QTreeWidgetItem(parent)
{
    level = newlevel;
    id = newid;

    setText(0, name);

    if (after)
        parent->insertChild(parent->indexOfChild(after) + 1, this);
}


wGroupTreeItem::wGroupTreeItem(wGroupTreeItem *parent,
                               wGroupTreeItem *after,
                               aCatGroup *g)
    : QTreeWidgetItem(parent)
{
    level = g->Value("Level").toInt();
    id = g->getUid();

    setText(0, g->displayString());

    if (after)
        parent->insertChild(parent->indexOfChild(after) + 1, this);
}


wGroupTreeItem::~wGroupTreeItem()
{
}



wGroupTreeItem *
wGroupTreeItem::parentItem()
{
	return ( wGroupTreeItem *) parent();
}


wGroupTree::wGroupTree(QWidget *parent, Qt::WFlags fl)
    : aWidget(parent, "wGroupTree", fl)
{
    tree = new QTreeWidget(this);
    tree->setColumnCount(1);
    tree->header()->hide();
    tree->setSortingEnabled(false);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);

    root = new wGroupTreeItem(tree, "ROOT");
    root->setExpanded(true);
    root->setIcon(0, QIcon(rcIcon("wcatalogue.png")));

    QGridLayout *l = new QGridLayout(this);
    l->addWidget(tree, 0, 0);

    connect(tree, SIGNAL(itemSelectionChanged()),
            this, SLOT(on_selectionChanged()));
}


wGroupTree::~wGroupTree()
{
}


void
wGroupTree::initObject( aDatabase *adb )
{
	aWidget::initObject( adb );
	findGroupTree();
	//connectSlots();
}


QDialog*
wGroupTree::createEditor( QWidget * )//parent )
{
    return 0; //new eCatalogue( parent );
}



/*!
 * Create aDocument database object.
 */
aObject *
wGroupTree::createDBObject(  aCfgItem obj, aDatabase *adb )
{
	return new aCatGroup( obj, adb );
}

void wGroupTree::buildGroupTree(aCfgItem obj, aCatGroup *cg1, wGroupTreeItem *wG)
{
    aCatGroup cg2(obj, db);
    QString t;
    int err = 0;
    wGroupTreeItem *item = 0;

    CHECK_POINT

    err = cg2.SelectChild(cg1);
    if (!err)
    {
        do
        {
            t = cg2.Value("Name").toString();
            Q_UNUSED(t);

            item = new wGroupTreeItem(wG, 0, &cg2);
            item->setIcon(0, QIcon(rcIcon("t_cat_g.png")));

            buildGroupTree(obj, &cg2, item);
        }
        while (cg2.Next());
    }
    else
    {
        printf("buildGroupTree err = %d\n", err);
    }
}

void wGroupTree::findGroupTree()
{
    aCfgItem tab, cat;
    Q_UNUSED(tab);

    if (!db || !md)
        return;

    CHECK_POINT

    aWidget *pc = parentContainer(this);
    if (!pc)
        return;

    CHECK_POINT

    printf("inserted in %s\n", pc->metaObject()->className());

    if (QString(pc->metaObject()->className()) == QString("wCatalogue"))
    {
        cat = md->find(pc->getId());
        root->setText(0, md->attr(cat, mda_name));

        buildGroupTree(cat, 0, root);

        CHECK_POINT
    }
}

/*!
 *\~english
 *
 *\~russian
 * ������ ������������� �����.
 *\~
 */
void wGroupTree::keyPressEvent(QKeyEvent *e)
{
    ANANAS_UID id = 0;
    wGroupTreeItem *item = static_cast<wGroupTreeItem *>(tree->currentItem());

    printf("key=%04x\n", e->key());

    switch (e->key())
    {
    case Qt::Key_Return:
        if (item)
            id = item->id;

        if (e->modifiers() & Qt::ShiftModifier)
        {
            printf("Shift+Return pressed %llu\n", (unsigned long long)id);
            if (id)
                EditGroup();
        }
        else
        {
            printf("Return pressed %llu\n", (unsigned long long)id);
            emit selected(id);
        }

        e->accept();
        break;

    case Qt::Key_Escape:
        break;

    case Qt::Key_Delete:
        if (item)
        {
            id = item->id;
            if (id)
                DeleteGroup();
        }
        break;

    case Qt::Key_Insert:
        NewGroup();
        item = static_cast<wGroupTreeItem *>(tree->currentItem());
        if (item)
        {
            id = item->id;
            if (id)
                EditGroup();
        }
        break;

    default:
        e->ignore();
        break;
    }

    aWidget::keyPressEvent(e);
}

void wGroupTree::NewGroup()
{
    wGroupTreeItem *item = 0;
    wGroupTreeItem *cur = static_cast<wGroupTreeItem *>(tree->currentItem());
    QString t = "Element";

    if (!cur)
        cur = root;

    int level = cur->level + 1;

    ((aCatGroup *)dbobj)->New();
    dbobj->SetValue("Name", t);
    dbobj->setSysValue("level", level);
    dbobj->setSysValue("idp", cur->id);
    dbobj->Update();

    item = new wGroupTreeItem(cur, 0, (aCatGroup *)dbobj);
    item->id = dbobj->getUid();

    cur->setExpanded(true);
    tree->setCurrentItem(item);
}

void
wGroupTree::DeleteGroup()
{
CHECK_POINT
	aCatGroup g( *getMDObject(), db);
	wGroupTreeItem * item = ( wGroupTreeItem * ) tree->currentItem();
	if(item!=root)
	{
		g.select( item->id );
		g.Delete();
		item->id=0;
		item->setText(0,"X");
	}
}


void
wGroupTree::EditGroup()
{
	ANANAS_UID id = 0;
	wGroupTreeItem * item = ( wGroupTreeItem * ) tree->currentItem();
	if(item!=root)
	{
		aForm * f = 0;
		if ( item ) id = item->id;
CHECK_POINT
		if ( id ) {
			if ( engine ) {
				f = engine->openForm( parentContainer( this )->getId(), 0, md_action_edit, md_form_group, id, this );
				if ( f ) {
					f->SelectGroup(id);
					connect(f, SIGNAL( update( ANANAS_UID )), this, SLOT(updateItem( ANANAS_UID )));
//				connect(f, SIGNAL(selected( qulonglong )), this, SLOT(on_selected( qulonglong )));
//				f->closeAfterSelect = true;
				}
			} else printf("No Engine\n");

		}
	}
}


//void
//wGroupTree::UpdateItem( aCatGroup *g )
//{
//}


/*!
 * Show appropriated group after catalogue item selected.
 */
void
wGroupTree::on_selected( ANANAS_UID element )
{

}


/*!
 * Update item after edit.
 */
void wGroupTree::updateItem(ANANAS_UID element)
{
    wGroupTreeItem *i = 0;
    aCatGroup g(*getMDObject(), db);

    printf("Need update text %llu\n", (unsigned long long)element);

    i = findItem(element);
    if (i)
    {
        g.select(element);

        printf("founded text %llu %s\n",
               (unsigned long long)element,
               qPrintable(i->text(0)));

        i->setText(0, g.displayString());
    }
}


wGroupTreeItem *wGroupTree::findItem(ANANAS_UID id)
{
    QTreeWidgetItemIterator it(tree);

    while (*it)
    {
        wGroupTreeItem *item = dynamic_cast<wGroupTreeItem *>(*it);
        if (item && item->id == id)
            return item;

        ++it;
    }

    return 0;
}


/*!
 * Create toolbar for Journal.
 */
QToolBar* wGroupTree::createToolBar(QMainWindow *owner)
{
    QAction *a;
    QToolBar *t = new QToolBar("GroupTreeTools", owner);

    a = new QAction(rcIcon("doc_new.png"), tr("New"), t);
    a->setToolTip(tr("New group (Ins)"));
    t->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(NewGroup()));

    a = new QAction(rcIcon("doc_edit.png"), tr("Edit"), t);
    a->setToolTip(tr("Edit group (Enter)"));
    t->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(EditGroup()));

    a = new QAction(rcIcon("doc_delete.png"), tr("Delete group"), t);
    a->setToolTip(tr("Delete group (Delete)"));
    t->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(DeleteGroup()));

    return t;
}

void wGroupTree::on_selectionChanged()
{
    QTreeWidgetItem *item = tree->currentItem();
    if (!item)
        return;

    wGroupTreeItem *gitem = static_cast<wGroupTreeItem *>(item);
    emit selected(gitem->id);
}
