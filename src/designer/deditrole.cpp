#include "deditrole.h"

#include "acfg.h"

/*
 *  Constructs a dEditRole as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditRole::dEditRole(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditRole::~dEditRole()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditRole::languageChange()
{
    retranslateUi(this);
}

extern aCfg cfg;

void dEditRole::setData(CfgForm *c, aCfgItem o)
{
    cf = c;
    obj = o;

    setWindowTitle(tr("Role:") + cf->cfg.attr(obj, mda_name));
    eName->setText(cf->cfg.attr(obj, mda_name));
    eDescription->setPlainText(cf->cfg.sText(obj, md_description));
}

void dEditRole::init()
{
    delete statusBar();
}

void dEditRole::closeEditor()
{
    updateMD();
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}

void dEditRole::updateMD()
{
    cf->cfg.setAttr(obj, mda_name, eName->text().trimmed());
    cf->cfg.setSText(obj, md_description, eDescription->toPlainText());
    cf->initRoles();
    ((MainForm*)topLevelWidget())->wl->remove(this);
}
