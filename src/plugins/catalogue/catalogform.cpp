#include "catalogform.h"


#include "alog.h"
// TODO Remove!!!
#include "../ananas/mainform.h"
#include "aform.h"
#include "aservice.h"


/*
 *  Constructs a CatalogForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
CatalogForm::CatalogForm(QWidget* parent, Qt::WindowFlags fl) : QMainWindow(parent, fl)
{
    setupUi(this);

    setObjectName("CatalogForm");

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
CatalogForm::~CatalogForm()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CatalogForm::languageChange()
{
    retranslateUi(this);
}

/*!
 * \return id selected element
 */
qulonglong
CatalogForm::getId()
{
    return id;
}

/*!
 * Function set initial data. Adds one element in each group.
 * \param catalog (in) - object catalog for edit or select.
 * \param mg (in) - map of catalog groups. Key - group id.
 * \param Fname (in) - list of field names of elements.
 * \param FnameGroup (in) - list of field names of groups.
 */
void CatalogForm::setData(aCatalogue* catalog,
	QMap<qulonglong, QTreeWidgetItem*> mg,
	const QStringList &Fname,
	const QStringList &FnameGroup,
	qulonglong idElForm,
	qulonglong idGrForm,
	const bool toSelect)
{
	ListView->setDestination(toSelect);
	aLog::print(aLog::Debug, tr("CatalogForm:setData begin"));

	QTreeWidgetItem *item;
	uint i;
	QPixmap pixmap(getElementPixmap());
	QPixmap pixmap_mark_deleted(getMarkDeletedPixmap());

	idElementForm = idElForm;
	idGroupForm = idGrForm;
	cat = catalog;
	fieldName = Fname[0];
	fieldList = Fname;
	fieldListGroup = FnameGroup;
	map_gr = mg;

	ListView->setAllColumnsShowFocus(true);

	QMap<qulonglong, QTreeWidgetItem*>::iterator it = map_gr.begin();

	cat->Select();

	// added one element in each group
	while (it != map_gr.end())
	{
		cat->selectByGroup(it.key());

		if (cat->First())
		{
			item = new QTreeWidgetItem(map_gr[it.key()]);
			aLog::print(aLog::Debug, tr("CatalogForm add first element in group"));

			if (cat->isElementMarkDeleted())
				item->setIcon(0, QIcon(pixmap_mark_deleted));
			else
				item->setIcon(0, QIcon(pixmap));

			for (i = 0; i < (uint)fieldList.count(); i++)
			{
				item->setText(i, cat->sysValue(fieldList[i]).toString());
			}

			map_el.insert(cat->sysValue("id").toULongLong(), item);
		}

		++it;
	}

	cat->selectByGroup(0);

	if (cat->First())
	{
		aLog::print(aLog::Debug, tr("CatalogForm add elements into root"));

		do
		{
			item = new QTreeWidgetItem(ListView);

			if (cat->isElementMarkDeleted())
				item->setIcon(0, QIcon(pixmap_mark_deleted));
			else
				item->setIcon(0, QIcon(pixmap));

			for (i = 0; i < (uint)fieldList.count(); i++)
			{
				item->setText(i, cat->sysValue(fieldList[i]).toString());
			}

			map_el.insert(cat->sysValue("id").toULongLong(), item);
		}
		while (cat->Next());
	}

	cat->Select();

	QRect r = aService::loadSizeFromConfig(
		QString("%1_embedded editor").arg(cat->md->attr(cat->obj, mda_name))
	);
	setGeometry(r);

	if (ListView->topLevelItemCount() > 0)
	{
		QTreeWidgetItem *first = ListView->topLevelItem(0);
		ListView->setCurrentItem(first);
		first->setSelected(true);
	}

	aLog::print(aLog::Debug, tr("CatalogForm init"));
}

/*!
 * Function set id, returned getId().
 * \param idx (in) - id.
 */
void
CatalogForm::setId(qulonglong idx )
{
	id = idx;
}

/*! Function search string s in table of elements and show list founding lines.
 * Searching for length s >= constant LENGTH_NO_FIND.
 * Displaing <= VISIBLE_ROWS lines.
 * \param s (in) - string for search.
 */
