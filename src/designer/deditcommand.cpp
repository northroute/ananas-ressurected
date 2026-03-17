#include "deditcommand.h"

#include "acfg.h"

//extern aCfg cfg;

/*
 *  Constructs a dEditCommand as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditCommand::dEditCommand(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditCommand::~dEditCommand()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditCommand::languageChange()
{
    retranslateUi(this);
}

void dEditCommand::updateMD()
{
    aCfgItem com_action, apix;
    al->updateMD();
    QPixmap pix;

    item->setText(0, eName->text().trimmed());
    md->setAttr(obj, mda_name, eName->text().trimmed());
    md->setSText(obj, md_description, eDescription->toPlainText());
    md->setSText(obj, md_menutext, eMenuText->text());

    QString sKey = "";
    if (cbKey->currentText() != "") {
        if (cbCTRL->isChecked())
            sKey += md_km_ctrl;
        if (cbALT->isChecked())
            sKey += md_km_alt;
        if (cbShift->isChecked())
            sKey += md_km_shift;
        sKey += cbKey->currentText();
    }
    md->setSText(obj, md_key, sKey);

    do {
        com_action = md->findChild(obj, md_comaction, 0);
        if (!com_action.isNull())
            md->remove(com_action);
    } while (!com_action.isNull());

    QTreeWidgetItem *it = vComActions->topLevelItem(0);
    while (it) {
        ananasListViewItem *aitem = static_cast<ananasListViewItem*>(it);

        com_action = md->insert(obj, md_comaction, QString(), -1);
        md->setText(com_action, QString("%1").arg(aitem->id));

        int index = vComActions->indexOfTopLevelItem(it);
        it = vComActions->topLevelItem(index + 1);
    }

    ananasListViewItem *firstItem =
        static_cast<ananasListViewItem*>(vComActions->topLevelItem(0));

    if (!firstItem)
        return;

    apix = md->findChild(md->find(firstItem->id), md_active_picture, 0);
    if (apix.isNull())
        return;

    pix.loadFromData(md->binary(apix));
    item->setIcon(0, QIcon(pix));
}

void dEditCommand::closeEditor()
{
    updateMD();
    ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}

void dEditCommand::init()
{
	statusBar()->hide();
}

void dEditCommand::setData(InterfaceListViewItem *o)
{
    int i, n, k, id;
    item = o;
    md = o->md;
    obj = o->obj;

    aCfgItem com_action, apix;
    aAliasEditor *a = new aAliasEditor(md, obj, tAliases);
    QPixmap pix;

    vComActions = new ananasTreeView(tabWidget2->widget(1), md);
    vComActions->setSortingEnabled(false);
    // TODO Fixme!!!
    // layout28->addWidget(vComActions, 0, 0);

    actiontree = new aActionTreeView(tabWidget2->widget(1), md);
    // TODO Fixme!!!
    // layout29->addWidget(actiontree, 0, 0);

    al = a;
    al->setData();

    setWindowTitle(tr("Command:") + md->attr(obj, mda_name));
    eName->setText(md->attr(obj, mda_name));
    eMenuText->setText(md->sText(obj, md_menutext));
    eDescription->setPlainText(md->sText(obj, md_description));

    QString sKey = md->sText(obj, md_key);

    if (sKey.contains(md_km_ctrl)) {
        cbCTRL->setChecked(true);
        sKey.remove(md_km_ctrl);
    }
    if (sKey.contains(md_km_alt)) {
        cbALT->setChecked(true);
        sKey.remove(md_km_alt);
    }
    if (sKey.contains(md_km_shift)) {
        cbShift->setChecked(true);
        sKey.remove(md_km_shift);
    }

    n = cbKey->count();
    for (i = 0; i < n; i++) {
        if (sKey == cbKey->itemText(i)) {
            cbKey->setCurrentIndex(i);
            break;
        }
    }

    n = md->countChild(obj, md_comaction);
    k = md->count(md->find(mdc_actions), md_action);

    Q_UNUSED(k);

    ananasListViewItem *aitem;
    aCfgItem actionObj;

    for (i = 0; i < n; i++) {
        actionObj = md->find(obj, md_comaction, i);
        if (actionObj.isNull())
            continue;

        id = md->text(actionObj).toLong();
        com_action = md->find(id);

        if (com_action.isNull()) {
            md->remove(actionObj);
            continue;
        }

        aitem = new ananasListViewItem(vComActions, 0, md, com_action);
        aitem->setFlags(aitem->flags() & ~Qt::ItemIsEditable);

        apix = md->findChild(com_action, md_active_picture, 0);
        if (!apix.isNull()) {
            pix.loadFromData(md->binary(apix));
            aitem->setIcon(0, QIcon(pix));
        }
    }
}


void dEditCommand::bAddAction_clicked()
{
    aCfgItem apix;
    QPixmap pix;

    ananasListViewItem *cur =
        static_cast<ananasListViewItem*>(actiontree->currentItem());

    if (!cur)
        return;

    if (md->objClass(cur->obj) != md_action)
        return;

    ananasListViewItem *aitem =
        new ananasListViewItem(vComActions, 0, md, cur->obj);

    apix = md->findChild(md->find(aitem->id), md_active_picture, 0);
    pix.loadFromData(md->binary(apix));

    aitem->setIcon(0, QIcon(pix));
}


void dEditCommand::bRemoveAction_clicked()
{
    QTreeWidgetItem *item = vComActions->currentItem();
    if (!item)
        return;

    delete item;
}


void dEditCommand::bMoveUp_clicked()
{
    QTreeWidgetItem *item = vComActions->currentItem();
    if (!item)
        return;

    int index = vComActions->indexOfTopLevelItem(item);
    if (index > 0) {
        vComActions->takeTopLevelItem(index);
        vComActions->insertTopLevelItem(index - 1, item);
        vComActions->setCurrentItem(item);
    }
}


void dEditCommand::bMoveDown_clicked()
{
    QTreeWidgetItem *item = vComActions->currentItem();
    if (!item)
        return;

    int index = vComActions->indexOfTopLevelItem(item);
    if (index < vComActions->topLevelItemCount() - 1) {
        vComActions->takeTopLevelItem(index);
        vComActions->insertTopLevelItem(index + 1, item);
        vComActions->setCurrentItem(item);
    }
}
