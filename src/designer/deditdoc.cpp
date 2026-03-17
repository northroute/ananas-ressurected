#include "deditdoc.h"

#include "acfg.h"

/*
 *  Constructs a dEditDoc as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditDoc::dEditDoc(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditDoc::~dEditDoc()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditDoc::languageChange()
{
    retranslateUi(this);
}

void dEditDoc::init()
{
	delete statusBar();
	//eModule->setInterpreter( new QSInterpreter() );
	//eStrViewF->setInterpreter( new QSInterpreter() );
	eStrViewF->setEnabled( FALSE );
}


void dEditDoc::closeEditor()
{
    updateMD();
    item->editor = 0;
    ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}


void dEditDoc::setData(aListViewItem *o)
{
    long int i;
    int fid, fieldid, stdf;
    item = o;
    aCfg *md = o->md;
    aCfgItem obj = o->obj, sv, field;

    aAliasEditor *a = new aAliasEditor(md, obj, tAliases);
    al = a;
    al->setData();

    aRoleEditor *r = new aRoleEditor(md, obj, tRoles, md_document);
    re = r;
    re->setData();

    setWindowTitle(tr("Document:") + md->attr(obj, mda_name));
    eName->setText(md->attr(obj, mda_name));
    eDescription->setPlainText(md->sText(obj, md_description));
    eModule->setPlainText(md->sText(obj, md_sourcecode));

    sv = md->find(obj, md_string_view);
    eStrViewF->setPlainText(md->sText(sv, md_svfunction));

    eSv->addItem("[ standart function ]");
    eSv->addItem("[ user function ]");

    fid = md->sText(sv, md_fieldid).toInt();
    stdf = md->attr(sv, mda_stdf).toInt();
    eSv->setCurrentIndex(0);

    for (i = 0; i < md->count(obj, md_field); i++) {
        field = md->find(obj, md_field, i);
        eSv->addItem(QString("%1").arg(md->attr(field, mda_name)));
        fieldid = md->id(field);
        fields.insert(i, fieldid);

        if (fid == fieldid && !stdf) {
            eSv->setCurrentIndex(i + 2);
            eStrViewF->setEnabled(false);
        }
    }

    if (!fid && !stdf) {
        eStrViewF->setEnabled(true);
        eSv->setCurrentIndex(1);
    }
}

void dEditDoc::updateMD()
{
    aCfg *md = item->md;
    aCfgItem obj = item->obj, sv;

    al->updateMD();
    re->updateMD();

    item->setText(0, eName->text().trimmed());
    md->setAttr(obj, mda_name, eName->text().trimmed());
    md->setSText(obj, md_description, eDescription->toPlainText());
    md->setSText(obj, md_sourcecode, eModule->toPlainText());

    sv = md->find(obj, md_string_view);
    if (sv.isNull()) sv = md->insert(obj, md_string_view);

    md->setSText(sv, md_svfunction, eStrViewF->toPlainText());

    if (eSv->currentIndex() == 0) {
        md->setAttr(sv, mda_stdf, "1");
    } else {
        md->setAttr(sv, mda_stdf, "0");
        if (eSv->currentIndex() == 1)
            md->setSText(sv, md_fieldid, "0");
        else
            md->setSText(sv, md_fieldid, QString("%1").arg(fields.value(eSv->currentIndex() - 2)));
    }
}



void dEditDoc::eSv_activated( int index )
{
    if ( index == 1 ) eStrViewF->setEnabled( TRUE );
    else eStrViewF->setEnabled( FALSE );
}