void CatalogForm::find(const QString& s)
{
	int count = 0;
	bool ok;

	int tmp = aService::readConfigVariable("LengthAboveFind", &ok).toInt();
	if (!ok) tmp = 3;
	const int LENGTH_NO_FIND = tmp;

	tmp = aService::readConfigVariable("VisibleRows", &ok).toInt();
	if (!ok) tmp = 50;
	const int VISIBLE_ROWS = tmp;

	setId(0);
	ListHint->clear();

	if (s.length() >= LENGTH_NO_FIND)
	{
		if (!ListHint->isVisible())
		{
			ListHint->show(LineEdit, StatusFrame);
			StatusFrame->show();
		}

		if (cat->First())
		{
			do
			{
				if (count++ < VISIBLE_ROWS)
				{
					ListHint->insertItem(
						cat->sysValue(fieldName).toString(),
						cat->sysValue("id").toInt()
					);
				}
			}
			while (cat->Next());
		}

		if (!ListHint->count())
		{
			ListHint->hide();
			StatusFrame->hide();
		}
		else
		{
			StatusFrame->update();
			StatusFrame->setToolTip(
				QString(tr("displaying %1 from %2"))
					.arg(ListHint->count())
					.arg(count)
			);
		}
	}
	else
	{
		if (ListHint->isVisible())
		{
			ListHint->hide();
			StatusFrame->hide();
		}
	}
}

/*!
 * Handler signal press arrow for line edit
 */
void CatalogForm::pressArrow()
{
	if (ListHint->isVisible())
	{
		ListHint->setFocus();

		QListWidgetItem *item = ListHint->currentItem();
		if (item)
			item->setSelected(true);
	}
	else
	{
		find(LineEdit->text());

		if (ListHint->isVisible())
		{
			ListHint->setFocus();

			QListWidgetItem *item = ListHint->currentItem();
			if (item)
				item->setSelected(true);
		}
	}
}


void
CatalogForm::isPressArrow()
{

}

/*!
 * Function create visual objects and connect slots.
 */
void CatalogForm::init()
{
	ListView = new aListView(centralWidget(), Qt::WindowFlags());
	ListView->setObjectName("listView");
	ListView->setGeometry(30, 30, 400, 300);
	ListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	LineEdit = new aLineEdit(centralWidget());
	LineEdit->setObjectName("lineEdit");

	ListHint = new aListBox(centralWidget());
	ListHint->setObjectName("listBox");

	StatusFrame = new QFrame(centralWidget());
	StatusFrame->setObjectName("statusFrame");

	ListView->setRootIsDecorated(true);

	StatusFrame->setGeometry(QRect(0, 0, 50, 5));
	StatusFrame->setFrameShape(QFrame::StyledPanel);
	StatusFrame->setFrameShadow(QFrame::Raised);
	StatusFrame->hide();

	GridLayout = new QGridLayout(centralWidget());
	GridLayout->setMargin(11);
	GridLayout->setSpacing(6);

	GridLayout->addWidget(ListView, 2, 0, 2, 1);
	GridLayout->addWidget(LineEdit, 1, 0);
	GridLayout->addWidget(bCancel, 3, 1);

	QLabel *lb = new QLabel(tr("Search"), centralWidget());
	lb->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
	GridLayout->addWidget(lb, 0, 0);

	QSpacerItem *spacer = new QSpacerItem(20, 390, QSizePolicy::Minimum, QSizePolicy::Expanding);
	GridLayout->addItem(spacer, 0, 1, 3, 1);

	QPixmap pix = rcIcon("cat.png");
	if (pix.isNull())
	{
		aLog::print(aLog::Info, tr("Catalog Form image cat.png not loaded"));
	}
	else
	{
		setWindowIcon(QIcon(pix));
	}

	connect(ListView, SIGNAL(newItemRequest(QTreeWidgetItem*)),
		this, SLOT(new_item(QTreeWidgetItem*)));

	connect(ListView, SIGNAL(newGroupRequest(QTreeWidgetItem*)),
		this, SLOT(new_group(QTreeWidgetItem*)));

	connect(ListView, SIGNAL(delItemRequest(QTreeWidgetItem*)),
		this, SLOT(del_item(QTreeWidgetItem*)));

	connect(ListView, SIGNAL(markDeletedRequest(QTreeWidgetItem*)),
		this, SLOT(mark_deleted(QTreeWidgetItem*)));

	connect(ListView, SIGNAL(undoMarkDeletedRequest(QTreeWidgetItem*)),
		this, SLOT(undo_mark_deleted(QTreeWidgetItem*)));

	connect(ListView, SIGNAL(editRequest(QTreeWidgetItem*, int)),
		this, SLOT(edit(QTreeWidgetItem*, int)));

	connect(ListView, SIGNAL(selectRequest(QTreeWidgetItem*)),
		this, SLOT(select(QTreeWidgetItem*)));

	connect(LineEdit, SIGNAL(delayTextChanged(const QString&)),
		this, SLOT(find(const QString&)));

	connect(LineEdit, SIGNAL(keyArrowPressed()),
		this, SLOT(pressArrow()));

	connect(LineEdit, SIGNAL(keyEnterPressed()),
		this, SLOT(go()));

	connect(ListView, SIGNAL(expanded(QTreeWidgetItem*)),
		this, SLOT(onLoadElements(QTreeWidgetItem*)));

	connect(ListHint, SIGNAL(lostFocus()),
		LineEdit, SLOT(setFocus()));

	connect(ListHint, SIGNAL(lostFocus()),
		StatusFrame, SLOT(hide()));

	connect(ListHint, SIGNAL(lostFocus()),
		ListHint, SLOT(hide()));

	connect(ListHint, SIGNAL(keyArrowLRPressed()),
		LineEdit, SLOT(setFocus()));

	connect(ListHint, SIGNAL(keyArrowLRPressed(const QString&)),
		this, SLOT(setText(const QString&)));

	connect(ListHint, SIGNAL(keyEnterPressed()),
		this, SLOT(go()));

	connect(ListHint, SIGNAL(sendMessage(const QString &)),
		this->statusBar(), SLOT(showMessage(const QString &)));

	connect(ListView, SIGNAL(sendMessage(const QString &)),
		this->statusBar(), SLOT(showMessage(const QString &)));

	connect(LineEdit, SIGNAL(sendMessage(const QString &)),
		this->statusBar(), SLOT(showMessage(const QString &)));

	LineEdit->setFocus();
}

