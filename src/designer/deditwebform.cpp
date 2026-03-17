#include "deditwebform.h"

#include "acfg.h"


/*
 *  Constructs a dEditWebForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditWebForm::dEditWebForm(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditWebForm::~dEditWebForm()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditWebForm::languageChange()
{
    retranslateUi(this);
}

void dEditWebForm::init()
{
	delete statusBar();
	//eServerModule->setInterpreter(new QSInterpreter());
	//eClientModule->setInterpreter(new QSInterpreter());
}

void dEditWebForm::closeEditor()
{
    updateMD();
    ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
    item->editor = 0;
}

void dEditWebForm::setData(aListViewItem *o)
{
    item = o;
    aCfg *md = o->md;
    aCfgItem obj = o->obj;

    aAliasEditor *a = new aAliasEditor(md, obj, tAliases);
    al = a;
    al->setData();

    setWindowTitle(tr("Web form:") + md->attr(obj, mda_name));
    eName->setText(md->attr(obj, mda_name));
    eDescription->setPlainText(md->sText(obj, md_description));
    eServerModule->setPlainText(md->sText(obj, md_servermodule));
    eClientModule->setPlainText(md->sText(obj, md_clientmodule));
    eFormSource->setPlainText(md->sText(obj, md_formsource));
}

void dEditWebForm::updateMD()
{
    aCfg *md = item->md;
    aCfgItem obj = item->obj;

    al->updateMD();
    item->setText(0, eName->text().trimmed());
    md->setAttr(obj, mda_name, eName->text().trimmed());
    md->setSText(obj, md_description, eDescription->toPlainText());
    md->setSText(obj, md_servermodule, eServerModule->toPlainText());
    md->setSText(obj, md_clientmodule, eClientModule->toPlainText());
    md->setSText(obj, md_formsource, eFormSource->toPlainText());
}
