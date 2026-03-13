#include "dselectdb.h"

#include "atests.h"
#include "dhelpcfgform.h"

/*
 *  Constructs a dSelectDB as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
dSelectDB::dSelectDB(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
    , settings(QSettings::IniFormat, QSettings::UserScope, "Ananas company Ltd", "Ananas4")
{
    if (name)
        setObjectName(name);

    setModal(modal);

    setupUi(this);
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dSelectDB::~dSelectDB()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dSelectDB::languageChange()
{
    retranslateUi(this);
}


#include "deditrc.h"
#include "dimportdb.h"
#include "acfgrc.h"
#include "atests.h"
#include "adataexchange.h"
#include "alog.h"


void dSelectDB::init()
{
    aLog::init("ananas.log", aLog::Debug);
    createMenu();
    progressBar1->hide();

    QString fname;
    withgroups = 1;

    settings.beginGroup("groups");
    local = settings.childGroups().count();
    settings.endGroup();

    changes = false;
	/*--if(!local)
	{

		aLog::print(aLog::Debug, tr("dSelectDB local settings not found"));
		settings.insertSearchPath( QSettings::Windows, "/ananasgroup/ananas/globalsettings");
#ifdef Q_OS_WIN32
		QStringList subkeys = settings.subkeyList("/ananasgroup/ananas/globalsettings");
		settings.insertSearchPath(QSettings::Windows,"/ananasgroup/ananas/globalsettings");
		QStringList entryGroup;
		for(uint i=0; i<subkeys.count();i++)
		{
			QString str =subkeys[i];
			//str = str.left(str.length()-2); // length("rc")=2
			entryGroup =settings.entryList("/"+str);
			settings.beginGroup("/"+str);
			readSettings(entryGroup);
			settings.endGroup();
		}

#else
		QString suff = "*_grouprc";
		QDir dir("/etc/ananas",suff,QDir::Name | QDir::IgnoreCase, QDir::Files | QDir::Readable);
		QStringList subkeys = dir.entryList(suff);
		settings.insertSearchPath(QSettings::Unix,"/etc/ananas/");
		QStringList entryGroup;
		for(uint i=0; i<subkeys.count();i++)
		{
			QString str =subkeys[i];
			str = str.left(str.length()-2); // length("rc")=2
			entryGroup =settings.entryList("/"+str);
			settings.beginGroup("/"+str);
			readSettings(entryGroup);
			settings.endGroup();
		}
#endif
	}
	else*/
	{
    QStringList lst = settings.childGroups();
    settings.beginGroup("groups");
    readSettings(lst);
    settings.endGroup();
	}

	setWindowIcon(rcIcon("a-system.png"));
	listDBRC->hideColumn(1);
	listDBRC->setSortingEnabled(false);
	listDBRC->header()->hide();
	listDBRC->setRootIsDecorated(true);
	buttonOk->setEnabled(false);
	return;
}

void dSelectDB::readSettings(QStringList entryGroup)
{
    rcfile = "";
    ro_mode = 0;

    QFile f, fc;
    QString s, sp, sn, rc;
    rcListViewItem *item, *gitem = NULL;

    for (int j = 0; j < entryGroup.count(); j++)
    {
        QString groupKey = entryGroup[j];

        settings.beginGroup(groupKey);
        QStringList eitems = settings.childKeys();
        settings.endGroup();

        QString groupName = settings.value(groupKey, "unknown group").toString();

        aLog::print(aLog::Debug,
                    tr("dSelectDB read settings for group with name %1").arg(groupName));

        gitem = new rcListViewItem(listDBRC, groupName, "", true);
        gitem->setExpanded(true);

        QMap<QString,QString> cfg;
        for (int k = eitems.count() - 1; k >= 0; k--)
        {
            rc = settings.value(groupKey + "/" + eitems[k]).toString();
            cfg = aTests::readConfig(QDir::toNativeSeparators(rc));
            sn = cfg["dbtitle"];

            if (gitem)
                item = new rcListViewItem(gitem, sn, rc);
            else
                item = new rcListViewItem(listDBRC, sn, rc);
        }
    }
}


void dSelectDB::itemSelect(QTreeWidgetItem *item, int col)
{
    if (!item) return;

    rcListViewItem *i = static_cast<rcListViewItem*>(item);
    buttonOk->setEnabled(!i->group);
}