/*!
 * Function delete objects, which were created in function init()
 */
void
CatalogForm::destroy()
{
	aService::saveSize2Config(this->rect(),QString("%1_embedded editor").arg(cat->md->attr(cat->obj, mda_name)));
	delete cat;
	cat = 0;
}

/*!
 * Function set text for line edit. Don't emit signals.
 */
void CatalogForm::setText(const QString &s)
{
    LineEdit->blockSignals(true);
    LineEdit->setText(s);

    int idx = ListHint->currentRow();
    if (idx >= 0)
        setId(ListHint->getId(idx));

    LineEdit->blockSignals(false);
}

/*!
 * Handler bGo::onClick() event.
 */
void
CatalogForm::go()
{
	qulonglong ide=getId(), idg;
    	if(!getId())
	{
		ide = findFirst(LineEdit->text());
		setId(ide);
	}
	idg = getIdg(ide);
	loadElements(idg);
	if(map_el.contains(getId()))
    	{
		goToItem(map_el[getId()]);
   	}

}
/*!
 * Ensures that item is visible, scrolling the list view vertically if necessary and opening (expanding) any parent items if this is required to show the item.
 */
void CatalogForm::goToItem(QTreeWidgetItem *item)
{
	if (!item) return;

	ListView->setCurrentItem(item);
	item->setSelected(true);
	ListView->scrollToItem(item);
	ListView->setFocus();
}

/*!
 * Searching string s in table of elements.
 * \param s (in) - stirng for search.
 * \return id first founding line.
 */
long
CatalogForm::findFirst( const QString &s )
{
	qulonglong res=0;
	cat->select(fieldName + " like '%"+s+"%'");// AND df<>'1'");
	//Cat->Select();
	if(cat->First())
	{
		res = cat->sysValue("id").toULongLong();

 	}
	return res;
}

/*!
 * Loads elements in group, if then don't loaded.
 * \param idGroup (in) - id group for load.
 */
void CatalogForm::loadElements(qulonglong idGroup)
{
	QTreeWidgetItem *item;
	QTreeWidgetItem *p_item;
	uint i;

	QPixmap pixmap(getElementPixmap());
	QPixmap pixmap_mark_deleted(getMarkDeletedPixmap());

	cat->selectByGroup(idGroup);

	if (cat->First())
	{
		do
		{
			if (map_gr.contains(idGroup)
				&& !map_el.contains(cat->sysValue("id").toULongLong()))
			{
				p_item = map_gr[idGroup];
				item = new QTreeWidgetItem(p_item);

				if (cat->isElementMarkDeleted())
					item->setIcon(0, QIcon(pixmap_mark_deleted));
				else
					item->setIcon(0, QIcon(pixmap));

				for (i = 0; i < (uint)fieldList.count(); i++)
				{
					item->setText(i, cat->sysValue(fieldList[i]).toString());
				}

				map_el.insert(cat->sysValue("id").toULongLong(), item);
			}
		}
		while (cat->Next());
	}
}

