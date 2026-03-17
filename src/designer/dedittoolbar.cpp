#include "dedittoolbar.h"

#include "acfg.h"

//extern aCfg cfg;

/*
 *  Constructs a dEditToolbar as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditToolbar::dEditToolbar(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditToolbar::~dEditToolbar()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditToolbar::languageChange()
{
    retranslateUi(this);
}

void dEditToolbar::updateMD()
{
    aCfgItem com_action, apix;
    al->updateMD();
    QPixmap pix;

    item->setText(0, eName->text().trimmed());
    md->setAttr(obj, mda_name, eName->text().trimmed());
    md->setSText(obj, md_description, eDescription->toPlainText());

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

    for (int i = 0; i < vComActions->topLevelItemCount(); i++) {
        ananasListViewItem *aitem =
            static_cast<ananasListViewItem*>(vComActions->topLevelItem(i));

        if (!aitem)
            continue;

        com_action = md->insert(obj, md_comaction, QString(), -1);
        md->setText(com_action, QString("%1").arg(aitem->id));
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

void dEditToolbar::closeEditor()
{
    updateMD();
    ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}

void dEditToolbar::init()
{
    statusBar()->hide();
}

void dEditToolbar::setData(InterfaceListViewItem *o)
{
    int i, j, n, k, id;
    item = o;
    md = o->md;
    obj = o->obj;

    aCfgItem com_action, apix;
    aAliasEditor *a = new aAliasEditor(md, obj, tAliases);
    QPixmap pix;

    vComActions = new ananasTreeView(tabWidget2->widget(1), md);
    vComActions->setSortingEnabled(false);
    // layout28->addWidget(vComActions, 0, 0);

    actiontree = new aActionTreeView(tabWidget2->widget(1), md);
    // layout29->addWidget(actiontree, 0, 0);

    al = a;
    al->setData();

    setWindowTitle(tr("Command:") + md->attr(obj, mda_name));
    eName->setText(md->attr(obj, mda_name));
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

    ananasListViewItem *aitem;
    for (i = 0; i < n; i++)
    {
        id = md->text(md->find(obj, md_comaction, i)).toInt();

        for (j = 0; j < k; j++)
        {
            com_action = md->find(md->find(mdc_actions), md_action, j);
            if (md->id(com_action) == id)
            {
                aitem = new ananasListViewItem(vComActions, 0, md, com_action);
                aitem->setFlags(aitem->flags() & ~Qt::ItemIsEditable);

                apix = md->findChild(md->find(aitem->id), md_active_picture, 0);
                if (apix.isNull())
                    break;

                pix.loadFromData(md->binary(apix));
                aitem->setIcon(0, QIcon(pix));
            }
        }
    }
}

void dEditToolbar::bAddAction_clicked()
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


void dEditToolbar::bRemoveAction_clicked()
{
    QTreeWidgetItem *item = vComActions->currentItem();
    if (!item)
        return;

    delete item;
}


void dEditToolbar::bMoveUp_clicked()
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


void dEditToolbar::bMoveDown_clicked()
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
