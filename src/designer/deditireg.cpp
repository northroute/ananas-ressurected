#include "deditireg.h"

#include "acfg.h"

/*
 *  Constructs a dEditIReg as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditIReg::dEditIReg(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditIReg::~dEditIReg()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditIReg::languageChange()
{
    retranslateUi(this);
}

void dEditIReg::init()
{
    delete statusBar();
}

void dEditIReg::closeEditor()
{
    updateMD();
    ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}

void dEditIReg::setData(aListViewItem *o)
{
    item = o;
    aCfg *md = o->md;
    aCfgItem obj = o->obj;

    aAliasEditor *a = new aAliasEditor(md, obj, tAliases);
    al = a;
    al->setData();

    setWindowTitle(tr("Information register:") + md->attr(obj, mda_name));
    eName->setText(md->attr(obj, mda_name));

    if (md->attr(obj, mda_no_unconduct) == "1")
        checkBox1->setChecked(true);
    else
        checkBox1->setChecked(false);

    eDescription->setPlainText(md->sText(obj, md_description));
}

void dEditIReg::updateMD()
{
    aCfg *md = item->md;
    aCfgItem obj = item->obj;

    al->updateMD();

    item->setText(0, eName->text().trimmed());
    md->setAttr(obj, mda_name, eName->text().trimmed());
    md->setAttr(obj, mda_no_unconduct, checkBox1->isChecked() ? "1" : "0");
    md->setSText(obj, md_description, eDescription->toPlainText());
}
