#include "edbtable.h"

#include "wdbtable.h"


/*
 *  Constructs a eDBTable as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
eDBTable::eDBTable(QWidget *parent, Qt::WindowFlags fl) : QDialog(parent, fl)
{
    setObjectName("eDBTable");

    setupUi(this);

    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
eDBTable::~eDBTable()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void eDBTable::languageChange()
{
    retranslateUi(this);
}

void eDBTable::init()
{
}


void
eDBTable::destroy()
{
//	tablerow_free( r );
}


/*
void eDBTable::insertColumn(const char *Header, const char *Name,
int idxt, int cw, int tw, int td)
{

tablefield *f;
char s[30];

sprintf(s, "%s %i %i", (const char *)otypes[idxt], tw, td);

f = tablefield_newsys(Name, s, "");
tablefield_setheader(f, Header);
f->sizex = cw;
f->flen = tw;
f->decimals = td;
f->ftypeindex = idxt;
tablerow_insertcolumn(r, f, -1);

//	blockSignals( true );
ListCol->insertItem(trUtf8(Header));
//	eColHeader->setText( trUtf8(Header));
//	eColName->setText(trUtf8(Name));
//	eColWidth->setValue(cw);
//	eColTWidth->setValue(tw);
//	eColTDec->setValue(td);
//	blockSignals( false );
if (ListCol->count()) ListCol->setCurrentItem(ListCol->count()-1);

}
*/

void eDBTable::ColumnDel()
{
    int idx = ListCol->currentRow();
    if (idx == -1 || ListCol->count() == 0)
        return;

    QString str = fname[idx];

    deletedFields.append(str);
    deletedFieldsId.append(idlist[idx]);

    idlist.removeAt(idx);
    fname.removeAt(idx);
    cwidth.removeAt(idx);

    delete ListCol->takeItem(idx);
}

void eDBTable::ColumnL()
{
    int idx = ListCol->currentRow();
    if (idx <= 0) return;

    fname.move(idx, idx - 1);
    idlist.move(idx, idx - 1);
    cwidth.move(idx, idx - 1);

    QListWidgetItem *item = ListCol->takeItem(idx);
    ListCol->insertItem(idx - 1, item);
    ListCol->setCurrentRow(idx - 1);
}

void eDBTable::ColumnR()
{
    int idx = ListCol->currentRow();
    if (idx == -1) return;

    if (idx < ListCol->count() - 1)
    {
        fname.move(idx, idx + 1);
        idlist.move(idx, idx + 1);
        cwidth.move(idx, idx + 1);

        QListWidgetItem *item = ListCol->takeItem(idx);
        ListCol->insertItem(idx + 1, item);
        ListCol->setCurrentRow(idx + 1);
    }
}

void eDBTable::ColumnSel(int col)
{
    QString str, s;

    eColHeader->blockSignals(true);
    eColWidth->blockSignals(true);

    eType->clear();
    eColTWidth->setValue(0);
    eColTDec->setValue(0);
    eColHeader->clear();
    eColWidth->setValue(0);
    eColName->clear();

    if (col == -1 || ListCol->count() == 0)
        return;

    int ind = ListCol->currentRow();

    eColHeader->setText(ListCol->item(ind)->text());
    eColHeader->blockSignals(false);

    str = cwidth[ind];
    eColWidth->setValue(str.toInt());
    eColWidth->blockSignals(false);

    str = fname[ind];
    eColName->setText(str);

    str = table->getFieldType(idlist[ind].toInt());

    // parse string with type, num, numdec
    if (!str.isEmpty())
    {
        eType->setText(str.left(1));
        str.remove(0, 2);

        if (!str.isEmpty())
            eColTWidth->setValue(str.toInt());

        int pos = str.indexOf(' ');   // decimal part

        if (pos != -1)
        {
            s = str.mid(pos + 1);
        }
        else
        {
            s = "0";
        }

        eColTDec->setValue(s.toInt());
    }
}

