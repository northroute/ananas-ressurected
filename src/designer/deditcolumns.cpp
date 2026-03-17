#include "deditcolumns.h"

#include "acfg.h"

/*
 *  Constructs a dEditColumns as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditColumns::dEditColumns(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditColumns::~dEditColumns()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditColumns::languageChange()
{
    retranslateUi(this);
}

void dEditColumns::setData(ananasListViewItem *o)
{
    item = o;
    md = o->md;
    obj = o->obj;

    usedFields = new ananasTreeView(gbFields, md);
    QGridLayout *l = new QGridLayout(gbFields);
    l->addWidget(usedFields, 0, 0);

    availableFields = new ananasTreeView(frmFields, md);
    l = new QGridLayout(frmFields);
    l->addWidget(availableFields, 0, 0);

    aCfgItem cobj, fobj, dobj, pobj, tobj;

    uint n, fCount, tCount, addfields, addtables;
    long id;

    cobj = md->parent(md->parent(obj));

    setWindowTitle(tr("Column:") + md->attr(cobj, mda_name) + "." + md->attr(obj, mda_name));
    eName->setText(md->attr(obj, mda_name));

    ananasListViewItem *fitem, *ditem, *titem, *tparent, *docs;

    QStringList docsId = md->getJournalDocuments(cobj);

    docs = new ananasListViewItem(
        availableFields,
        0,
        md,
        md->find(md->find(mdc_metadata), md_documents, 0),
        QObject::tr("Documents"));

    docs->setIcon(0, QIcon(rcIcon("doc_g.png")));
    docs->setExpanded(true);

    n = docsId.count();

    for (uint i = 0; i < n; i++)
    {
        id = docsId[i].toInt();
        dobj = md->find(id);
        if (dobj.isNull())
            continue;

        ditem = new ananasListViewItem(docs, 0, md, dobj);
        ditem->setIcon(0, QIcon(rcIcon("doc.png")));

        tobj = md->find(dobj, md_header, 0);
        tparent = new ananasListViewItem(ditem, 0, md, tobj, QObject::tr("Header"));
        tparent->setIcon(0, QIcon(rcIcon("doc_h.png")));

        fCount = md->count(tobj, md_field);

        for (uint k = 0; k < fCount; k++)
        {
            fobj = md->find(tobj, md_field, k);
            if (fobj.isNull())
                continue;

            fitem = new ananasListViewItem(tparent, 0, md, fobj);
            fitem->setIcon(0, QIcon(rcIcon("field.png")));
        }

        pobj = md->find(ditem->obj, md_tables, 0);

        if (!pobj.isNull())
        {
            tparent = new ananasListViewItem(ditem, 0, md, pobj, QObject::tr("Tables"));
            tparent->setIcon(0, QIcon(rcIcon("table_g.png")));

            addtables = 0;
            tCount = md->count(pobj, md_table);

            for (uint j = 0; j < tCount; j++)
            {
                tobj = md->find(pobj, md_table, j);
                if (tobj.isNull())
                    continue;

                titem = new ananasListViewItem(tparent, 0, md, tobj);
                titem->setIcon(0, QIcon(rcIcon("table.png")));

                fCount = md->count(tobj, md_field);
                addfields = 0;

                for (uint k = 0; k < fCount; k++)
                {
                    fobj = md->find(tobj, md_field, k);
                    if (fobj.isNull())
                        continue;

                    QString sum = md->attr(fobj, mda_sum);
                    if (sum != "1")
                        continue;

                    addfields++;

                    fitem = new ananasListViewItem(titem, 0, md, fobj);
                    fitem->setIcon(0, QIcon(rcIcon("field.png")));
                }

                if (!addfields)
                    delete titem;
                else
                    addtables++;
            }

            if (!addtables)
                delete tparent;
        }
    }

    fieldsList.clear();

    n = md->count(obj, md_fieldid);

    for (uint i = 0; i < n; i++)
    {
        fobj = md->find(obj, md_fieldid, i);
        if (fobj.isNull())
            continue;

        id = md->text(fobj).toLong();
        aCfgItem testitem = md->find(id);

        if (testitem.isNull())
            md->remove(fobj);
        else
            addField(testitem);
    }
}

void dEditColumns::updateMD()
{
    aCfgItem field;

    item->setText(0, eName->text().trimmed());
    md->setAttr(obj, mda_name, eName->text().trimmed());

    do {
        field = md->findChild(obj, md_fieldid, 0);
        if (!field.isNull())
            md->remove(field);
    } while (!field.isNull());

    QTreeWidgetItem *it = usedFields->topLevelItem(0);
    while (it) {
        ananasListViewItem *aitem = static_cast<ananasListViewItem*>(it);

        field = md->insert(obj, md_fieldid, QString(), -1);
        md->setText(field, QString("%1").arg(aitem->id));

        int index = usedFields->indexOfTopLevelItem(it);
        it = usedFields->topLevelItem(index + 1);
    }
}


void dEditColumns::closeEditor()
{
    updateMD();
    ((MainForm*)topLevelWidget())->wl->remove(this);
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}


void dEditColumns::init()
{
    if (statusBar())
        statusBar()->hide();
}


void dEditColumns::bAddAction_clicked()
{
    ananasListViewItem *cur =
        static_cast<ananasListViewItem*>(availableFields->currentItem());

    if (!cur)
        return;

    if (md->objClass(cur->obj) != md_field)
        return;

    eName->setText(md->attr(cur->obj, mda_name));
    addField(cur->obj);
}


void dEditColumns::bRemoveAction_clicked()
{
    ananasListViewItem *rmItem =
        static_cast<ananasListViewItem*>(usedFields->currentItem());

    if (!rmItem)
        return;

    aCfgItem cobj = md->find(md->id(rmItem->obj));
    aCfgItem doc, parent = md->parent(cobj);

    if (md->objClass(parent) == md_header)
        doc = md->parent(parent);
    else
        doc = md->parent(md->parent(parent));

    fieldsList.removeAll(md->attr(doc, mda_id));

    delete rmItem;
}


void dEditColumns::bMoveUp_clicked()
{
    QTreeWidgetItem *item = usedFields->currentItem();
    if (!item)
        return;

    int index = usedFields->indexOfTopLevelItem(item);
    if (index > 0) {
        usedFields->takeTopLevelItem(index);
        usedFields->insertTopLevelItem(index - 1, item);
        usedFields->setCurrentItem(item);
    }
}


void dEditColumns::bMoveDown_clicked()
{
    QTreeWidgetItem *item = usedFields->currentItem();
    if (!item)
        return;

    int index = usedFields->indexOfTopLevelItem(item);
    if (index < usedFields->topLevelItemCount() - 1) {
        usedFields->takeTopLevelItem(index);
        usedFields->insertTopLevelItem(index + 1, item);
        usedFields->setCurrentItem(item);
    }
}

void dEditColumns::addField(aCfgItem addItem)
{
    aCfgItem fparent, parent;
    fparent = md->parent(addItem);

    QString fName;

    if (md->objClass(fparent) == md_header) {
        fName = QObject::tr("header");
        parent = md->parent(fparent);
    } else {
        fName = md->attr(fparent, mda_name);
        parent = md->parent(md->parent(fparent));
    }

    QString fId = md->attr(parent, mda_id);

    if (fieldsList.indexOf(fId) != -1)
        return;

    fName = md->attr(parent, mda_name) + "." + fName + "." + md->attr(addItem, mda_name);

    ananasListViewItem *aitem =
        new ananasListViewItem(usedFields, 0, md, addItem, fName);

    aitem->setIcon(0, QIcon(rcIcon("field.png")));
    aitem->setFlags(aitem->flags() & ~Qt::ItemIsEditable);

    fieldsList.append(fId);
}
