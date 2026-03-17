/****************************************************************************
** $Id: roleeditor.cpp,v 1.1 2008/11/05 21:16:27 leader Exp $
**
** Code file of the Alias editor of Ananas
** Designer applications
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

#include "roleeditor.h"
#include "acfg.h"


aRoleEditor::aRoleEditor(aCfg *c, aCfgItem o, QTableWidget *t, const char *p)
{
    ac = c;
    obj = o;
    tRoles = t;
    parent = p;

    tRoles->setRowCount(0);
    tRoles->setColumnCount(1);
    tRoles->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Read")));

    if (QString(parent) == md_document) {
        tRoles->setColumnCount(5);
        tRoles->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Write")));
        tRoles->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Delete")));
        tRoles->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("Turn On")));
        tRoles->setHorizontalHeaderItem(4, new QTableWidgetItem(tr("Turn Off")));
    }

    if (QString(parent) == md_catalogue) {
        tRoles->setColumnCount(3);
        tRoles->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Write")));
        tRoles->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Delete")));
    }
}

aRoleEditor::~aRoleEditor()
{
}

void aRoleEditor::setData()
{
    int i, j, n;
    aCfgItem orole, roles, role;
    QString rolename, right;

    roles = ac->find(ac->find(mdc_root), md_roles, 0);
    roleCount = ac->count(roles, md_role);
    n = ac->countChild(obj, md_role);

    tRoles->setRowCount(roleCount);

    for (i = 0; i < roleCount; ++i) {
        role = ac->findChild(roles, md_role, i);
        rolename = ac->attr(role, mda_name);

        QTableWidgetItem *headerItem = new QTableWidgetItem(rolename);
        tRoles->setVerticalHeaderItem(i, headerItem);

        QTableWidgetItem *r = new QTableWidgetItem();
        r->setFlags(r->flags() | Qt::ItemIsUserCheckable);
        r->setCheckState(Qt::Unchecked);
        tRoles->setItem(i, 0, r);

        QTableWidgetItem *w = new QTableWidgetItem();
        w->setFlags(w->flags() | Qt::ItemIsUserCheckable);
        w->setCheckState(Qt::Unchecked);

        QTableWidgetItem *d = new QTableWidgetItem();
        d->setFlags(d->flags() | Qt::ItemIsUserCheckable);
        d->setCheckState(Qt::Unchecked);

        QTableWidgetItem *on = new QTableWidgetItem();
        on->setFlags(on->flags() | Qt::ItemIsUserCheckable);
        on->setCheckState(Qt::Unchecked);

        QTableWidgetItem *off = new QTableWidgetItem();
        off->setFlags(off->flags() | Qt::ItemIsUserCheckable);
        off->setCheckState(Qt::Unchecked);

        if (QString(parent) == md_catalogue) {
            tRoles->setItem(i, 1, w);
            tRoles->setItem(i, 2, d);
        }

        if (QString(parent) == md_document) {
            tRoles->setItem(i, 1, w);
            tRoles->setItem(i, 2, d);
            tRoles->setItem(i, 3, on);
            tRoles->setItem(i, 4, off);
        }

        for (j = 0; j < n; ++j) {
            orole = ac->findChild(obj, md_role, j);
            if (rolename == ac->attr(orole, mda_name)) {
                right = ac->attr(orole, mda_rights);

                if (right.contains("-r"))   r->setCheckState(Qt::Checked);
                if (right.contains("-w"))   w->setCheckState(Qt::Checked);
                if (right.contains("-d"))   d->setCheckState(Qt::Checked);
                if (right.contains("-on"))  on->setCheckState(Qt::Checked);
                if (right.contains("-off")) off->setCheckState(Qt::Checked);
            }
        }
    }
}

void aRoleEditor::updateMD()
{
    int i;
    aCfgItem role;

    do {
        role = ac->findChild(obj, md_role, 0);
        if (!role.isNull())
            ac->remove(role);
    } while (!role.isNull());

    for (i = 0; i < tRoles->rowCount(); ++i) {
        QString right;

        role = ac->insert(obj, md_role, tRoles->item(i, 0) ? tRoles->item(i, 0)->text() : QString(), -1);
        ac->setAttr(role, mda_name, tRoles->verticalHeaderItem(i) ? tRoles->verticalHeaderItem(i)->text() : QString());

        QTableWidgetItem *q = tRoles->item(i, 4);
        if (q && q->checkState() == Qt::Checked) right.insert(0, "-off");

        q = tRoles->item(i, 3);
        if (q && q->checkState() == Qt::Checked) right.insert(0, "-on");

        q = tRoles->item(i, 2);
        if (q && q->checkState() == Qt::Checked) right.insert(0, "-d");

        q = tRoles->item(i, 1);
        if (q && q->checkState() == Qt::Checked) right.insert(0, "-w");

        q = tRoles->item(i, 0);
        if (q && q->checkState() == Qt::Checked) right.insert(0, "-r");

        ac->setAttr(role, mda_rights, right);
    }
}