/*!
 * Loads elemens in group, if then don't loaded.
 * \param item (in) - group for load.
 */
void CatalogForm::onLoadElements(QTreeWidgetItem *item)
{
	QMap<qulonglong, QTreeWidgetItem*>::iterator it;

	for (it = map_gr.begin(); it != map_gr.end(); ++it)
	{
		if (it.value() == item)
		{
			loadElements(it.key());
			break;
		}
	}
}

/*!
 * Gets id parent group for element.
 * \param ide (in) - id of element.
 * \return id of group or 0 if no group.
 */
qulonglong
CatalogForm::getIdg( qulonglong ide )
{
    return cat->idGroupByElement(ide);
}


/*!
 * Adds new element.
 * \param parentItem (in) - parent group. If parent group == 0, element not added.
 */
void CatalogForm::new_item(QTreeWidgetItem *parentItem)
{
	if (!parentItem)
	{
		cat->newElement(0);

		QTreeWidgetItem *item = new QTreeWidgetItem(ListView);
		QPixmap pixmap(getElementPixmap());

		item->setIcon(0, QIcon(pixmap));
		map_el.insert(cat->sysValue("id").toULongLong(), item);

		edit(item, true);
		ListView->scrollToItem(item);
		return;
	}

	qulonglong id = getGroupId(parentItem);
	QPixmap pixmap(getElementPixmap());

	if (id)
	{
		cat->groupSelect(id);

		if (!cat->isGroupMarkDeleted())
		{
			cat->newElement(id);

			QTreeWidgetItem *item = new QTreeWidgetItem(map_gr[id]);
			item->setIcon(0, QIcon(pixmap));

			map_el.insert(cat->sysValue("id").toULongLong(), item);

			edit(item, true);
			ListView->scrollToItem(item);
		}
		else
		{
			cfg_message(0, tr("Can't added new element to mark deleted group").toUtf8().constData());
		}
	}
	else
	{
		new_item(parentItem->parent());
	}
}

/*!
 * Gets group id.
 * \param item (in) - group item.
 */
long CatalogForm::getGroupId(QTreeWidgetItem *item)
{
	QMap<qulonglong, QTreeWidgetItem*>::iterator it;

	for (it = map_gr.begin(); it != map_gr.end(); ++it)
	{
		if (it.value() == item)
			return (long)it.key();
	}

	return 0;
}

/*!
 * Adds new group.
 * \param parentItem (in) - parent group. If parent group == 0, group adds in root.
 */
void CatalogForm::new_group(QTreeWidgetItem *parentItem)
{
	qulonglong id = getGroupId(parentItem);
	QTreeWidgetItem *item = 0;
	QPixmap pixmap(getGroupPixmap());

	if (!id)
	{
		id = getElementId(parentItem);
		if (id)
		{
			cfg_message(0, tr("Can't added group to element").toUtf8().constData());
			return;
		}
		else
		{
			cat->newGroup(id);
			item = new QTreeWidgetItem(ListView);
		}
	}
	else
	{
		if (cat->isGroupMarkDeleted())
		{
			cfg_message(0, tr("Can't added group to mark deleted group").toUtf8().constData());
			return;
		}

		cat->newGroup(id);
		item = new QTreeWidgetItem(map_gr[id]);
	}

	item->setIcon(0, QIcon(pixmap));
	map_gr.insert(cat->GroupSysValue("id").toULongLong(), item);

	edit(item, true);

	ListView->scrollToItem(item);
	ListView->setCurrentItem(item);
	item->setSelected(true);
}

/*!
 * Gets pixmap for group.
 * \return pixmap.
 */
QPixmap
CatalogForm::getGroupPixmap()
{
 QPixmap  pixmap = rcIcon("group2.png");
// pixmap.detach();
	if(pixmap.isNull())
	{
		aLog::print(aLog::Info, tr("Catalog Form group pixmap not load"));
	}
return pixmap;
}


/*!
 * Gets pixmap for element.
 * \return pixmap.
 */
