#include "deditfield.h"

#include "acfg.h"
#include "acfgrc.h"

#include "alog.h"

/*
 *  Constructs a dEditField as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditField::dEditField(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditField::~dEditField()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditField::languageChange()
{
    retranslateUi(this);
}

void setText()
{
}


void dEditField::closeEditor()
{
    updateMD();
    ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}

void dEditField::init()
{
    // eModule->setInterpreter(new QSInterpreter());
    delete statusBar();

    otypes.clear();
    artypes.clear();
    eType->clear();

    otypes.append(" ");
    eType->addItem(tr("Unknown"));
}


void dEditField::setData(aListViewItem *o)
{
    item = o;
    aCfg *md = o->md;
    aCfgItem obj = o->obj;

    QString ts;
    char t = ' ';
    int w = 0, d = 0, oid, idx = 0;
    unsigned int i;

    aAliasEditor *a = new aAliasEditor(md, obj, tAliases);
    al = a;
    al->setData();

    eName->setText(md->attr(obj, mda_name));
    nameChanged();

    ts = md->attr(obj, mda_type);

    if (md->objClass(md->parent(obj)) == md_element)
    {
        saldoTextLabel->show();
        comboBox2->show();
    }
    else
    {
        saldoTextLabel->hide();
        comboBox2->hide();
    }

    eDescription->setPlainText(md->sText(obj, md_description));

    if (md->attr(obj, mda_sort) == "1") efSort->setChecked(true);
    else efSort->setChecked(false);

    if (md->attr(obj, mda_plus) == "1") efPlus->setChecked(true);
    else efPlus->setChecked(false);

    if (md->attr(obj, mda_nz) == "1") efNZ->setChecked(true);
    else efNZ->setChecked(false);

    efSum->setChecked(md->attr(obj, mda_sum) == "1");

    QStringList tlist;
    if (md->objClass(md->parent(obj)) == md_resources)
    {
        tlist.append("\t" + QObject::tr("Unknown"));
        tlist.append("N %d %d\t" + QObject::tr("Numberic"));
    }
    else
    {
        tlist = md->types();
    }

    otypes.clear();
    eType->clear();

    for (QStringList::Iterator it = tlist.begin(); it != tlist.end(); ++it) {
        otypes.append((*it).section("\t", 0, 0));
        eType->addItem((*it).section("\t", 1, 1));
        idx++;
    }

    if (!ts.isEmpty())
    {
        sscanf(ts.toLocal8Bit().constData(), "%c %d %d", &t, &w, &d);
    }
    else
    {
        t = 'N';
        w = 10;
    }

    if (t == 'O') {
        for (i = 0; i < (unsigned int)otypes.count(); i++) {
            if (otypes[i][0] == 'O') {
                sscanf(otypes[i].toLocal8Bit().constData(), "O %d", &oid);
                if (oid == w) {
                    eType->setCurrentIndex(i);
                    break;
                }
            }
        }
    } else {
        if (t == ' ') eType->setCurrentIndex(0);
        if (t == 'N')
        {
            eWidth->setMaximum(20);
            eDec->setMaximum(99);
            eType->setCurrentIndex(1);
        }
        if (t == 'C')
        {
            eWidth->setMaximum(254);
            eDec->setMaximum(99);
            eType->setCurrentIndex(2);
        }
        if (t == 'D') eType->setCurrentIndex(3);
        if (t == 'B') eType->setCurrentIndex(4);

        eWidth->setValue(w);
        eDec->setValue(d);
    }

    aCfgItem obj2;
    int n, n1, aregid, aregfid;
    QString str;
    aCfgItem context = md->findChild(md->find(mdc_metadata), md_registers);

    comboBox2->clear();
    comboBox2->addItem(" ");

    artypes.clear();
    artypes.append(" ");

    n = md->count(context, md_aregister);

    for (i = 0; i < (unsigned int)n; i++)
    {
        obj = md->find(context, md_aregister, i);
        if (!obj.isNull())
        {
            aregid = md->attr(obj, mda_id).toInt();
            str = tr("AccumulationRegister.") + md->attr(obj, mda_name);

            obj = md->findChild(obj, md_resources);
            n1 = md->count(obj, md_field);

            for (uint j = 0; j < (uint)n1; j++)
            {
                obj2 = md->find(obj, md_field, j);
                if (!obj2.isNull())
                {
                    aregfid = md->attr(obj2, mda_id).toInt();
                    artypes.append(QString(" %1 %2").arg(aregid).arg(aregfid));
                    comboBox2->addItem(str + "." + md->attr(obj2, mda_name));

                    if (w == aregid && d == aregfid)
                        comboBox2->setCurrentIndex(comboBox2->count() - 1);
                }
            }
        }
    }

    typeSelect(eType->currentIndex());
}

void dEditField::updateMD()
{
    QString st;

    aCfg *md = item->md;
    aCfgItem obj = item->obj;

    al->updateMD();

    item->setText(0, eName->text().trimmed());
    md->setAttr(obj, mda_name, eName->text().trimmed());
    md->setSText(obj, md_description, eDescription->toPlainText());

    if (md->objClass(md->parent(obj)) == md_resources)
        efSort->setChecked(true);

    if (efSort->isChecked()) md->setAttr(obj, mda_sort, "1");
    else md->setAttr(obj, mda_sort, "0");

    if (efPlus->isChecked()) md->setAttr(obj, mda_plus, "1");
    else md->setAttr(obj, mda_plus, "0");

    if (efNZ->isChecked()) md->setAttr(obj, mda_nz, "1");
    else md->setAttr(obj, mda_nz, "0");

    if (efSum->isChecked())
        md->setAttr(obj, mda_sum, "1");
    else
        md->setAttr(obj, mda_sum, "0");

    if (eType->currentIndex() == 0)
    {
        st = QString(artypes[comboBox2->currentIndex()]);
    }
    else
    {
        st = QString(otypes[eType->currentIndex()]).arg(eWidth->value()).arg(eDec->value());
        printf("type is %s\n", st.toLocal8Bit().constData());
        if (efSort->isChecked())
            st.append(" I");
    }

    md->setAttr(obj, mda_type, st);
}


void dEditField::typeSelect(int idx)
{
    if (otypes[idx][0] == 'N')
    {
        eWidth->setMaximum(20);
        eWidth->show();
        eDec->setMaximum(99);
        eDec->show();
        tWidth->show();
        tDec->show();
        efSum->show();
        comboBox2->setEnabled(false);
    }
    else
    {
        if (otypes[idx][0] == 'C')
        {
            eWidth->setMaximum(254);
            eWidth->show();
            eDec->hide();
            tWidth->show();
            tDec->hide();
            efSum->hide();
            efSum->setChecked(false);
            comboBox2->setEnabled(false);
        }
        else
        {
            if (idx == 0)
            {
                comboBox2->setEnabled(true);
                eWidth->hide();
                eDec->hide();
                tWidth->hide();
                tDec->hide();
                efSum->hide();
                efSum->setChecked(false);
            }
            else
            {
                comboBox2->setEnabled(false);
                eWidth->hide();
                eDec->hide();
                tWidth->hide();
                tDec->hide();
                efSum->hide();
                efSum->setChecked(false);
            }
        }
    }
}

void dEditField::nameChanged()
{
    setWindowTitle(tr("Field:") + eName->text());
}


void dEditField::AARegSelect( int i )
{

}
