#include "ejournal.h"

#include "acfg.h"

/*
 *  Constructs a eJournal as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
eJournal::eJournal(QWidget *parent, Qt::WindowFlags fl) : QDialog(parent, fl)
{
    setObjectName("eJournal");
    setModal(true);

    setupUi(this);
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
eJournal::~eJournal()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void eJournal::languageChange()
{
    retranslateUi(this);
}

void eJournal::init()
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



/*void eJournal::setData( aCatalogue *f, QWidget *mw )
{
	aCfg *md = 0;
	if ( !f ) return;
//	printf("name = '%s'\n", (const char *) mw->name() );
	if (mw->name() == QString("ananas-designer_mainwindow") ) {
//		printf("running under ananas designer\n");
		CfgForm *cf = ( ( MainForm *) mw )->cfgForm();
		if ( cf ) {
			md = &cf->cfg;
//			QString ts = f->getFieldType();
//			char t=' ';
			int w=0, d=0, idx=0;
			unsigned int i;
			long oid , id;

			id = f->getId();

			otypes.clear();
			eType->clear();

			QStringList tlist = md->types( md_catalogue );
			otypes.clear();
			eType->clear();
			for ( QStringList::Iterator it = tlist.begin(); it != tlist.end(); ++it ) {
				otypes.append( (*it).section( "\t", 0, 0 ) );
				eType->insertItem( (*it).section("\t", 1, 1 ), idx++ );
			}
			for ( i = 0 ; i < otypes.count(); i++ ) {
				oid = 0;
				if( otypes[i][0] == 'O' ) {
					sscanf( (const char *)otypes[ i ], "O %d", &oid );
					if ( oid == id ) {
						eType->setCurrentItem( i );
						break;
					}
				}
			}
		} else reject();
	}

}
*/

void eJournal::setData(QWidget *o, aCfg *md)
{
    if (!o || !md)
    {
        reject();
        return;
    }

    wJournal *f = qobject_cast<wJournal *>(o);
    if (!f)
    {
        reject();
        return;
    }

    int idx = 0;
    long oid = 0;
    long id = f->getId();

    QStringList tlist = md->types(md_journal);
    otypes.clear();
    eType->clear();

    for (QStringList::Iterator it = tlist.begin(); it != tlist.end(); ++it)
    {
        otypes.append((*it).section("\t", 0, 0));
        eType->insertItem(idx++, (*it).section("\t", 1, 1));
    }

    for (int i = 0; i < otypes.count(); ++i)
    {
        if (!otypes[i].isEmpty() && otypes[i].at(0) == 'O')
        {
            oid = otypes[i].section(' ', 1, 1).toLong();
            if (oid == id)
            {
                eType->setCurrentIndex(i);
                break;
            }
        }
    }
}

void eJournal::getData(QWidget *o)
{
    if (!o)
        return;

    wJournal *f = qobject_cast<wJournal *>(o);
    if (!f)
        return;

    int idx = eType->currentIndex();
    long oid = 0;

    if (!otypes[idx].isEmpty() && otypes[idx].at(0) == 'O')
    {
        oid = otypes[idx].section(' ', 1, 1).toLong();
        f->setId(oid);
    }
}
