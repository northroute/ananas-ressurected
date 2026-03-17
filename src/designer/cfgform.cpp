#include "cfgform.h"

//#include <iostream.h>

#include "ananas.h"
#include "mainform.h"
#include "deditfield.h"
#include "deditcfg.h"
#include "deditdoc.h"
#include "deditrole.h"
#include "deditreport.h"
#include "deditdialog.h"
#include "deditcat.h"
#include "deditlang.h"
#include "mdtree.h"
#include "actiontree.h"
#include "alog.h"

/*
 *  Constructs a CfgForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
CfgForm::CfgForm(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
}

/*
 *  Destroys the object and frees any allocated resources
 */
CfgForm::~CfgForm()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CfgForm::languageChange()
{
    retranslateUi(this);
}

extern MainForm *mainform;

void set_Icon(QTreeWidgetItem *item, const char *name)
{
    QString iconName = QString(name) + ".png";
    item->setIcon(0, rcIcon(iconName.toUtf8().constData()));
}


/*class ObjEditing {
    public:
    ObjEditing(){item=NULL; widget=NULL;}
    ObjEditing(QListViewItem *aitem, QWidget *awidget) {item=aitem;widget=awidget;}

    QListViewItem *item;
    QWidget *widget;
};
*/
//typedef QValueList<ObjEditing> ObjEditingList;
//ObjEditingList ObjEdList;

CfgForm *configform=NULL;


void CfgForm::fileNew()
{

}

void CfgForm::fileOpen()
{

}

void CfgForm::fileSave()
{

}

void CfgForm::fileSaveAs()
{

}

void CfgForm::filePrint()
{

}

void CfgForm::fileExit()
{
}


void
CfgForm::init(QString &rcf, bool flagNew)
{
    statusBar()->hide();
    configform = this;
    rcfile = rcf;
    setWindowIcon(rcIcon("a-system.png"));

    if (!flagNew) {
        if (cfg.readrc(rcfile)) {
            aLog::print(aLog::Error, tr("CfgForm readrc error"));
            return;
        }
    }

    setWindowTitle(tr("Business scheme: ") + cfg.info(mda_name));

    QGridLayout *l = new QGridLayout(tabWidget->widget(0));
    l->setContentsMargins(2, 2, 2, 2);
    mdtree = new aMetadataTreeView(tabWidget->widget(0), &cfg);
    l->addWidget(mdtree, 0, 0);

    initLang();
    initRoles();
    initImageCollection();

    mainform->TBToolbar->setVisible(true);
    mainform->configSaveAction->setVisible(true);

    actiontree = new aActionTreeView(tabWidget->widget(1), &cfg);
    QGridLayout *j = new QGridLayout(tabWidget->widget(1));
    j->setContentsMargins(2, 2, 2, 2);
    j->addWidget(actiontree, 0, 0);

    interfacetree = new InterfaceTreeView(tabWidget->widget(2), &cfg);
    QGridLayout *k = new QGridLayout(tabWidget->widget(2));
    k->setContentsMargins(2, 2, 2, 2);
    k->addWidget(interfacetree, 0, 0);

    connect(mainform->objMetadataNewAction, SIGNAL(triggered()), mdtree, SLOT(itemNew()));
    connect(mainform->objMetadataEditAction, SIGNAL(triggered()), mdtree, SLOT(itemEdit()));
    connect(mainform->objActionsNewAction, SIGNAL(triggered()), actiontree, SLOT(itemNewAction()));
    connect(mainform->objActionsNewGroupAction, SIGNAL(triggered()), actiontree, SLOT(itemNewGroup()));
    connect(mainform->objActionsEditAction, SIGNAL(triggered()), actiontree, SLOT(itemEdit()));
    connect(mainform->objInterfaceNewCommandAction, SIGNAL(triggered()), interfacetree, SLOT(itemNewCommand()));
    connect(mainform->objInterfaceNewSubmenuAction, SIGNAL(triggered()), interfacetree, SLOT(itemNewSubmenu()));
    connect(mainform->objInterfaceNewSeparatorAction, SIGNAL(triggered()), interfacetree, SLOT(itemNewSeparator()));
    connect(mainform->objInterfaceEditAction, SIGNAL(triggered()), interfacetree, SLOT(itemEdit()));
    connect(mainform->objImageCollectionAddAction, SIGNAL(triggered()), this, SLOT(bAddImage_clicked()));
    connect(mainform->objLanguagesNewAction, SIGNAL(triggered()), this, SLOT(bAdd_clicked()));
    connect(mainform->objLanguagesEditAction, SIGNAL(triggered()), this, SLOT(bEdit_clicked()));
    connect(mainform->objLanguagesClearAction, SIGNAL(triggered()), this, SLOT(bKill_clicked()));
    connect(mainform->objRoleNewAction, SIGNAL(triggered()), this, SLOT(bAddRole_clicked()));
    connect(mainform->objRoleEditAction, SIGNAL(triggered()), this, SLOT(bEditRole_clicked()));
    connect(mainform->objRoleClearAction, SIGNAL(triggered()), this, SLOT(bKillRole_clicked()));
}

