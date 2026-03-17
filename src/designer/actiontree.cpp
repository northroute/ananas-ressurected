/****************************************************************************
** $Id: actiontree.cpp,v 1.1 2008/11/05 21:16:26 leader Exp $
**
** Code file of the Action Tree of Ananas Designer applications
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

#include "acfg.h"
#include "actiontree.h"
#include "mainform.h"
#include "deditaction.h"
#include "awindowslist.h"

extern MainForm *mainform;
extern QPixmap ANANAS_EXPORT rcIcon(const char *name);
extern void set_Icon(QTreeWidgetItem *item, const char *name);


ActionListViewItem::ActionListViewItem(QTreeWidget *parent,
                                       aCfg *cfgmd,
                                       aCfgItem cfgobj,
                                       const QString &name)
    : ananasListViewItem(parent, cfgmd, cfgobj, name)
{
    if (id)
        setFlags(flags() | Qt::ItemIsEditable);
}

ActionListViewItem::ActionListViewItem(ananasListViewItem *parent,
                                       ananasListViewItem *after,
                                       aCfg *cfgmd,
                                       aCfgItem cfgobj,
                                       const QString &name)
    : ananasListViewItem(parent, after, cfgmd, cfgobj, name)
{
    aCfgItem active;
    QPixmap pix;

    if (id)
        setFlags(flags() | Qt::ItemIsEditable);

    active = md->findChild(obj, md_active_picture, 0);
    if (!active.isNull()) {
        pix.loadFromData(md->binary(active));
        setIcon(0, pix);
    }
}

void ActionListViewItem::loadTree()
{
    QString oclass;
    aCfgItem cobj;

    // clear tree
    clearTree();
    if (!md) return;

    setIcon(0, rcIcon("actions.png"));

    cobj = md->firstChild(obj);
    while (!cobj.isNull())
    {
        oclass = md->objClass(cobj);
        if (oclass == md_actiongroup)
            loadGroup(this, cobj);

        oclass = md->objClass(cobj);
        if (oclass == md_action)
            new ActionListViewItem(this, 0, md, cobj, QString());

        cobj = md->nextSibling(cobj);
    }
}

void ActionListViewItem::loadGroup(ananasListViewItem *parent, aCfgItem child)
{
    ActionListViewItem *gparent;
    QString oclass;
    aCfgItem cobj;

    gparent = new ActionListViewItem(parent, 0, md, child, QString());
    gparent->setIcon(0, rcIcon("action_g.png"));

    cobj = md->firstChild(gparent->obj);
    while (!cobj.isNull())
    {
        oclass = md->objClass(cobj);
        if (oclass == md_actiongroup)
            loadGroup(gparent, cobj);

        oclass = md->objClass(cobj);
        if (oclass == md_action)
            new ActionListViewItem(gparent, 0, md, cobj, QString());

        cobj = md->nextSibling(cobj);
    }
}

void ActionListViewItem::newActionGroup()
{
    aCfgItem newobj;
    ActionListViewItem *newitem;
    QString oclass = md->objClass(obj);

    if (oclass == md_actiongroup || oclass == md_actions)
    {
        setSelected(false);
        setExpanded(true);

        newobj = md->insert(obj, md_actiongroup, QObject::tr("New Action Group"));
        newitem = new ActionListViewItem(this, 0, md, newobj);
        newitem->setSelected(true);
        newitem->setIcon(0, rcIcon("action_g.png"));
        newitem->edit();
    }
}

void
ActionListViewItem::newAction()
{
    aCfgItem newobj;
    ActionListViewItem *newitem;
    QString oclass = md->objClass(obj);

    if (oclass == md_actiongroup || oclass == md_actions)
    {
        setSelected(false);
        setExpanded(true);

        newobj = md->insert(obj, md_action, QObject::tr("New Action"));
        newitem = new ActionListViewItem(this, 0, md, newobj);
        newitem->setSelected(true);
        newitem->edit();
    }
}

void
ActionListViewItem::edit()
{
    QMdiArea *ws = mainform->ws;
    aWindowsList *wl = mainform->wl;
    QString oclass = md->objClass(obj);
    int objid = md->id(obj);

    if (wl->find(objid)) {
        wl->get(objid)->setFocus();
        return;
    }

    if (oclass == md_action)
    {
        dEditAction *e = new dEditAction(mainform->ws, 0);
        e->setAttribute(Qt::WA_DeleteOnClose);
        wl->insert(objid, e);
        editor = e;
        QObject::connect(mainform, SIGNAL(tosave()), editor, SLOT(updateMD()));
        e->setData(this);
        e->show();
        mainform->addTab(e);
        return;
    }
}

aActionTreeView::aActionTreeView(QWidget *parent, aCfg *cfgmd)
    : ananasTreeView(parent, cfgmd)
{
    ActionListViewItem *actions;
    aCfgItem item;
    item = md->find(mdc_actions);
    if (item.isNull())
    {
        item = md->insert(md->find(mdc_root), md_actions, QString(), -1);
    }

    actions = new ActionListViewItem(this, md, item, QObject::tr("Actions"));
    actions->loadTree();
    actions->setExpanded(true);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ContextMenu()));
    connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this, SLOT(itemEdit()));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
            this, SLOT(itemEdit()));
}

void aActionTreeView::ContextMenu()
{
    QMenu *m = new QMenu(this);
    Q_CHECK_PTR(m);

    ContextMenuAdd(m);
    m->addAction(QObject::tr("New &Group"), this, SLOT(itemNewGroup()), QKeySequence(Qt::CTRL + Qt::Key_G));
    m->addAction(QObject::tr("New &Action"), this, SLOT(itemNewAction()), QKeySequence(Qt::CTRL + Qt::Key_A));
//  m->addAction(QObject::tr("&Rename"), this, SLOT(itemRename()), QKeySequence(Qt::CTRL + Qt::Key_R));
//  m->addAction(QObject::tr("&Edit"), this, SLOT(itemEdit()), QKeySequence(Qt::CTRL + Qt::Key_E));
//  m->addAction(QObject::tr("&Delete"), this, SLOT(itemDelete()), QKeySequence(Qt::CTRL + Qt::Key_D));

    m->exec(QCursor::pos());
    delete m;
}

void aActionTreeView::itemNewGroup()
{
    ActionListViewItem *i = static_cast<ActionListViewItem*>(currentItem());
    if (i)
        i->newActionGroup();
}

void aActionTreeView::itemNewAction()
{
    ActionListViewItem *i = static_cast<ActionListViewItem*>(currentItem());
    if (i)
        i->newAction();
}

void
aActionTreeView::itemDelete()
{
    deleteItem();
};

void
aActionTreeView::itemMoveUp()
{
	moveUpItem();

};

void
aActionTreeView::itemMoveDown()
{
	moveDownItem();
};

void aActionTreeView::itemEdit()
{
	ActionListViewItem *i = static_cast<ActionListViewItem*>(currentItem());
	if (i) i->edit();
}

void
aActionTreeView::itemRename()
{
	renameItem();
}
