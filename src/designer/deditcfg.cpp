#include "deditcfg.h"

#include "acfg.h"

/*
 *  Constructs a dEditCfg as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditCfg::dEditCfg(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditCfg::~dEditCfg()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditCfg::languageChange()
{
    retranslateUi(this);
}

void dEditCfg::init()
{
    delete statusBar();

    eDBType->clear();
    eDBType->addItem(tr("Unknown"));
    eDBType->addItem(tr("MySQL"));
    eDBType->addItem(tr("Postgres"));

    // QGridLayout *l = new QGridLayout(tabWidget->widget(2));
    // eModule = new QSEditor(tabWidget->widget(2), "global module");
    // l->addWidget(eModule, 5, 5, 0);
    // eModule->setInterpreter(new QSInterpreter());
}

void dEditCfg::closeEditor()
{
    updateMD();
    ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
    //	item->editor = 0;
}

void dEditCfg::setData(aListViewItem *o)
{
    item = o;
    aCfg *md = item->md;
    aCfgItem obj = item->obj;

    QString v;

    setWindowTitle(tr("Business scheme parameters: ") + md->info(md_info_name));

    v = md->rc.value("dbtype");
    eDBType->setCurrentIndex(0);
    if (v == "postgres") eDBType->setCurrentIndex(2);
    if (v == "mysql") eDBType->setCurrentIndex(1);

    eCfgName->setText(md->rc.value("configfile"));
    eWorkDir->setText(md->rc.value("workdir"));
    eDBHost->setText(md->rc.value("dbhost"));
    eDBName->setText(md->rc.value("dbname"));
    eDBHeader->setText(md->rc.value("dbtitle"));
    eDBPass->setText(md->rc.value("dbpass"));
    eDBUserName->setText(md->rc.value("dbuser"));

    eName->setText(md->info(md_info_name));
    eAuthor->setText(md->info(md_info_author));
    eDate->setText(md->info(md_info_date));
    eDescription->setPlainText(md->info(md_info_remark));

    globals = md->find(obj, md_globals, 0);
    eModule->setPlainText(md->sText(globals, md_sourcecode));
}

void dEditCfg::updateMD()
{
    aCfg *md = item->md;
    aCfgItem obj = item->obj;

    md->setInfo(md_info_name, eName->text());
    md->setInfo(md_info_remark, eDescription->toPlainText());
    md->setInfo(md_info_author, eAuthor->text());
    md->setInfo(md_info_date, eDate->text());

    md->rc.setValue("configfile", eCfgName->text().trimmed());
    md->rc.setValue("workdir", eWorkDir->text().trimmed());
    md->rc.setValue("dbhost", eDBHost->text().trimmed());
    md->rc.setValue("dbtitle", eDBHeader->text().trimmed());
    md->rc.setValue("dbname", eDBName->text().trimmed());
    md->rc.setValue("dbpass", eDBPass->text().trimmed());
    md->rc.setValue("dbuser", eDBUserName->text().trimmed());

    if (eDBType->currentIndex() == 1) md->rc.setValue("dbtype", "mysql");
    if (eDBType->currentIndex() == 2) md->rc.setValue("dbtype", "postgres");

    globals = md->find(obj, md_globals, 0);
    md->setSText(globals, md_sourcecode, eModule->toPlainText());
}