/*
  * Image collection initialization
  */
void CfgForm::initImageCollection()
{
    aCfgItem oroot, image_collection, image;
    int i, n;
    QPixmap pix;
    QString name;

    oroot = cfg.find(mdc_root);
    image_collection = cfg.find(oroot, md_image_collection, 0);
    if (image_collection.isNull())
        image_collection = cfg.insert(oroot, md_image_collection, QString(), -1);

    n = cfg.countChild(image_collection, md_image);
    for (i = 0; i < n; i++) {
        image = cfg.findChild(image_collection, md_image, i);
        name = cfg.attr(image, mda_name);
        pix.loadFromData(cfg.binary(image));

        QListWidgetItem *item = new QListWidgetItem(pix, name, vImageCollection);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        idList.insert(cfg.id(image), item);
    }

    if (vImageCollection->count())
        bRemoveImage->setEnabled(true);
}

/*
  * Add image into collection
  */
void CfgForm::bAddImage_clicked()
{
    QPixmap pix;
    QByteArray ba;
    QString name;
    QStringList names;
    aCfgItem image_collection, image;
    QFile f;

    names = QFileDialog::getOpenFileNames(
        this,
        tr("Open image dialog"),
        QString(),
        tr("Images (*.png *.xpm *.jpg *.jpeg *.bmp)")
    );

    QStringList::Iterator it = names.begin();
    while (it != names.end()) {
        name = *it;
        ++it;

        if (!name.isEmpty()) {
            f.setFileName(name);
            if (f.open(QIODevice::ReadOnly)) {
                ba = f.readAll();
                f.close();
                pix.loadFromData(ba);
            } else {
                continue;
            }
        }

        QFileInfo fi(name);
        name = fi.baseName();

        QListWidgetItem *item = new QListWidgetItem(pix, name, vImageCollection);
        item->setFlags(item->flags() | Qt::ItemIsEditable);

        image_collection = cfg.findChild(cfg.find(mdc_root), md_image_collection, 0);
        image = cfg.insert(image_collection, md_image, name);
        cfg.setBinary(image, ba);
        idList.insert(cfg.id(image), item);
    }

    if (!names.isEmpty()) {
        bRemoveImage->setEnabled(true);
        mainform->objTBDeleteAction->setEnabled(true);
    }
}

/*
  * Remove image from collection
  */
void CfgForm::bRemoveImage_clicked()
{
    QListWidgetItem *current;
    aCfgItem image_collection, image;
    int i, n, id;

    image_collection = cfg.findChild(cfg.find(mdc_root), md_image_collection, 0);
    n = cfg.countChild(image_collection, md_image);
    current = vImageCollection->currentItem();

    for (i = 0; i < n; i++) {
        image = cfg.findChild(image_collection, md_image, i);
        id = cfg.id(image);

        if (current == idList.value(id)) {
            cfg.remove(image);
            idList.remove(id);
            delete current;
            break;
        }
    }

    if (!vImageCollection->count()) {
        bRemoveImage->setEnabled(false);
        mainform->objTBDeleteAction->setEnabled(false);
    }

    vImageCollection->sortItems();
}

