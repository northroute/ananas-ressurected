/****************************************************************************
** $Id: wjournal.cpp,v 1.2 2008/11/08 20:16:36 leader Exp $
**
** Code file of the journal plugin of Ananas
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

#include "adatabase.h"
#include "wjournal.h"
#include "ejournal.h"
#include "eselectdoctype.h"
#include "adocjournal.h"
#include "eselectdoctype.h"
#include "wdbtable.h"
#include "alog.h"



wJournal::wJournal(QWidget *parent, Qt::WindowFlags fl)
    : aWidget(parent, "wJournal", fl)
{
    dSelectType = new eSelectDocType(this, 0);
    setInited(false);
}


wJournal::~wJournal()
{
	delete dSelectType;
	dSelectType = 0;
}


void wJournal::initObject(aDatabase *adb)
{
    eSelectDocType *d = qobject_cast<eSelectDocType *>(dSelectType);

    aWidget::initObject(adb);

    if (d)
        d->setJournal(&adb->cfg, getId());

    if (((aDocJournal*)dbobj)->type() == 0 && toolbar)
    {
        date_from = new QDateEdit(toolbar);
        date_to   = new QDateEdit(toolbar);

        QDate current = QDate::currentDate();

        date_to->setMinimumWidth(100);
        date_from->setMinimumWidth(100);

        date_to->setDate(current);
        date_from->setDate(current.addMonths(-1));

        toolbar->addWidget(date_from);
        toolbar->addWidget(date_to);

        connect(date_from, SIGNAL(dateChanged(const QDate &)),
                this, SLOT(setFilterByDate()));
        connect(date_to, SIGNAL(dateChanged(const QDate &)),
                this, SLOT(setFilterByDate()));
    }
    else
    {
        date_from = 0;
        date_to = 0;
    }

    setFilterByDate();

    QList<wDBTable*> tables = findChildren<wDBTable*>();
    uint i = 0;

    for (QList<wDBTable*>::iterator it = tables.begin(); it != tables.end(); ++it)
    {
        wDBTable *obj = *it;
        if (!obj)
            continue;

        aLog::print(aLog::Info, tr("wDBTable #%1 found ").arg(++i));

        connect(obj, SIGNAL(selectRecord(qulonglong)),
                this, SLOT(select(qulonglong)));
        connect(obj, SIGNAL(insertRequest()),
                this, SLOT(insert()));
        connect(obj, SIGNAL(updateRequest()),
                this, SLOT(update()));
        connect(obj, SIGNAL(viewRequest()),
                this, SLOT(view()));
        connect(obj, SIGNAL(deleteRequest()),
                this, SLOT(markDelete()));
    }
}


bool
wJournal::checkStructure()
{

	return false;
}


/*!
 * Create toolbar for Journal.
 */
QToolBar *wJournal::createToolBar(QMainWindow *owner)
{
    QAction *a, *b, *c, *d, *e;

    toolbar = new QToolBar("JournalTools", owner);

    a = new QAction(rcIcon("doc_new.png"), tr("New"), toolbar);
    a->setShortcut(QKeySequence("Insert"));
    a->setToolTip(tr("New document <Ins>"));
    toolbar->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(insert()));

    b = new QAction(rcIcon("doc_edit.png"), tr("Edit"), toolbar);
    b->setShortcut(QKeySequence(Qt::Key_Return));
    b->setToolTip(tr("Edit document <Enter>"));
    toolbar->addAction(b);
    connect(b, SIGNAL(triggered()), this, SLOT(update()));

    c = new QAction(rcIcon("doc_view.png"), tr("View"), toolbar);
    c->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Return));
    c->setToolTip(tr("View document <Shift+Enter>"));
    toolbar->addAction(c);
    connect(c, SIGNAL(triggered()), this, SLOT(view()));

    d = new QAction(rcIcon("doc_delete.png"), tr("Delete"), toolbar);
    d->setShortcut(QKeySequence("Del"));
    d->setToolTip(tr("Delete document <Delete>"));
    toolbar->addAction(d);
    connect(d, SIGNAL(triggered()), this, SLOT(markDelete()));

    e = new QAction(rcIcon("doc_copy.png"), tr("Copy"), toolbar);
    e->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    e->setToolTip(tr("Duplicate document <Ctrl+D>"));
    toolbar->addAction(e);
    connect(e, SIGNAL(triggered()), this, SLOT(copy()));

    return toolbar;
}