QPixmap
CatalogForm::getElementPixmap()
{
	QPixmap  pixmap= rcIcon("field2.png");
	if(pixmap.isNull())
	{
		aLog::print(aLog::Info, tr("Catalog Form element pixmap not load"));
	}
return pixmap;
}

/*!
 * Gets pixmap for mark deleted element or group.
 * \return pixmap.
 */
QPixmap
CatalogForm::getMarkDeletedPixmap()
{
	QPixmap  pixmap = rcIcon("editdelete2.png");
// pixmap.
	if(pixmap.isNull())
	{
		aLog::print(aLog::Info, tr("Catalog Form mark deleted group pixmap not load"));
	}
return  pixmap;
}

/*!
 * Delets current element or group (with subgroups).
 * \param item (in) - deleted element or group item.
 */
void CatalogForm::del_item(QTreeWidgetItem *item)
{
	qulonglong id = getElementId(item);

	if (id)
	{
		cat->select(id);
		if (cat->First())
		{
			cat->delElement();
			map_el.remove(id);
			delete item;
			item = 0;
		}
	}
	else
	{
		id = getGroupId(item);
		if (id)
		{
			QList<qulonglong> listDeletedId;
			cat->delGroup(id, listDeletedId);

			QList<qulonglong>::iterator it = listDeletedId.begin();
			while (it != listDeletedId.end())
			{
				if (map_el.contains(*it))
					map_el.remove(*it);
				else if (map_gr.contains(*it))
					map_gr.remove(*it);

				++it;
			}

			delete item;
			item = 0;
		}
	}
}

/*!
 * Marks deleted current element or group (with subgroups).
 * While for mark deleted items sets ahother pixmap only.
 * \param item (in) - marked deleted element or group item.
 */