/*
  * Image rename event handler
  */
void CfgForm::vImageCollection_itemChanged(QListWidgetItem *item)
{
    aCfgItem image_collection, image;
    int i, n;

    if (!item) return;

    image_collection = cfg.findChild(cfg.find(mdc_root), md_image_collection, 0);
    n = cfg.countChild(image_collection, md_image); // count images

    for (i = 0; i < n; i++) {
        image = cfg.findChild(image_collection, md_image, i); // get image
        if (item == idList.value(cfg.id(image))) {            // if corresponds
            cfg.setAttr(image, mda_name, item->text());       // rename metaitem
            break;
        }
    }
}

/*
  * Languages initialization
  */
void CfgForm::initLang()
{
    aCfgItem oroot;
    aCfgItem lang, obj;
    int i, n;

    tLang->setRowCount(0);
    oroot = cfg.find(mdc_root);

    if (oroot.isNull()) {
        aLog::print(aLog::Info, tr("CfgForm metadata root is null"));
    }

    lang = cfg.findChild(oroot, md_languages, 0);
    if (lang.isNull()) {
        aLog::print(aLog::Info, tr("CfgForm metadata lang is null"));
    }

    if (lang.isNull())
        lang = cfg.insert(oroot, md_languages, QString(), -1);

    n = cfg.count(lang, md_language);
    aLog::print(aLog::Debug, QString(tr("lang = %1")).arg(n));

    for (i = 0; i < n; i++) {
        obj = cfg.find(lang, md_language, i);
        if (obj.isNull())
            aLog::print(aLog::Debug, QString(tr("CfgForm %1 metadata lang is null")).arg(i));

        int row = tLang->rowCount();
        tLang->insertRow(row);
        tLang->setItem(row, 0, new QTableWidgetItem(cfg.attr(obj, mda_tag)));
        tLang->setItem(row, 1, new QTableWidgetItem(cfg.attr(obj, mda_name)));
        tLang->setItem(row, 2, new QTableWidgetItem(cfg.attr(obj, mda_trfile)));
    }

    if (!tLang->rowCount()) {
        bDelete->setEnabled(false);
        bEdit->setEnabled(false);
        bKill->setEnabled(false);
        mainform->objLanguagesClearAction->setEnabled(false);
        mainform->objLanguagesEditAction->setEnabled(false);
    }
}

/*
  * Roles initialization
  */
void CfgForm::initRoles()
{
    aCfgItem oroot;
    aCfgItem role, obj;
    int i, n;
    QString rd;

    tRole->setRowCount(0);
    oroot = cfg.find(mdc_root);
    role = cfg.find(oroot, md_roles, 0);

    if (role.isNull())
        role = cfg.insert(oroot, md_roles, QString(), -1);

    n = cfg.count(role, md_role);

    for (i = 0; i < n; i++) {
        obj = cfg.find(role, md_role, i);

        int row = tRole->rowCount();
        tRole->insertRow(row);
        tRole->setItem(row, 0, new QTableWidgetItem(cfg.attr(obj, mda_name)));

        rd = cfg.sText(obj, md_description);
        rd.replace(QChar('\n'), " ");
        tRole->setItem(row, 1, new QTableWidgetItem(rd));
    }

    if (!tRole->rowCount()) {
        bDeleteRole->setEnabled(false);
        bEditRole->setEnabled(false);
        bKillRole->setEnabled(false);
        mainform->objRoleClearAction->setEnabled(false);
        mainform->objRoleEditAction->setEnabled(false);
    }
}

/*
  * Destroy configuration form
  */
void CfgForm::destroy()
{
    configform = 0;
//  globalCfg = 0;
    mainform->cfgform = 0;
    mainform->removeTab(objectName());
}

/*
  * Save metadata on disk
  */
void CfgForm::save()
{
    aDatabase *database = aDatabase::database();

    if (cfg.writerc(rcfile))
        cfg_message(0, tr("Unable to write rc file.").toUtf8().constData());
    else
        database->update();
}

/*
  * Add new language
  */
