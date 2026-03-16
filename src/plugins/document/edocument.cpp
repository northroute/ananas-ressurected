#include "edocument.h"


#include "acfg.h"

/*
 *  Constructs a eDocument as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
eDocument::eDocument(QWidget *parent, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setObjectName("eDocument");
    setModal(true);

    setupUi(this);
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
eDocument::~eDocument()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void eDocument::languageChange()
{
    retranslateUi(this);
}

void eDocument::init()
{
/*
	int oc, i, f, idx=0;
	char *id, *name, *ot;
	cfg_objptr o;
	char otype[100];
	QString named;

	otypes.clear();
	eType->clear();
	otypes.append("D");
	eType->insertItem(trUtf8("Дата"), idx++);
	otypes.append("T");
	eType->insertItem(trUtf8("Время"), idx++);
	otypes.append("N %d %d");
	eType->insertItem(trUtf8("Число"), idx++);
	otypes.append("C %d");
	eType->insertItem(trUtf8("Строка"), idx++);

	oc=cfgobj_count(NULL, NULL);
	for (i=1;i<=oc;i++) {
		id=cfgobj_idn(NULL, NULL, i, &o);
		ot=(char *)o->name;
		name=cfgobj_attr(o, "name");
		named="";
		f=0;
		if (strcmp((char *)ot, aot_doc)==0) {
			named=trUtf8("Документ.");
			f=1;
		}
		if (strcmp((char *)ot, aot_cat)==0) {
			named=trUtf8("Справочник.");
			f=1;
		}
		if (f) {
			named=named+trUtf8(name);
			sprintf(otype, "O %s", id);
			otypes.append(otype);
			eType->insertItem(named, idx++);
		}
	}
*/
}



void eDocument::setData(QWidget *o, aCfg *md)
{
    if (!o || !md)
    {
        reject();
        return;
    }

    wDocument *f = qobject_cast<wDocument *>(o);
    if (!f)
    {
        reject();
        return;
    }

    int idx = 0;
    int oid = 0;
    qulonglong id = f->getId();

    otypes.clear();
    eType->clear();

    QStringList tlist = md->types(md_document);

    for (QStringList::Iterator it = tlist.begin(); it != tlist.end(); ++it)
    {
        otypes.append((*it).section("\t", 0, 0));
        eType->insertItem(idx++, (*it).section("\t", 1, 1));
    }

    for (int i = 0; i < otypes.count(); ++i)
    {
        if (!otypes[i].isEmpty() && otypes[i].at(0) == 'O')
        {
            oid = otypes[i].section(' ', 1, 1).toInt();
            if ((qulonglong)oid == id)
            {
                eType->setCurrentIndex(i);
                break;
            }
        }
    }
}


void eDocument::getData(QWidget *o)
{
    QVariant v;

    if (!o)
        return;

    wDocument *f = qobject_cast<wDocument *>(o);
    if (!f)
        return;

    int idx = eType->currentIndex();
    int oid = 0;

    if (otypes[idx].at(0) == 'O')
    {
        oid = otypes[idx].section(' ', 1, 1).toInt();
        f->setProperty("Id", QVariant(oid));
    }
}