void dSelectDB::newGroup()
{
    QTreeWidgetItem *gitem;

    gitem = new rcListViewItem(listDBRC, tr("New group"), "", true);

    listDBRC->setRootIsDecorated(true);
    listDBRC->setCurrentItem(gitem);

    editItem();
    withgroups = 1;
    changes = true;
}


void dSelectDB::newItem()
{
    rcListViewItem *item, *gitem;
    QString rc;

    item = static_cast<rcListViewItem *>(listDBRC->currentItem());
    if (!item) return;

    if (withgroups)
    {
        if (item->parent())
            gitem = static_cast<rcListViewItem *>(item->parent());
        else
            gitem = item;

        item = new rcListViewItem(gitem, tr("New shema"), "myrc");
        gitem->setExpanded(true);
    }
    else
    {
        item = new rcListViewItem(listDBRC, tr("New shema"), "myrc");
    }

    listDBRC->setCurrentItem(item);
    editItem();
    changes = true;
}

void dSelectDB::editItem()
{
    rcListViewItem *item;
    dEditRC *d = new dEditRC(this);

    item = static_cast<rcListViewItem *>(listDBRC->currentItem());
    if (!item) return;

    if (item->group)
    {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        listDBRC->editItem(item, 0);
        changes = true;
    }
    else
    {
        d->setdata(QDir::toNativeSeparators(item->rcfile), item);
        if (d->exec() == QDialog::Accepted)
        {
            changes = true;
        }
    }
}



