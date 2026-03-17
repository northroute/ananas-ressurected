#include "deditjournal.h"

#include "acfg.h"

/*
 *  Constructs a dEditJournal as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditJournal::dEditJournal(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditJournal::~dEditJournal()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditJournal::languageChange()
{
    retranslateUi(this);
}

void dEditJournal::setData(aListViewItem *o)
{
    item = o;
    md = o->md;
    obj = o->obj;

    aCfgItem cobj, doc, docs;
    int i, j, n;

    aAliasEditor *a = new aAliasEditor(md, obj, tAliases);
    al = a;
    al->setData();

    aRoleEditor *r = new aRoleEditor(md, obj, tRoles, md_journal);
    re = r;
    re->setData();

    setWindowTitle(tr("Journal:") + md->attr(obj, mda_name));
    eName->setText(md->attr(obj, mda_name));
    eDescription->setPlainText(md->sText(obj, md_description));

    i = md->attr(obj, mda_type).toInt();
    cbType->setCurrentIndex(i);

    journalDocs = new ananasTreeView(tabWidget18->widget(2), md);
    journalDocs->setSortingEnabled(false);
    // layout28->addWidget(journalDocs, 0, 0);

    n = md->count(obj, md_used_doc);
    for (j = 0; j < n; j++)
    {
        cobj = md->find(obj, md_used_doc, j);
        if (cobj.isNull())
            continue;

        doc = md->find(md->text(cobj).toLong());
        if (doc.isNull())
            md->remove(cobj);
        else
            new ananasListViewItem(journalDocs, 0, md, doc);
    }

    allDocs = new ananasTreeView(tabWidget18->widget(2), md);
    allDocs->setSortingEnabled(false);
    // layout29->addWidget(allDocs, 0, 0);

    docs = md->find(md->find(mdc_metadata), md_documents, 0);
    n = md->count(docs, md_document);

    for (j = 0; j < n; j++)
    {
        cobj = md->find(docs, md_document, j);
        if (cobj.isNull())
            continue;

        doc = md->find(md->id(cobj));
        new ananasListViewItem(allDocs, 0, md, doc);
    }

    docPage = tabWidget18->widget(2);

    if (cbType->currentIndex() != 1)
        typeChange();
}


void dEditJournal::init()
{
    statusBar()->hide();
}

void dEditJournal::updateMD()
{
    aCfgItem doc, docs;

    al->updateMD();
    re->updateMD();

    item->setText(0, eName->text().trimmed());
    md->setAttr(obj, mda_name, eName->text().trimmed());
    md->setSText(obj, md_description, eDescription->toPlainText());
    md->setAttr(obj, mda_type, QString::number(cbType->currentIndex()));

    docs = md->find(obj, md_columns, 0);

    do
    {
        doc = md->findChild(docs, md_used_doc, 0);
        if (!doc.isNull())
            md->remove(doc);
    } while (!doc.isNull());

    if (cbType->currentIndex() != 1)
        return;

    for (int i = 0; i < journalDocs->topLevelItemCount(); i++)
    {
        ananasListViewItem *aitem =
            static_cast<ananasListViewItem*>(journalDocs->topLevelItem(i));

        if (!aitem)
            continue;

        doc = md->insert(docs, md_used_doc, QString(), -1);
        md->setText(doc, QString("%1").arg(aitem->id));
    }
}

void dEditJournal::closeEditor()
{
    updateMD();
    ((MainForm*)topLevelWidget())->wl->remove(this);
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}


void dEditJournal::typeChange()
{
    if (cbType->currentIndex() != 1) {
        int index = tabWidget18->indexOf(docPage);
        if (index != -1)
            tabWidget18->removeTab(index);
    } else {
        if (tabWidget18->indexOf(docPage) == -1)
            tabWidget18->addTab(docPage, QObject::tr("Documents"));
    }
}


void dEditJournal::addDoc()
{
    ananasListViewItem *cur =
        static_cast<ananasListViewItem*>(allDocs->currentItem());

    if (!cur)
        return;

    if (md->objClass(cur->obj) != md_document)
        return;

    new ananasListViewItem(journalDocs, 0, md, cur->obj);
}


void dEditJournal::removeDoc()
{
    QTreeWidgetItem *item = journalDocs->currentItem();
    if (!item)
        return;

    delete item;
}


void dEditJournal::moveUp()
{
    QTreeWidgetItem *item = journalDocs->currentItem();
    if (!item)
        return;

    int index = journalDocs->indexOfTopLevelItem(item);
    if (index > 0) {
        journalDocs->takeTopLevelItem(index);
        journalDocs->insertTopLevelItem(index - 1, item);
        journalDocs->setCurrentItem(item);
    }
}


void dEditJournal::moveDown()
{
    QTreeWidgetItem *item = journalDocs->currentItem();
    if (!item)
        return;

    int index = journalDocs->indexOfTopLevelItem(item);
    if (index < journalDocs->topLevelItemCount() - 1) {
        journalDocs->takeTopLevelItem(index);
        journalDocs->insertTopLevelItem(index + 1, item);
        journalDocs->setCurrentItem(item);
    }
}