void CfgForm::bAdd_clicked()
{
    aCfgItem lang, obj;

    int row = tLang->rowCount();
    tLang->insertRow(row);
    tLang->setCurrentCell(row, 0);

    lang = cfg.find(cfg.find(mdc_root), md_languages, 0);
    obj = cfg.insert(lang, md_language, tr("New language"));

    dEditLang *e = new dEditLang(ws, 0);
    e->setAttribute(Qt::WA_DeleteOnClose);
    e->setData(configform, obj);
    e->show();

    bDelete->setEnabled(true);
    bEdit->setEnabled(true);
    bKill->setEnabled(true);
    mainform->objLanguagesClearAction->setEnabled(true);
    mainform->objLanguagesEditAction->setEnabled(true);
    mainform->objTBDeleteAction->setEnabled(true);
}

/*
  * Delete language
  */
void CfgForm::bDelete_clicked()
{
    QTableWidgetItem *itemTag = tLang->item(tLang->currentRow(), 0);
    QTableWidgetItem *itemName = tLang->item(tLang->currentRow(), 1);

    QString tag = itemTag ? itemTag->text() : QString();
    QString name = itemName ? itemName->text() : QString();

    if (QMessageBox::warning(this,
                             tr("Deleting row"),
                             tr("Delete '%1'?").arg(name),
                             QMessageBox::Yes,
                             QMessageBox::No) == QMessageBox::Yes) {

        aCfgItem lang, obj;
        int i, n;

        lang = cfg.find(cfg.find(mdc_root), md_languages, 0);
        n = cfg.count(lang, md_language);

        for (i = 0; i < n; i++) {
            obj = cfg.find(lang, md_language, i);
            if (cfg.attr(obj, mda_tag) == tag) {
                cfg.remove(obj);
                break;
            }
        }

        tLang->removeRow(tLang->currentRow());

        if (!tLang->rowCount()) {
            bDelete->setEnabled(false);
            bEdit->setEnabled(false);
            bKill->setEnabled(false);
            mainform->objLanguagesClearAction->setEnabled(false);
            mainform->objLanguagesEditAction->setEnabled(false);
            mainform->objTBDeleteAction->setEnabled(false);
        }
    }
}

/*
  * Language editing
  */
void CfgForm::bEdit_clicked()
{
    aCfgItem obj;

    obj = cfg.find(cfg.find(cfg.find(mdc_root), md_languages, 0),
                   md_language,
                   tLang->currentRow());

    dEditLang *e = new dEditLang(ws, 0);
    e->setAttribute(Qt::WA_DeleteOnClose);
    e->setData(configform, obj);
    e->show();
}

/*
  * Double-click table event
  */
void CfgForm::tLang_doubleClicked( int, int, int, const QPoint & )
{
    bEdit_clicked();		// call edit button click event
}

/*
  * Kill dead aliases througout metadata
  */
void CfgForm::bKill_clicked()
{
    aCfgItem oroot, alias;
    int i = 0, s, n, ac, killed = 0;

    oroot = cfg.find(mdc_root);
    n = cfg.count(oroot, md_alias);

    while (i < n) {
        ac = 0;
        alias = cfg.find(oroot, md_alias, i);

        for (s = 0; s < tLang->rowCount(); s++) {
            QTableWidgetItem *item = tLang->item(s, 0);
            if (item && cfg.attr(alias, mda_tag) == item->text())
                ac++;
        }

        if (!ac) {
            cfg.remove(alias);
            killed++;
            n--;
        } else {
            i++;
        }
    }

    QMessageBox::warning(this,
                         tr("Clearing dead aliases"),
                         tr("%1 dead aliases killed.").arg(killed),
                         QMessageBox::Ok);
}

void CfgForm::newObj()
{
}


void CfgForm::listCfg_onItem( QTreeWidgetItem * )
{

}



void CfgForm::newField()
{
}



void CfgForm::newForm()
{
}


void CfgForm::newTable()
{
}

/*
  * Cofiguration form close event
  */