void dSelectDB::deleteItem()
{
    QTreeWidgetItem *item = listDBRC->currentItem();

    if (item)
    {
        QString msg = tr("Delete item?");
        if (QMessageBox::question(this, tr("confirm delete"), msg,
                                  QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok)
        {
            return;
        }

        delete item;

        if (listDBRC->topLevelItemCount() == 0)
            withgroups = 0;

        changes = true;
    }
}


void dSelectDB::saveRC()
{
    rcListViewItem *item, *gitem;
    QFile f(rcfile);

    aLog::print(aLog::Info, tr("dSelectDB save settings"));

    if (!local)
    {
        if (!changes) return;

        QString home = QDir::homePath();
#ifndef Q_OS_WIN32
        QString msg = QString("Все внесенные изменения будут сохранены локально в вашем домашнем каталоге.\nПри очередном запуске программы будут использованы локальные настройки из каталога\n%1/.ananas.\nЕсли Вы хотите использовать глобальные настройки, вам следует удалить каталог\n%2/.ananas").arg(home).arg(home);
#else
        QString msg = tr("save settings local?");
#endif
        if (QMessageBox::question(this, tr("save"), QString::fromUtf8(msg.toUtf8()), QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok)
        {
            return;
        }
    }

    uint gcount = 0;
    clearSettings();

    settings.beginGroup("groups");

    for (int gi = 0; gi < listDBRC->topLevelItemCount(); ++gi)
    {
        gitem = static_cast<rcListViewItem *>(listDBRC->topLevelItem(gi));

        if (withgroups)
        {
            settings.setValue(QString::number(gcount), gitem->text(0));

            for (int ei = 0; ei < gitem->childCount(); ++ei)
            {
                item = static_cast<rcListViewItem *>(gitem->child(ei));
                settings.setValue(QString::number(gcount) + "/" + QString::number(ei), item->rcfile);
            }
        }

        ++gcount;
    }

    settings.endGroup();
}


void dSelectDB::onOK()
{
    rcListViewItem *item;

    saveRC();
    item = static_cast<rcListViewItem*>(listDBRC->currentItem());

    if (item)
    {
        if (!item->rcfile.isEmpty())
        {
            rcfile = item->rcfile;
            accept();
        }
    }
}


void dSelectDB::onCancel()
{
	//saveRC();
	reject();
}


void dSelectDB::ItemRenamed(QTreeWidgetItem *item, int col)
{
    if (!item) return;
    if (col > 1) return;
}


void dSelectDB::clearSettings()
{
    settings.beginGroup("groups");

    QStringList entryGroup = settings.childGroups();
    for (int j = 0; j < entryGroup.count(); j++)
    {
        settings.remove(entryGroup[j]);
    }

    settings.endGroup();
}


void dSelectDB::onHelp()
{
	HelpCfgForm f;
	f.exec();
}

void dSelectDB::createMenu()
{
    menuBar = new QMenuBar(this);

    QMenu *menuFile = new QMenu(this);
    menuFile->setTitle(tr("Program"));
    menuFile->addAction(tr("E&xit"), this, SLOT(onCancel()));

    QMenu *menuEdit = new QMenu(this);
    menuEdit->setTitle(tr("Actions"));
    menuEdit->addAction(tr("New &group"), this, SLOT(newGroup()));
    menuEdit->addAction(tr("&New shema"), this, SLOT(newItem()));
    menuEdit->addSeparator();
    menuEdit->addAction(tr("Edi&t"), this, SLOT(editItem()));
    menuEdit->addSeparator();
    menuEdit->addAction(tr("&Delete"), this, SLOT(deleteItem()));

    QMenu *menuAction = new QMenu(this);
    menuAction->setTitle(tr("Service"));
    menuAction->addAction(tr("&Import"), this, SLOT(importItem()));
    menuAction->addAction(tr("&Export"), this, SLOT(exportItem()));

    menuBar->addMenu(menuFile);
    menuBar->addMenu(menuEdit);
    menuBar->addMenu(menuAction);

    layout()->setMenuBar(menuBar);
    menuBar->show();
}


void dSelectDB::importItem()
{
    rcListViewItem *item, *gitem;
    dImportDB *d = new dImportDB(this);

    item = static_cast<rcListViewItem *>(listDBRC->currentItem());
    if (!item) return;

    if (withgroups)
    {
        if (item->parent())
            gitem = static_cast<rcListViewItem *>(item->parent());
        else
            gitem = item;

        item = new rcListViewItem(gitem, tr("New shema"), "myrc");
        gitem->setExpanded(true);
    }
    else
    {
        item = new rcListViewItem(listDBRC, tr("New shema"), "myrc");
    }

    listDBRC->setCurrentItem(item);
    d->setdata("", item);

    if (d->exec() == QDialog::Accepted)
    {
        QString rcfile = item->rcfile;
        QString filename = d->eCfgName->text();

        progressBar1->show();
        progressBar1->reset();

        aBackup newBase;
        // connect(&newBase, SIGNAL(progress(int,int)), progressBar1, SLOT(setProgress(int,int)));

        if (newBase.importData(rcfile, filename, true) == false)
        {
            QMessageBox::information(this, tr("Backup"), newBase.lastError(), QMessageBox::Ok);
            progressBar1->hide();
            aLog::print(aLog::Info, tr("dSelectDB business schema import"));
        }
        else
        {
            aLog::print(aLog::Error, tr("dSelectDB restore: %1").arg(newBase.lastError()));
            QMessageBox::critical(this, tr("Backup"), newBase.lastError(), QMessageBox::Ok);
            progressBar1->hide();
            delete item;
            item = 0;
            return;
        }

        changes = true;
    }
    else
    {
        delete item;
        item = 0;
        return;
    }
}


void dSelectDB::exportItem()
{
    QString dir, rcfile, filename;
#ifndef Q_OS_WIN32
    dir = "/home";
#endif

    rcListViewItem *item = static_cast<rcListViewItem *>(listDBRC->currentItem());
    if (item)
    {
        if (!item->rcfile.isEmpty() && !item->group)
        {
            rcfile = item->rcfile;
        }
        else
        {
            QMessageBox::information(this, tr("Select item"),
                                     tr("Please, select item for export"),
                                     QMessageBox::Ok);
            return;
        }

        filename = QFileDialog::getSaveFileName(
            this,
            tr("Export"),
            dir,
            tr("Any files (*)")
        );

        if (filename.isEmpty())
            return;

        aBackup backupBase;
        progressBar1->show();
        progressBar1->reset();

        // Старый connect почти наверняка надо переписывать отдельно:
        // connect(&backupBase, SIGNAL(progress(int,int)), progressBar1, SLOT(setProgress(int,int)));

        if (backupBase.exportData(rcfile, filename, true) == 0)
        {
            aLog::print(aLog::Info, tr("dSelectDB backup ok"));
            QMessageBox::information(this, tr("Backup"), backupBase.lastError(), QMessageBox::Ok);
            progressBar1->hide();
        }
        else
        {
            aLog::print(aLog::Debug, tr("dSelectDB backup: %1").arg(backupBase.lastError()));
            QMessageBox::critical(this, tr("Backup"), backupBase.lastError(), QMessageBox::Ok);
            progressBar1->hide();
            return;
        }
    }
    else
    {
        QMessageBox::information(this, tr("Select item"),
                                 tr("Please, select item for export"),
                                 QMessageBox::Ok);
        return;
    }
}


void dSelectDB::onDblClick(QTreeWidgetItem *item, int col)
{
    if (!static_cast<rcListViewItem*>(item)->group)
    {
        onOK();
    }
}
