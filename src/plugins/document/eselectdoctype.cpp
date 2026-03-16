#include "eselectdoctype.h"

/*
 *  Constructs a eSelectDocType as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
eSelectDocType::eSelectDocType(QWidget *parent, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setObjectName("eSelectDocType");
    setModal(true);

    setupUi(this);
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
eSelectDocType::~eSelectDocType()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void eSelectDocType::languageChange()
{
    retranslateUi(this);
}

void eSelectDocType::init()
{
    listDocs->header()->hide();
    listDocs->setColumnWidth( 0, 1000 );
    listDocs->setFocus();
}


void eSelectDocType::setJournal(aCfg *md, int id)
{
    aCfgItem obj, dobj;
    int i, did;
    QString name;

    listDocs->clear();
    dlist.clear();

    obj = md->find(id);
    if (!obj.isNull())
    {
        dlist = md->getJournalDocuments(obj);

        for (i = 0; i < dlist.count(); ++i)
        {
            did = dlist[i].toInt();
            dobj = md->find(did);

            if (!dobj.isNull())
            {
                name = md->attr(dobj, mda_name);

                QTreeWidgetItem *item = new QTreeWidgetItem(listDocs);
                item->setText(0, name);
                item->setText(1, dlist[i]);
            }
        }

        QTreeWidgetItem *first = listDocs->topLevelItem(0);
        if (first)
            listDocs->setCurrentItem(first);
    }
}


void eSelectDocType::accepted()
{
    QTreeWidgetItem *item = listDocs->currentItem();
    if (!item)
        return;

    docId = item->text(1).toInt();

    accept();
}
