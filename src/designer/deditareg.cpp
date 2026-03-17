#include "deditareg.h"

#include "acfg.h"

/*
 *  Constructs a dEditAReg as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditAReg::dEditAReg(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditAReg::~dEditAReg()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditAReg::languageChange()
{
    retranslateUi(this);
}

void dEditAReg::init()
{
    delete statusBar();
}

void dEditAReg::closeEditor()
{
    updateMD();
    ((MainForm*)topLevelWidget())->wl->remove(this);
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}

void dEditAReg::setData(aListViewItem *o)
{
    item = o;
    aCfg *md = o->md;
    aCfgItem obj = o->obj;

    aAliasEditor *a = new aAliasEditor(md, obj, tAliases);
    al = a;
    al->setData();

    setWindowTitle(tr("Accumulation register:") + md->attr(obj, mda_name));
    eName->setText(md->attr(obj, mda_name));
    eDescription->setText(md->sText(obj, md_description));
}

void dEditAReg::updateMD()
{
    aCfg *md = item->md;
    aCfgItem obj = item->obj;

    al->updateMD();
    item->setText(0, eName->text().trimmed());
    md->setAttr(obj, mda_name, eName->text().trimmed());
    md->setSText(obj, md_description, eDescription->toPlainText());
}
