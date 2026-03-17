#include "deditreport.h"

#include "acfg.h"


/*
 *  Constructs a dEditReport as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditReport::dEditReport(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditReport::~dEditReport()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditReport::languageChange()
{
    retranslateUi(this);
}

void dEditReport::setData(aListViewItem *o)
{
    item = o;
    aCfg *md = o->md;
    aCfgItem obj = o->obj;

    aAliasEditor *a = new aAliasEditor(md, obj, tAliases);
    al = a;
    al->setData();

    setWindowTitle(tr("Report:") + md->attr(obj, mda_name));
    eName->setText(md->attr(obj, mda_name));
    eDescription->setPlainText(md->sText(obj, md_description));
    eModule->setPlainText(md->sText(obj, md_sourcecode));
}


void dEditReport::init()
{
    delete statusBar();
    // eModule->setInterpreter(new QSInterpreter());
}


void dEditReport::updateMD()
{
    aCfg *md = item->md;
    aCfgItem obj = item->obj;

    al->updateMD();

    item->setText(0, eName->text().trimmed());
    md->setAttr(obj, mda_name, eName->text().trimmed());
    md->setSText(obj, md_description, eDescription->toPlainText());
    md->setSText(obj, md_sourcecode, eModule->toPlainText());
}

void dEditReport::closeEditor()
{
    updateMD();
    ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}
