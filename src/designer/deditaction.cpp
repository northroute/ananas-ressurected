#include "deditaction.h"

#include "acfg.h"


/*
 *  Constructs a dEditAction as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditAction::dEditAction(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    statusBar(); // создаёт при необходимости
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditAction::~dEditAction()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditAction::languageChange()
{
    retranslateUi(this);
}

void dEditAction::setData(ActionListViewItem *o)
{
    item = o;
    md = o->md;
    obj = o->obj;

    aCfgItem active, passive;
    QString st;
    QPixmap pix, pix1;

    static const char *empty[] = {
        "1 1 1 1",
        ". c None",
        "."
    };
    dp = new QPixmap(empty);

    aAliasEditor *a = new aAliasEditor(md, obj, tAliases);
    al = a;
    al->setData();

    pActive->setPixmap(*dp);
    pPassive->setPixmap(*dp);

    eName->setText(md->attr(obj, mda_name));
    setWindowTitle(tr("Action:") + md->attr(obj, mda_name));
    eDescription->setPlainText(md->sText(obj, md_description));
    eParams->setText(md->sText(obj, md_argument));

    st = md->attr(obj, mda_type);
    if (st == "0") cbType->setCurrentIndex(0);
    else if (st == "1") cbType->setCurrentIndex(1);

    eModule->setPlainText(md->sText(obj, md_sourcecode));

    active = md->findChild(obj, md_active_picture, 0);
    if (!active.isNull()) {
        bRemoveActive->setEnabled(true);
        pix.loadFromData(md->binary(active));
        pActive->setPixmap(pix);
        setWindowIcon(pix);
    }

    passive = md->findChild(obj, md_passive_picture, 0);
    if (!passive.isNull()) {
        bRemovePassive->setEnabled(true);
        pix1.loadFromData(md->binary(passive));
        pPassive->setPixmap(pix1);
    }

    QStringList tlist = md->ObjectTypes();
    otypes.clear();
    cbObject->clear();

    for (QStringList::Iterator it = tlist.begin(); it != tlist.end(); ++it) {
        otypes.append((*it).section("\t", 0, 0).section(" ", 1, 1));
        cbObject->addItem((*it).section("\t", 1, 1));
    }

    QString oid = md->sText(obj, md_objectid);
    uint n = otypes.count();
    for (uint i = 0; i < n; i++) {
        if (otypes[i] == oid) {
            cbObject->setCurrentIndex(i);
            break;
        }
    }

    cbObjectChange();

    n = formslist.count();
    oid = md->sText(obj, md_formid);
    for (uint i = 0; i < n; i++) {
        if (formslist[i] == oid) {
            cbForm->setCurrentIndex(i);
            break;
        }
    }

    int type = md->sText(obj, md_actiontype).toInt();
    switch (type)
    {
    case 0:
        cbAction->setCurrentIndex(md_action_new);
        break;
    case 1:
        cbAction->setCurrentIndex(md_action_edit);
        break;
    case 2:
        cbAction->setCurrentIndex(md_action_view);
        break;
    case 3:
        cbAction->setCurrentIndex(md_action_execute);
        break;
    }

    cbTypeChange();
}

void dEditAction::updateMD()
{
    al->updateMD();
    char sid[50] = "";
    int id = 0;

    item->setText(0, eName->text());
    md->setAttr(obj, mda_name, eName->text());
    md->setSText(obj, md_description, eDescription->toPlainText());
    md->setSText(obj, md_argument, eParams->text());

    if (cbType->currentIndex() == 0)
    {
        md->setAttr(obj, mda_type, "0");

        id = cbAction->currentIndex();
        switch (id)
        {
        case 0:
            sprintf(sid, "%i", md_action_new);
            md->setSText(obj, md_actiontype, sid);
            break;
        case 1:
            sprintf(sid, "%i", md_action_edit);
            md->setSText(obj, md_actiontype, sid);
            break;
        case 2:
            sprintf(sid, "%i", md_action_view);
            md->setSText(obj, md_actiontype, sid);
            break;
        case 3:
            sprintf(sid, "%i", md_action_execute);
            md->setSText(obj, md_actiontype, sid);
            break;
        }

        id = cbObject->currentIndex();
        md->setSText(obj, md_objectid, otypes[id]);

        id = cbForm->currentIndex();
        md->setSText(obj, md_formid, formslist[id]);
    }

    if (cbType->currentIndex() == 1)
    {
        md->setAttr(obj, mda_type, "1");
        md->setSText(obj, md_sourcecode, eModule->toPlainText());
    }
}


void dEditAction::init()
{
    statusBar()->hide();
    //eModule->setInterpreter(new QSInterpreter());
    // tabWidget2->setTabEnabled(1, false);
}

void dEditAction::destroy()
{
    updateMD();

    MainForm *mf = qobject_cast<MainForm*>(window());
    if (mf) {
        mf->wl->remove(this);
        mf->removeTab(objectName());
    }
}


void dEditAction::cbTypeChange()
{
    wStack->setCurrentIndex(cbType->currentIndex());
}

void dEditAction::selectStandart()
{
    lAction->show();
    lObject->show();
    lParams->show();
    cbAction->show();
    cbObject->show();
    cbForm->show();
    eParams->show();
//    tabWidget2->page(1)->hide() ;
    eModule->hide() ;
//    tabWidget2->setTabEnabled ( tabWidget2->page(1), FALSE );
}



void dEditAction::selectUser()
{

    lAction->hide();
    lObject->hide();
    lParams->hide();
    cbAction->hide();
    cbObject->hide();
    cbForm->hide();
    eParams->hide();
    eModule->show() ;
//    tabWidget2->setTabEnabled ( tabWidget2->page(1), TRUE );
}


void dEditAction::bSetActive_clicked()
{
    QString st;
    QPixmap pix;
    QByteArray ba;
    QString name;
    QFile f;
    aCfgItem active;

    name = QFileDialog::getOpenFileName(
        this,
        tr("Open image dialog"),
        QString(),
        tr("Images (*.png *.xpm *.jpg *.jpeg *.bmp)")
    );

    if (!name.isEmpty()) {
        f.setFileName(name);
        if (f.open(QIODevice::ReadOnly)) {
            ba = f.readAll();
            f.close();

            pix.loadFromData(ba);
            active = item->md->findChild(item->obj, md_active_picture, 0);
            if (active.isNull())
                active = item->md->insert(item->obj, md_active_picture, QString(), -1);

            item->md->setBinary(active, ba);
            item->setIcon(0, pix);
            setWindowIcon(pix);
        }
    }

    pActive->setPixmap(pix);
    bRemoveActive->setEnabled(true);
}


void dEditAction::bSetPassive_clicked()
{
    QString st;
    QPixmap pix;
    QFileDialog *fd = new QFileDialog(this);
    fd->setNameFilter("Images (*.png *.xpm *.jpg *.jpeg *.bmp)");
    QByteArray ba;
    QString name;
    QFile f;
    PixmapPreview *p = new PixmapPreview;
    aCfgItem passive;

    fd->setOption(QFileDialog::DontUseNativeDialog, true);
    fd->setFileMode(QFileDialog::ExistingFiles);
    fd->setWindowTitle(tr("Open image dialog"));

    if (fd->exec() == QDialog::Accepted) {
        name = fd->selectedFiles().first();

        if (!name.isEmpty()) {
            f.setFileName(name);
            if (f.open(QIODevice::ReadOnly)) {
                ba = f.readAll();
                f.close();

                pix.loadFromData(ba);

                passive = item->md->findChild(item->obj, md_passive_picture, 0);
                if (passive.isNull())
                    passive = item->md->insert(item->obj, md_passive_picture, QString(), -1);

                item->md->setBinary(passive, ba);
            }
        }

        pPassive->setPixmap(pix);
        bRemovePassive->setEnabled(true);
    }

    delete fd;
}


void dEditAction::bRemoveActive_clicked()
{
    item->md->remove(item->md->findChild(item->obj, md_active_picture, 0));
    bRemoveActive->setEnabled(false);
    pActive->clear();
    item->setIcon(0, QIcon());
    setWindowIcon(*dp);
    pActive->setPixmap(*dp);
}


void dEditAction::bRemovePassive_clicked()
{
    item->md->remove( item->md->findChild( item->obj, md_passive_picture, 0 ) );
    bRemovePassive->setEnabled( FALSE );
    pPassive->clear();
    pPassive->setPixmap( *dp );
}


void dEditAction::cbObjectChange()
{
    aCfgItem cobj, fobj;
    long id = otypes[cbObject->currentIndex()].toLong();
    uint n;

    cobj = md->find(id);

    cbForm->clear();
    formslist.clear();

    cbForm->addItem(QObject::tr("default"));
    formslist.append("0");

    if (md->objClass(cobj) == md_catalogue) {
        cbForm->addItem(QObject::tr("embedded editor"));
        formslist.append("1");
    }

    n = md->count(cobj, md_form);
    for (uint i = 0; i < n; i++) {
        fobj = md->find(cobj, md_form, i);
        if (fobj.isNull())
            continue;

        cbForm->addItem(md->attr(fobj, mda_name));
        formslist.append(md->attr(fobj, mda_id));
    }
}
