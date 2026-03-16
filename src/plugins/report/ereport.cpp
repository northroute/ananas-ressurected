#include "ereport.h"

#include "acfg.h"

/*
 *  Constructs a eReport as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
eReport::eReport(QWidget *parent, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
eReport::~eReport()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void eReport::languageChange()
{
    retranslateUi(this);
}

void eReport::init()
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



/*void eReport::setData( aCatalogue *f, QWidget *mw )
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

void eReport::setData(QWidget *o, aCfg *md)
{
    if (!o || !md) {
        reject();
        return;
    }

    wReport *f = qobject_cast<wReport *>(o);
    if (!f) {
        reject();
        return;
    }

    int idx = 0;
    long oid = 0;
    long id = f->getId();

    QStringList tlist = md->types(md_journal);

    otypes.clear();
    eType->clear();

    for (QStringList::Iterator it = tlist.begin(); it != tlist.end(); ++it) {
        QString typeId = (*it).section("\t", 0, 0);
        QString typeName = (*it).section("\t", 1, 1);

        otypes.append(typeId);
        eType->addItem(typeName);

        idx++;
    }

    for (int i = 0; i < otypes.count(); ++i) {
        if (otypes[i].startsWith("O")) {
            sscanf(otypes[i].toStdString().c_str(), "O %ld", &oid);

            if (oid == id) {
                eType->setCurrentIndex(i);
                break;
            }
        }
    }
}


void eReport::getData(QWidget *o)
{
    if (!o) return;

    wReport *f = qobject_cast<wReport *>(o);
    if (!f) return;

    int idx = eType->currentIndex();
    long oid = 0;

    if (idx < 0) return;

    if (otypes[idx].startsWith("O")) {
        sscanf(otypes[idx].toStdString().c_str(), "O %ld", &oid);
        f->setId(oid);
    }
}
