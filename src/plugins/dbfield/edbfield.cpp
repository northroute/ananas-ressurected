#include "edbfield.h"

#include "acfg.h"
#include "wdbfield.h"

/*
 *  Constructs a eDBField as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
eDBField::eDBField(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    if (name)
		setObjectName(name);

    if (modal)
		setModal(modal);
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
eDBField::~eDBField()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void eDBField::languageChange()
{
    retranslateUi(this);
}


void eDBField::init()
{
	eName->clear();
	onames.clear();
	oids.clear();
}

void eDBField::setData(QWidget *o, aCfg *md)
{
	if (!o)
	{
		reject();
		return;
	}

	if (o->metaObject()->className() != QString("wDBField") || !md)
	{
		reject();
		return;
	}

	int idx = 0;
	unsigned int i;
	long oid, widgetId, fieldId;
	QString pn;
	aWidget *widget = 0;
	wDBField *field = static_cast<wDBField*>(o);
	QWidget *pWidget = field->parentWidget();

	while (pWidget)
	{
		pn = pWidget->metaObject()->className();
		if (pn == QString("wCatalogue") || pn == QString("wDocument"))
			break;
		pWidget = pWidget->parentWidget();
	}

	if (pn == QString("wCatalogue"))
		widget = (wCatalogue*)pWidget;
	else if (pn == QString("wDocument"))
		widget = (wDocument*)pWidget;
	else
		return;

	widgetId = widget->getId();
	fieldId = field->getId();

	if (widgetId)
		widget->setMDObject(md->find(widgetId));

	QStringList tlist = md->types(md_field, widget->getMDObject());
	otypes.clear();
	eName->clear();
	eType->clear();

	for (QStringList::Iterator it = tlist.begin(); it != tlist.end(); ++it)
	{
		otypes.append((*it).section("\t", 0, 0));
		eName->insertItem(idx++, (*it).section("\t", 1, 1));
	}

	for (i = 0; i < (unsigned int)otypes.count(); i++)
	{
		oid = 0;
		if (!otypes[(int)i].isEmpty() && otypes[(int)i][0] == 'O')
		{
			sscanf(otypes[(int)i].toLatin1().constData(), "O %ld", &oid);
			if (oid == fieldId)
			{
				eName->setCurrentIndex((int)i);
				break;
			}
		}
	}
}

void eDBField::getData(QWidget *o)
{
	if (!o) return;
	if (o->metaObject()->className() != QString("wDBField")) return;

	wDBField *f = static_cast<wDBField*>(o);

	int idx = eName->currentIndex();
	long oid = 0;

	if (f && idx >= 0 && idx < otypes.count())
	{
		if (!otypes[idx].isEmpty() && otypes[idx][0] == 'O')
		{
			sscanf(otypes[idx].toLatin1().constData(), "O %ld", &oid);
			// f->setId(oid);
		}
	}
}

void eDBField::namechanged(const QString &s)
{
	Q_UNUSED(s)

	int idx = eName->currentIndex();
	if (idx < 0 || idx >= otypes.count() || idx >= oids.count() || idx >= onames.count())
		return;

	selotype = otypes[idx];
	seloid = oids[idx];
	eType->setText(onames[idx]);
}
