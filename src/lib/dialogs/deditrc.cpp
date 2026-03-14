#include "deditrc.h"

#include "atests.h"

/*
 *  Constructs a dEditRC as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
dEditRC::dEditRC(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl) : QDialog(parent, fl)
{
    setObjectName(name);
    setModal(modal);

    setupUi(this);
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditRC::~dEditRC()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditRC::languageChange()
{
    retranslateUi(this);
}



void dEditRC::setdata(QString nameRC, rcListViewItem *item)
{
    it = item;
    QMap<QString,QString> cfg;

    cfg = aTests::readConfig(QDir::toNativeSeparators(nameRC));

    eRCFile->setText(QDir::toNativeSeparators(nameRC));
    eDBType->setCurrentIndex(0);

    eDBTitle->setText(cfg["dbtitle"]);
    eDBName->setText(cfg["dbname"]);
    eDBUserName->setText(cfg["dbuser"]);
    ePass->setText(cfg["dbpass"]);
    eDBHost->setText(cfg["dbhost"]);
    eDBPort->setText(cfg["dbport"]);
    eCfgName->setText(QDir::toNativeSeparators(cfg["configfile"]));

    if(cfg["dbtype"]=="postgres") eDBType->setCurrentIndex(3);
    if(cfg["dbtype"]=="mysql") eDBType->setCurrentIndex(2);
    if(cfg["dbtype"]=="internal") eDBType->setCurrentIndex(1);

    eWorkDir->setText(QDir::toNativeSeparators(cfg["workdir"]));
}


void dEditRC::updatecfg()
{
    QMap<QString,QString> cfg;

    cfg["dbtitle"] = eDBTitle->text();
    cfg["dbname"]  = eDBName->text();

    if(eDBType->currentIndex()==1) cfg["dbtype"] ="internal";
    if(eDBType->currentIndex()==2) cfg["dbtype"] ="mysql";
    if(eDBType->currentIndex()==3) cfg["dbtype"]= "postgres";

    cfg["dbuser"] = eDBUserName->text();
    cfg["dbpass"] = ePass->text();
    cfg["dbhost"] = eDBHost->text();
    cfg["dbport"] = eDBPort->text();

    cfg["workdir"] = QDir::toNativeSeparators(eWorkDir->text());
    cfg["configfile"] = QDir::toNativeSeparators(eCfgName->text());

    aTests::writeConfig(QDir::toNativeSeparators(eRCFile->text()), cfg);
}



void dEditRC::onOK()
{
	updatecfg();
	it->rcfile = QDir::convertSeparators(eRCFile->text());
	it->setText(0,eDBTitle->text());
	accept();
}


void dEditRC::onRCFile()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("ananas config resource (*.rc)"),
        QDir::toNativeSeparators(eRCFile->text()),
        tr("ananas config resource (*.rc)")
    );

    if (fileName.isEmpty())
        return;

    eRCFile->setText(QDir::toNativeSeparators(fileName));
    setdata(eRCFile->text(), it);
}

void dEditRC::onCFGFile()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("ananas config file (*.cfg)"),
        QDir::toNativeSeparators(eCfgName->text()),
        tr("ananas config file (*.cfg)")
    );

    if (fileName.isEmpty())
        return;

    eCfgName->setText(QDir::toNativeSeparators(fileName));
}
