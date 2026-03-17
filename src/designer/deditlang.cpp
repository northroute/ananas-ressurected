#include "deditlang.h"

#include "acfg.h"


/*
 *  Constructs a dEditLang as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditLang::dEditLang(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditLang::~dEditLang()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditLang::languageChange()
{
    retranslateUi(this);
}

extern aCfg cfg;

void dEditLang::setData(CfgForm *c, aCfgItem o)
{
    cf = c;
    obj = o;

    setWindowTitle(tr("Language:") + cf->cfg.attr(obj, mda_name));
    eTag->setText(cf->cfg.attr(obj, mda_tag));
    eName->setText(cf->cfg.attr(obj, mda_name));
    eTrFile->setText(cf->cfg.attr(obj, mda_trfile));
}

void dEditLang::init()
{
    delete statusBar();
}

void dEditLang::closeEditor()
{
    updateMD();
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}

void dEditLang::updateMD()
{
    cf->cfg.setAttr(obj, mda_tag, eTag->text());
    cf->cfg.setAttr(obj, mda_name, eName->text().trimmed());
    cf->cfg.setAttr(obj, mda_trfile, eTrFile->text());
    cf->initLang();
    ((MainForm*)topLevelWidget())->wl->remove(this);
}