void CatalogForm::mark_deleted(QTreeWidgetItem *item)
{
	qulonglong id = getElementId(item);

	if (id)
	{
		cat->select(id);
		if (cat->First())
		{
			cat->setMarkDeletedElement(id, true);
			item->setIcon(0, QIcon(getMarkDeletedPixmap()));
		}
	}
	else
	{
		id = getGroupId(item);
		if (id)
		{
			loadElements(id);

			QList<qulonglong> listDeletedId;
			cat->getMarkDeletedList(id, listDeletedId);

			QList<qulonglong>::iterator it = listDeletedId.begin();
			while (it != listDeletedId.end())
			{
				if (map_el.contains(*it))
				{
					map_el[*it]->setIcon(0, QIcon(getMarkDeletedPixmap()));
					cat->setMarkDeletedElement(*it, true);
					it = listDeletedId.erase(it);
				}
				else
				{
					++it;
				}
			}

			it = listDeletedId.begin();
			while (it != listDeletedId.end())
			{
				if (map_gr.contains(*it))
				{
					map_gr[*it]->setIcon(0, QIcon(getMarkDeletedPixmap()));
					cat->setMarkDeletedGroup(*it, true);
					it = listDeletedId.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
	}
}

/*!
 * Undo marks deleted current element or group (with subgroups).
 * \param item (in) - marked deleted element or group item.
 */
void CatalogForm::undo_mark_deleted(QTreeWidgetItem *item)
{
	qulonglong id = getElementId(item);

	if (id)
	{
		cat->select(id);
		if (cat->First())
		{
			cat->setMarkDeletedElement(id, false);
			item->setIcon(0, QIcon(getElementPixmap()));
		}
	}
	else
	{
		id = getGroupId(item);
		if (id)
		{
			QList<qulonglong> listDeletedId;
			cat->getMarkDeletedList(id, listDeletedId);

			QList<qulonglong>::iterator it = listDeletedId.begin();
			while (it != listDeletedId.end()) // first restore elements in this group
			{
				if (map_el.contains(*it))
				{
					map_el[*it]->setIcon(0, QIcon(getElementPixmap()));
					cat->setMarkDeletedElement(*it, false);
					it = listDeletedId.erase(it);
				}
				else
				{
					++it;
				}
			}

			it = listDeletedId.begin();
			while (it != listDeletedId.end()) // second restore groups
			{
				if (map_gr.contains(*it))
				{
					map_gr[*it]->setIcon(0, QIcon(getGroupPixmap()));
					cat->setMarkDeletedGroup(*it, false);
					it = listDeletedId.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
	}
}

void CatalogForm::edit( QTreeWidgetItem * item, int)
{
	edit( item, false);
}

void CatalogForm::edit(QTreeWidgetItem *item, bool afterNew)
{
	aLog::print(aLog::Debug, tr("Catalog Form edit element start"));

	MainForm *mw = (MainForm*) topLevelWidget();
	if (mw)
	{
		qulonglong id = getElementId(item);
		if (id)
		{
			aLog::print(aLog::Debug, tr("Catalog Form edit element with id=%1").arg(id));
			cat->select(id);

			if (idElementForm)
			{
				if (!cat->isElementMarkDeleted())
				{
					aForm *editForm = new aForm(mw, &mw->engine, (qulonglong) idElementForm, 0);
					if (editForm)
					{
						if (afterNew) editForm->setMode(0);
						else editForm->setMode(1);

						editForm->Select(id);
						connect(editForm, SIGNAL(closeForm(qulonglong)), this, SLOT(Refresh(qulonglong)));
						editForm->Show();
					}
					else
					{
						aLog::print(aLog::Error, tr("Catalog Form edit element form is null"));
					}
				}
				else
				{
					cfg_message(0, tr("Can't edit mark deleted element").toUtf8().constData());
				}
			}
			else
			{
				cfg_message(1, tr("Catalog haven't edit element form").toUtf8().constData());
			}
		}
		else
		{
			id = getGroupId(item);
			aLog::print(aLog::Debug, tr("Catalog Form edit group with id = %1").arg(id));

			if (id)
			{
				cat->groupSelect(id);

				if (idGroupForm)
				{
					if (!cat->isGroupMarkDeleted())
					{
						aForm *editForm = new aForm(mw, &mw->engine, (qulonglong) idGroupForm, 0);
						if (editForm)
						{
							if (afterNew) editForm->setMode(0);
							else editForm->setMode(1);

							editForm->SelectGroup(id);
							connect(editForm, SIGNAL(closeForm(qulonglong)), this, SLOT(Refresh(qulonglong)));
							editForm->Show();
						}
						else
						{
							aLog::print(aLog::Error, tr("Catalog Form edit group form is null"));
						}
					}
					else
					{
						cfg_message(0, tr("Can't edit mark deleted group").toUtf8().constData());
					}
				}
				else
				{
					cfg_message(1, tr("Catalog haven't edit group form").toUtf8().constData());
				}
			}
		}
	}
	else
	{
		aLog::print(aLog::Error, tr("Catalog Form main widget is not 'MainForm'"));
	}
}

/*!
 * Selects current group or element.
 * \param item (in) - selected element or group item
 */
void CatalogForm::select( QTreeWidgetItem * item )
{
	qulonglong res = getGroupId(item);
	if(!res)
	{
		res = getElementId(item);
		cat->select(res);
		if(!cat->isElementMarkDeleted())
		{
			setId(res);
			doOk();
		}
	}
	else
	{
		cat->groupSelect(res);
		if(!cat->isGroupMarkDeleted())
		{
			setId(res);
			doOk();
		}
	}
	emit(selected(res));
//    return res;
}

/*!
 * Gets element id.
 * \param item (in) - element item.
 */
qulonglong CatalogForm::getElementId(QTreeWidgetItem *item)
{
    QMap<qulonglong, QTreeWidgetItem*>::iterator it;

    for (it = map_el.begin(); it != map_el.end(); ++it)
    {
        if (it.value() == item)
            return it.key();
    }

    return 0;
}


void CatalogForm::doOk()
{
	close();
}



void CatalogForm::keyPressEvent( QKeyEvent * e )
{
	if ( e->key()==Qt::Key_Return)
	{
	}
	if ( e->key()==Qt::Key_Escape)
	{
		close();
	}
	e->ignore();
}


void CatalogForm::Refresh( qulonglong id )
{
	QTreeWidgetItem * item;
	if( map_el.contains( id ))
	{
		item = map_el[id];
		cat->select( id );
		for(uint i=0; i<fieldList.count(); i++)
		{

			item->setText(i,cat->sysValue(fieldList[i]).toString());
			ListView->setFocus();
		}
	}
	else
	{
		if( map_gr.contains( id ) )
		{
			item = map_gr[id];
			cat->groupSelect( id );
			item->setText(0,cat->GroupSysValue(fieldListGroup[0]).toString());
			ListView->setFocus();
		}
	}
}

//#include "catalogform.ui.h"