void eDBTable::ColumnUpd()
{
    QStringList lst;

    int id = table->getTableId(ComboBoxTable->currentIndex() - 1);
    QList<int> listTableId = table->getBindList();

    if (listTableId.contains(id))
    {
        cfg_message(0, tr("table in use, please select another table").toUtf8().constData());

        ListCol->clear();
        ListCol->addItem("table in use!");
        ListCol->setEnabled(false);
        bOK->setEnabled(false);

        ColumnSel(-1);
        return;
    }

    ListCol->setEnabled(true);
    bOK->setEnabled(true);

    lst = table->getFields(id);

    ListCol->clear();

    fname  = lst;
    idlist = table->getFields(id, true);

    ListCol->addItems(lst);

    cwidth.clear();
    QString defWidth = QString::number(table->getDefColWidth());

    for (int i = 0; i < lst.count(); ++i)
        cwidth << defWidth;

    findDeletedFields(id, idlist);
}

void eDBTable::getData(wDBTable *t)
{
    QStringList lst;

    if (ComboBoxTable->count())
    {
        t->setTblInd(t->getTableId(ComboBoxTable->currentIndex() - 1));

        // очищаем таблицу
        t->clear();
        t->setColumnCount(0);

        for (int j = 0; j < ListCol->count(); ++j)
        {
            QString header = ListCol->item(j)->text();
            lst << header;

            int col = t->columnCount();
            t->insertColumn(col);

            QTableWidgetItem *item = new QTableWidgetItem(header);
            t->setHorizontalHeaderItem(col, item);

            t->setColumnWidth(col, cwidth[j].toInt());
        }

        t->setDefHeaders(lst);
        t->setDefFields(fname);
        t->setDefIdList(idlist);
        t->setColWidth(cwidth);
    }
    else
    {
        t->setTblInd(-1);
    }
}


void eDBTable::setData(wDBTable *t, aCfg *md)
{
    QStringList lst;
    int idTable;
    bool ok;

    table = t;

    ComboBoxTable->insertItems(1, table->list_available_tables);

    idTable = table->property("TableInd").toInt(&ok);

    ComboBoxTable->setCurrentIndex(1 + t->getTableInd(idTable));

    if (ok && idTable > -1)
    {
        lst = table->property("DefHeaders").toStringList();
        ListCol->insertItems(0, lst);

        cwidth = table->property("ColWidth").toStringList();
        fname  = table->property("DefFields").toStringList();
        idlist = table->property("DefIdList").toStringList();

        findDeletedFields(idTable, idlist);
    }
    else
    {
        ColumnUpd();
    }

    ListCol->setFocus();
}


void eDBTable::ColumnTextUpd(const QString &s)
{
    CHECK_POINT

    int idx = ListCol->currentRow();
    if (idx == -1) return;

    ListCol->blockSignals(true);
    ListCol->item(idx)->setText(s);
    ListCol->blockSignals(false);
}


void eDBTable::doOK()
{
	getData(table);
	accept();
}


void eDBTable::ColumnAdd()
{
    QString s;
    QString idx;

    addfdialog e(this->topLevelWidget(), 0);
    e.setData(deletedFields, deletedFields, deletedFieldsId);

    if (e.exec() == QDialog::Accepted)
    {
        idx = e.getData(false);

        if (idx == "0")
            return;

        s = table->getFieldName(idx.toInt());

        idlist << idx;

        ListCol->insertItem(ListCol->count(), s);

        fname << s;
        cwidth << table->property("DefaultColWidth").toString();

        deletedFields.removeAll(s);
        deletedFieldsId.removeAll(idx);
    }
}

void eDBTable::ColWidthChange(int value)
{
    int ind = ListCol->currentRow();

    if (ind != -1)
    {
        cwidth[ind] = QString::number(value);
    }
}

QStringList eDBTable::findDeletedFields(int t_num, const QStringList &idls)
{
    QStringList lst_full;
    deletedFields.clear();
    deletedFieldsId.clear();

    // формируем список удалённых полей
    lst_full = table->getFields(t_num, true);

    for (int i = 0; i < lst_full.count(); ++i)
    {
        const QString &fieldId = lst_full.at(i);

        if (!idls.contains(fieldId))
        {
            deletedFieldsId << fieldId;
            deletedFields << table->getFieldName(fieldId.toInt());
        }
    }

    return deletedFields;
}


void eDBTable::doCancel()
{
}