void CfgForm::closeEvent(QCloseEvent *e)
{
    if (!cfg.modified()) {
        QMainWindow::closeEvent(e);
        return;
    }

    QMessageBox::StandardButton res =
        QMessageBox::warning(this,
                             tr("Saving changes"),
                             tr("Save changes %1?").arg(windowTitle()),
                             QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    switch (res) {
    case QMessageBox::Yes:
        mainform->closeChildWindows();
        save();
        QMainWindow::closeEvent(e);
        break;

    case QMessageBox::No:
        mainform->closeChildWindows();
        QMainWindow::closeEvent(e);
        break;

    default:
        e->ignore();
        break;
    }

    mainform->TBToolbar->setVisible(false);
    mainform->configSaveAction->setVisible(false);
    mainform->tbMetadata->setVisible(false);
    mainform->tbActions->setVisible(false);
    mainform->tbInterface->setVisible(false);
    mainform->tbRights->setVisible(false);
    mainform->tbLanguages->setVisible(false);
    mainform->tbImageCollection->setVisible(false);
}

/*
  * Subwindow close event
  */
int CfgForm::closeSubWindows()
{
    QList<QMdiSubWindow*> windows = ws->subWindowList();

    if (!windows.isEmpty()) {
        for (int i = 0; i < windows.size(); ++i) {
            QMdiSubWindow *sub = windows.at(i);
            QWidget *window = sub->widget();

            if (window != this) {

                if (window->metaObject()->className() == QString("messageswindow")) {
                    delete sub;
                    continue;
                }

                if (!window->close())
                    return 1;
                else
                    delete sub;
            }
        }
    }
    return 0;
}



/*
int CfgForm::activateExist( QListViewItem *item )
{
    QWidget *window;

    window=findOpened(item);
    if (window) {
	window->setFocus();
	return 1;
    }
    return 0;
}
*/

//QWidget *
//CfgForm::findOpened( QListViewItem *item )
//{
/*
    ObjEditingList::iterator it;
    QWidgetList windows = ws->windowList();
    QWidget *window, *res=NULL;
    int f;

    if ( windows.count() ) {
	it = ObjEdList.begin();
	while ( it != ObjEdList.end()) {
	    f=0;
	    for ( int i = 0; i < int(windows.count()); ++i ) {
		window = windows.at( i );
		if (window==(*it).widget) {
		    f=1; break;
		}
	    }
	    if (!f) {
		it=ObjEdList.remove(it);
	    } else it++;
	}
	for ( it = ObjEdList.begin(); it != ObjEdList.end(); ++it ) {
	    if (item==(*it).item) {
		res=(*it).widget;
		break;
	    }
	}
    }
    return res;
*/
//    return NULL;
//}

/*
  * When tab switches some actions reconnect to corresponding toolbar buttons and toolbars recombinate
  */
void CfgForm::tabWidget_selected(const QString &tab)
{
    bool fM, fA, fI, fR, fL, fIC;

    if (tab == tr("Metadata")) {
        connect(mainform->objTBDeleteAction, SIGNAL(triggered()), mdtree, SLOT(itemDelete()));
        connect(mainform->objTBRenameAction, SIGNAL(triggered()), mdtree, SLOT(itemRename()));
        connect(mainform->objTBMoveUpAction, SIGNAL(triggered()), mdtree, SLOT(itemMoveUp()));
        connect(mainform->objTBMoveDownAction, SIGNAL(triggered()), mdtree, SLOT(itemMoveDown()));
        fM = true;
    } else {
        disconnect(mainform->objTBDeleteAction, SIGNAL(triggered()), mdtree, SLOT(itemDelete()));
        disconnect(mainform->objTBRenameAction, SIGNAL(triggered()), mdtree, SLOT(itemRename()));
        disconnect(mainform->objTBMoveUpAction, SIGNAL(triggered()), mdtree, SLOT(itemMoveUp()));
        disconnect(mainform->objTBMoveDownAction, SIGNAL(triggered()), mdtree, SLOT(itemMoveDown()));
        fM = false;
    }

    if (tab == tr("Actions")) {
        connect(mainform->objTBDeleteAction, SIGNAL(triggered()), actiontree, SLOT(itemDelete()));
        connect(mainform->objTBRenameAction, SIGNAL(triggered()), actiontree, SLOT(itemRename()));
        connect(mainform->objTBMoveUpAction, SIGNAL(triggered()), actiontree, SLOT(itemMoveUp()));
        connect(mainform->objTBMoveDownAction, SIGNAL(triggered()), actiontree, SLOT(itemMoveDown()));
        fA = true;
    } else {
        disconnect(mainform->objTBDeleteAction, SIGNAL(triggered()), actiontree, SLOT(itemDelete()));
        disconnect(mainform->objTBRenameAction, SIGNAL(triggered()), actiontree, SLOT(itemRename()));
        disconnect(mainform->objTBMoveUpAction, SIGNAL(triggered()), actiontree, SLOT(itemMoveUp()));
        disconnect(mainform->objTBMoveDownAction, SIGNAL(triggered()), actiontree, SLOT(itemMoveDown()));
        fA = false;
    }

    if (tab == tr("Interface")) {
        connect(mainform->objTBDeleteAction, SIGNAL(triggered()), interfacetree, SLOT(itemDelete()));
        connect(mainform->objTBRenameAction, SIGNAL(triggered()), interfacetree, SLOT(itemRename()));
        connect(mainform->objTBMoveUpAction, SIGNAL(triggered()), interfacetree, SLOT(itemMoveUp()));
        connect(mainform->objTBMoveDownAction, SIGNAL(triggered()), interfacetree, SLOT(itemMoveDown()));
        fI = true;
    } else {
        disconnect(mainform->objTBDeleteAction, SIGNAL(triggered()), interfacetree, SLOT(itemDelete()));
        disconnect(mainform->objTBRenameAction, SIGNAL(triggered()), interfacetree, SLOT(itemRename()));
        disconnect(mainform->objTBMoveUpAction, SIGNAL(triggered()), interfacetree, SLOT(itemMoveUp()));
        disconnect(mainform->objTBMoveDownAction, SIGNAL(triggered()), interfacetree, SLOT(itemMoveDown()));
        fI = false;
    }

    if (tab == tr("Rights")) {
        connect(mainform->objTBDeleteAction, SIGNAL(triggered()), this, SLOT(bDeleteRole_clicked()));
        fR = true;
        if (!tRole->rowCount()) mainform->objTBDeleteAction->setEnabled(false);
        else mainform->objTBDeleteAction->setEnabled(true);
    } else {
        disconnect(mainform->objTBDeleteAction, SIGNAL(triggered()), this, SLOT(bDeleteRole_clicked()));
        fR = false;
    }

    if (tab == tr("Languages")) {
        connect(mainform->objTBDeleteAction, SIGNAL(triggered()), this, SLOT(bDelete_clicked()));
        fL = true;
        if (!tLang->rowCount()) mainform->objTBDeleteAction->setEnabled(false);
        else mainform->objTBDeleteAction->setEnabled(true);
    } else {
        disconnect(mainform->objTBDeleteAction, SIGNAL(triggered()), this, SLOT(bDelete_clicked()));
        fL = false;
    }

    if (tab == tr("Image collection")) {
        connect(mainform->objTBDeleteAction, SIGNAL(triggered()), this, SLOT(bRemoveImage_clicked()));
        fIC = true;
        if (!vImageCollection->count()) mainform->objTBDeleteAction->setEnabled(false);
        else mainform->objTBDeleteAction->setEnabled(true);
    } else {
        disconnect(mainform->objTBDeleteAction, SIGNAL(triggered()), this, SLOT(bRemoveImage_clicked()));
        fIC = false;
    }

    if (tab == tr("Image collection") || tab == tr("Languages") || tab == tr("Rights")) {
        mainform->objTBRenameAction->setVisible(false);
        mainform->objTBMoveUpAction->setVisible(false);
        mainform->objTBMoveDownAction->setVisible(false);
    } else {
        mainform->objTBRenameAction->setVisible(true);
        mainform->objTBMoveUpAction->setVisible(true);
        mainform->objTBMoveDownAction->setVisible(true);
        mainform->objTBDeleteAction->setEnabled(true);
    }

    mainform->tbMetadata->setVisible(fM);
    mainform->tbActions->setVisible(fA);
    mainform->tbInterface->setVisible(fI);
    mainform->tbRights->setVisible(fR);
    mainform->tbLanguages->setVisible(fL);
    mainform->tbImageCollection->setVisible(fIC);
}

/*
  * Adds new role
  */
void CfgForm::bAddRole_clicked()
{
    aCfgItem role, obj, roles;

    int row = tRole->rowCount();
    tRole->insertRow(row);                 // insert row
    tRole->setCurrentCell(row, 0);         // focus new cell

    roles = cfg.find(cfg.find(mdc_root), md_roles, 0);
    obj = cfg.insert(roles, md_role, tr("New role"));

    dEditRole *e = new dEditRole(ws, 0);
    e->setAttribute(Qt::WA_DeleteOnClose);
    e->setData(configform, obj);
    e->show();

    bDeleteRole->setEnabled(true);
    bEditRole->setEnabled(true);
    bKillRole->setEnabled(true);
    mainform->objRoleClearAction->setEnabled(true);
    mainform->objRoleEditAction->setEnabled(true);
    mainform->objTBDeleteAction->setEnabled(true);
}

/*
  * Delete role
  */
void CfgForm::bDeleteRole_clicked()
{
    QTableWidgetItem *currentItem = tRole->item(tRole->currentRow(), 0);
    QString roleName = currentItem ? currentItem->text() : QString();

    if (QMessageBox::warning(this,
                             tr("Deleting row"),
                             tr("Delete '%1'?").arg(roleName),
                             QMessageBox::Yes,
                             QMessageBox::No) == QMessageBox::Yes) {
        aCfgItem roles, obj;
        int i, n;

        roles = cfg.find(cfg.find(mdc_root), md_roles, 0);
        n = cfg.count(roles, md_role);

        for (i = 0; i < n; i++) {
            obj = cfg.find(roles, md_role, i);
            if (cfg.attr(obj, mda_name) == roleName) {
                cfg.remove(obj);
                break;
            }
        }

        tRole->removeRow(tRole->currentRow());

        if (!tRole->rowCount()) {
            bDeleteRole->setEnabled(false);
            bEditRole->setEnabled(false);
            bKillRole->setEnabled(false);
            mainform->objRoleClearAction->setEnabled(false);
            mainform->objRoleEditAction->setEnabled(false);
            mainform->objTBDeleteAction->setEnabled(false);
        }
    }
}

/*
  * Edit curren role
  */
void CfgForm::bEditRole_clicked()
{
    int objid;
    aCfgItem obj;

    obj = cfg.find(cfg.find(cfg.find(mdc_root), md_roles, 0), md_role, tRole->currentRow());

/*  objid = cfg.id(obj);
    if (wl->find(objid)) {
        wl->get(objid)->setFocus();
        return;
    }
*/

    dEditRole *e = new dEditRole(ws, 0);
    e->setAttribute(Qt::WA_DeleteOnClose);
    e->setData(configform, obj);
    e->show();
}

/*
  * Kill dead roles throughout the configuration
  */
void CfgForm::bKillRole_clicked()
{
    aCfgItem oroot, roles, role;
    int i = 0, s, n, ac, killed = 0;

    oroot = cfg.find(mdc_root);                 // get configuration root
    n = cfg.count(oroot, md_role);              // count roles

    while (i < n) {
        ac = 0;
        role = cfg.find(oroot, md_role, i);

        for (s = 0; s < tRole->rowCount(); s++) {
            QTableWidgetItem *item = tRole->item(s, 0);
            if (item && cfg.attr(role, mda_name) == item->text())
                ac++;
        }

        if (!ac) {
            cfg.remove(role);
            killed++;
            n--;
        } else {
            i++;
        }
    }

    QMessageBox::warning(this,
                         tr("Clearing dead roles"),
                         tr("%1 dead roles killed.").arg(killed),
                         QMessageBox::Ok);
}


void CfgForm::tRole_doubleClicked( int, int, int, const QPoint & )
{
    bEditRole_clicked();	// call role edit method
}