QDialog *wJournal::createEditor(QWidget *parent)
{
    return new eJournal(parent, 0);
}

int
wJournal::select( qulonglong id )
{
	if ( !dbobj ) return err_abstractobj;
	docUid = dbobj->docId();
//	docUid = 0;
	docId = db->uidType(docUid);
//	printf("selected record uid = %llu, doc uid = %llu doc md id %i\n", id, docUid, docId );
    return 0;
}



qulonglong
wJournal::insert()
{
	aForm * f = 0;
	int md_id = 0;
	eSelectDocType *d = ( eSelectDocType *)dSelectType;

	if ( dSelectType->exec() == QDialog::Accepted ) {
		md_id = d->docId;
		if (  md_id > 0 ){
			if ( engine ) {
				f = engine->openForm( md_id, 0, md_action_new, 0, 0, (aWidget*)this );
				if ( f ) {
//					connect(f, SIGNAL(selected( qulonglong )), this, SLOT(on_selected( qulonglong )));
//					f->closeAfterSelect = true;
				}
			} else printf("engine = NULL\n");

		}
	}
    return 0;
}


int
wJournal::update()
{
	if ( !dbobj ) return err_abstractobj;
	aDocument *doc = ((aDocJournal*)dbobj)->CurrentDocument();
	//doc->select(docUid);
	int res=0;
	if(doc->IsConducted())
	{
		res = QMessageBox::question(this,
						tr("Can not edit document"),
						tr("To edit document you have to unconduct it. Or you can view it. Unconduct?"),
						tr("&Unconduct"), tr("&View"),
						QString::null, 0, 1 );
	}
	else
	{
		delete doc;
		engine->openForm( docId, 0, md_action_edit, 0, docUid,  (aWidget*)this );
		return 0;
	}
	if(res==0)
	{
		doc->UnConduct();
		engine->openForm( docId, 0, md_action_edit, 0, docUid,  (aWidget*)this );
	}
	else
	{
		view();
	}
	delete doc;
	return 0;
}

int
wJournal::copy()
{
	aDocument *doc = ((aDocJournal*)dbobj)->CurrentDocument();
	if(doc)
	{
		int res = doc->Copy();
		Refresh();
		return res;
	}
	return err_copyerror;
}
int
wJournal::view()
{
//CHECK_POINT
	engine->openForm( docId, 0, md_action_view, 0, docUid,  (aWidget*)this );
    return 0;
}



int
wJournal::markDelete()
{
	if ( !dbobj ) return err_abstractobj;
	int res = QMessageBox::question(this,
					tr("Confirm"),
					tr("Do you really want to delete document?"),
					tr("&Yes"), tr("&No"),
					QString::null, 0, 1 );
	if(res) return res;
	aDocument *doc = ((aDocJournal*)dbobj)->CurrentDocument();
	if(doc->IsConducted()) doc->UnConduct();
	res = doc->Delete();
	//invalidate selection
	docId = docUid = 0;
	Refresh();
	delete doc;
	doc = 0;
	return res;
}


QString
wJournal::displayString()
{
	return "***";
}


/*!
 * Create aDocJournal database object.
 */
aObject *
wJournal::createDBObject(  aCfgItem obj, aDatabase *adb )
{
	return new aDocJournal( obj, adb );
}

void wJournal::setFilterByDate()
{
    if (date_from && date_to)
    {
        QList<wDBTable*> tables = findChildren<wDBTable*>();

        QString filter = QString("ddate>='%1T00:00:00' AND ddate<='%2T23:59:59'")
                         .arg(date_from->date().toString(Qt::ISODate))
                         .arg(date_to->date().toString(Qt::ISODate));

        for (QList<wDBTable*>::iterator it = tables.begin(); it != tables.end(); ++it)
        {
            (*it)->newFilter(filter);
        }
    }

    Refresh();
}
