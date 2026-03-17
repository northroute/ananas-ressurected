/****************************************************************************
** $Id: interfacetree.cpp,v 1.1 2008/11/05 21:16:27 leader Exp $
**
** Code file of the Interface Tree of Ananas Designer applications
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
#include "interfacetree.h"
#include "mainform.h"
#include "deditcommand.h"
#include "dedittoolbar.h"
#include "awindowslist.h"

extern MainForm *mainform;
extern QPixmap rcIcon(const char *name);
extern void set_Icon(QTreeWidgetItem *item, const char *name);

InterfaceListViewItem::InterfaceListViewItem(QTreeWidget *parent,
                                             aCfg *cfgmd,
                                             aCfgItem cfgobj,
                                             const QString &name)
    : ananasListViewItem(parent, cfgmd, cfgobj, name)
{
    id = md->id(obj);

    if (id && md->objClass(obj) != md_separator)
        setFlags(flags() | Qt::ItemIsEditable);

    aCfgItem comaction, active;
    QPixmap pix;
    int idd;

    comaction = md->findChild(obj, md_comaction, 0);
    if (!comaction.isNull()) {
        idd = md->text(comaction).toInt();
        active = md->findChild(md->find(idd), md_active_picture, 0);
        if (!active.isNull()) {
            pix.loadFromData(md->binary(active));
            setIcon(0, QIcon(pix));
        }
    }
}

InterfaceListViewItem::InterfaceListViewItem(ananasListViewItem *parent,
                                             ananasListViewItem *after,
                                             aCfg *cfgmd,
                                             aCfgItem cfgobj,
                                             const QString &name)
    : ananasListViewItem(parent, after, cfgmd, cfgobj, name)
{
    Q_UNUSED(after);

    id = md->id(obj);

    if (id && md->objClass(obj) != md_separator)
        setFlags(flags() | Qt::ItemIsEditable);

    aCfgItem comaction, active;
    int idd;
    QPixmap pix;

    comaction = md->findChild(obj, md_comaction, 0);
    if (!comaction.isNull()) {
        idd = md->text(comaction).toInt();
        active = md->findChild(md->find(idd), md_active_picture, 0);
        if (!active.isNull()) {
            pix.loadFromData(md->binary(active));
            setIcon(0, QIcon(pix));
        }
    }
}

void InterfaceListViewItem::loadTree()
{
    QString oclass;
    aCfgItem cobj;

    clearTree();

    if (!md) return;

    cobj = md->firstChild(obj);
    while (!cobj.isNull())
    {
        oclass = md->objClass(cobj);

        if (oclass == md_submenu || oclass == md_popupmenu || oclass == md_toolbar)
            loadSubmenu(this, 0, cobj);
        else if (oclass == md_command)
            loadCommand(this, 0, cobj);
        else if (oclass == md_separator)
            loadSeparator(this, 0, cobj);

        cobj = md->nextSibling(cobj);
    }
}

void InterfaceListViewItem::loadSubmenu(ananasListViewItem *parent, ananasListViewItem *after, aCfgItem child)
{
    Q_UNUSED(after);

    InterfaceListViewItem *mparent;
    QString oclass;
    aCfgItem cobj;

    mparent = new InterfaceListViewItem(parent, 0, md, child, QString());
    mparent->setIcon(0, QIcon(rcIcon("submenu.png")));

    cobj = md->firstChild(mparent->obj);
    while (!cobj.isNull())
    {
        oclass = md->objClass(cobj);

        if (oclass == md_submenu || oclass == md_toolbar)
            loadSubmenu(mparent, 0, cobj);
        else if (oclass == md_command)
            loadCommand(mparent, 0, cobj);
        else if (oclass == md_separator)
            loadSeparator(mparent, 0, cobj);

        cobj = md->nextSibling(cobj);
    }
}


void InterfaceListViewItem::loadCommand(ananasListViewItem *parent, ananasListViewItem *after, aCfgItem child)
{
    Q_UNUSED(after);
    new InterfaceListViewItem(parent, 0, md, child, QString());
}


void InterfaceListViewItem::loadSeparator(ananasListViewItem *parent, ananasListViewItem *after, aCfgItem child)
{
    Q_UNUSED(after);

    InterfaceListViewItem *newitem;
    newitem = new InterfaceListViewItem(parent, 0, md, child, QObject::tr("-------------------"));
    newitem->setIcon(0, QIcon(rcIcon("separator.png")));
}


void InterfaceListViewItem::newCommand()
{
    aCfgItem newobj;
    InterfaceListViewItem *newitem;
    QString oclass = md->objClass(obj);

    if (oclass == md_popupmenu || oclass == md_submenu ||
        oclass == md_mainmenu || oclass == md_toolbar)
    {
        setSelected(false);
        setExpanded(true);

        newobj = md->insert(obj, md_command, QObject::tr("New Command"));
        newitem = new InterfaceListViewItem(this, 0, md, newobj);
        newitem->edit();
    }
}

void InterfaceListViewItem::newSubmenu()
{
    aCfgItem newobj;
    InterfaceListViewItem *newitem;
    QString oclass = md->objClass(obj);

    if (oclass == md_submenu || oclass == md_mainmenu ||
        oclass == md_toolbars || oclass == md_popupmenus || oclass == md_popupmenu)
    {
        setSelected(false);
        setExpanded(true);

        if (oclass == md_popupmenus)
            newobj = md->insert(obj, md_popupmenu, QObject::tr("New Popup menu"));
        else if (oclass == md_toolbars)
            newobj = md->insert(obj, md_toolbar, QObject::tr("New tool bar"));
        else
            newobj = md->insert(obj, md_submenu, QObject::tr("New Submenu"));

        newitem = new InterfaceListViewItem(this, 0, md, newobj);
        newitem->setIcon(0, QIcon(rcIcon("submenu.png")));
    }
}

void InterfaceListViewItem::newSeparator()
{
    aCfgItem newobj;
    InterfaceListViewItem *newitem;
    QString oclass = md->objClass(obj);

    if (oclass == md_submenu || oclass == md_mainmenu || oclass == md_popupmenu)
    {
        setSelected(false);
        setExpanded(true);

        newobj = md->insert(obj, md_separator);
        newitem = new InterfaceListViewItem(this, 0, md, newobj, QObject::tr("-------------------"));
        newitem->setIcon(0, QIcon(rcIcon("separator.png")));
    }
}

void InterfaceListViewItem::edit()
{
    QMdiArea *ws = mainform->ws;
    aWindowsList *wl = mainform->wl;
    QString oclass = md->objClass(obj);
    int objid = md->id(obj);

    if (wl->find(objid)) {
        wl->get(objid)->setFocus();
        return;
    }

    if (oclass == md_command)
    {
        dEditCommand *e = new dEditCommand(ws, 0);
        wl->insert(objid, e);
        editor = e;
        QObject::connect(mainform, SIGNAL(tosave()), editor, SLOT(updateMD()));
        e->setData(this);
        e->show();
        mainform->addTab(e);
        return;
    }
}


InterfaceTreeView::InterfaceTreeView(QWidget *parent, aCfg *cfgmd)
    : ananasTreeView(parent, cfgmd)
{
    InterfaceListViewItem *mainmenu, *toolbars, *popups;
    aCfgItem iface, item;

    iface = md->find(mdc_interface);
    if (iface.isNull())
        iface = md->insert(md->find(mdc_root), md_interface, QString(), -1);

    item = md->find(iface, md_toolbars);
    if (item.isNull())
        item = md->insert(iface, md_toolbars, QString(), -1);
    toolbars = new InterfaceListViewItem(this, md, item, QObject::tr("Toolbars"));
    toolbars->setIcon(0, QIcon(rcIcon("toolbar.png")));
    toolbars->loadTree();
    toolbars->setExpanded(true);

    item = md->find(iface, md_mainmenu);
    if (item.isNull())
        item = md->insert(iface, md_mainmenu, QString(), -1);
    mainmenu = new InterfaceListViewItem(this, md, item, QObject::tr("Main menu"));
    mainmenu->setIcon(0, QIcon(rcIcon("m_menu.png")));
    mainmenu->loadTree();
    mainmenu->setExpanded(true);

    item = md->find(iface, md_popupmenus);
    if (item.isNull())
        item = md->insert(iface, md_popupmenus, QString(), -1);
    popups = new InterfaceListViewItem(this, md, item, QObject::tr("Popup menus"));
    popups->setIcon(0, QIcon(rcIcon("p_menus.png")));
    popups->loadTree();
    popups->setExpanded(true);

    connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(itemEdit()));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(itemEdit()));
}

void InterfaceTreeView::ContextMenu()
{
    QMenu *m = new QMenu(this);
    Q_CHECK_PTR(m);

    ananasTreeView::ContextMenuAdd(m);

    QAction *actSubmenu = m->addAction(tr("&New Submenu"));
    actSubmenu->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    QObject::connect(actSubmenu, SIGNAL(triggered()), this, SLOT(itemNewSubmenu()));

    QAction *actCommand = m->addAction(tr("New &Command"));
    actCommand->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    QObject::connect(actCommand, SIGNAL(triggered()), this, SLOT(itemNewCommand()));

    QAction *actSeparator = m->addAction(tr("New &Separator"));
    actSeparator->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    QObject::connect(actSeparator, SIGNAL(triggered()), this, SLOT(itemNewSeparator()));

    m->exec(QCursor::pos());
    delete m;
}


void InterfaceTreeView::itemNewSubmenu()
{
    InterfaceListViewItem *i =
        static_cast<InterfaceListViewItem*>(currentItem());

    if (i)
        i->newSubmenu();
}


void InterfaceTreeView::itemNewCommand()
{
    InterfaceListViewItem *i =
        static_cast<InterfaceListViewItem*>(currentItem());

    if (i)
        i->newCommand();
}


void InterfaceTreeView::itemNewSeparator()
{
    InterfaceListViewItem *i =
        static_cast<InterfaceListViewItem*>(currentItem());

    if (i)
        i->newSeparator();
}

void
InterfaceTreeView::itemDelete()
{
	deleteItem();
};

void
InterfaceTreeView::itemMoveUp()
{
	moveUpItem();
};

void
InterfaceTreeView::itemMoveDown()
{
	moveDownItem();
};

void InterfaceTreeView::itemEdit()
{
    InterfaceListViewItem *i =
        static_cast<InterfaceListViewItem*>(currentItem());

    if (i)
        i->edit();
}

void InterfaceTreeView::itemRename()
{
	renameItem();
}
