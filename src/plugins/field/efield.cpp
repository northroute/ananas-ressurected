#include "efield.h"

#include "acfg.h"

/*
 *  Constructs a eField as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
eField::eField(QWidget *parent, Qt::WindowFlags fl) : QDialog(parent, fl)
{
	setObjectName("eDocument");
    setModal(true);

    setupUi(this);
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
eField::~eField()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void eField::languageChange()
{
    retranslateUi(this);
}

void eField::init()
{
}

void eField::setData(QWidget *o, aCfg *cfg)
{
    md = cfg;

    if (!o || !md)
    {
        reject();
        return;
    }

    wField *f = qobject_cast<wField *>(o);
    if (!f)
    {
        reject();
        return;
    }

    QString ts = f->getFieldType();
    QChar t = ' ';
    int w = 0;
    int d = 0;
    int oid = 0;
    int idx = 0;

    otypes.clear();
    eType->clear();

    QStringList tlist = md->types();
    for (QStringList::Iterator it = tlist.begin(); it != tlist.end(); ++it)
    {
        otypes.append((*it).section("\t", 0, 0));
        eType->insertItem(idx++, (*it).section("\t", 1, 1));
    }

    if (!ts.isEmpty())
    {
        QStringList parts = ts.split(' ', QString::SkipEmptyParts);
        if (parts.count() > 0 && !parts[0].isEmpty())
            t = parts[0].at(0);
        if (parts.count() > 1)
            w = parts[1].toInt();
        if (parts.count() > 2)
            d = parts[2].toInt();
    }
    else
    {
        t = 'N';
        w = 10;
    }

    if (t == 'O')
    {
        for (int i = 0; i < otypes.count(); ++i)
        {
            if (!otypes[i].isEmpty() && otypes[i].at(0) == 'O')
            {
                oid = otypes[i].section(' ', 1, 1).toInt();
                if (oid == w)
                {
                    eType->setCurrentIndex(i);
                    break;
                }
            }
        }
    }
    else
    {
        eWidth->setValue(w);
        eDec->setValue(d);

        if (t == ' ') eType->setCurrentIndex(0);
        if (t == 'N') eType->setCurrentIndex(1);
        if (t == 'C') eType->setCurrentIndex(2);
        if (t == 'D') eType->setCurrentIndex(3);
        if (t == 'B') eType->setCurrentIndex(4);
    }

    typeSelect(eType->currentIndex());
}


void eField::getData(QWidget *o)
{
    if (!o)
        return;

    wField *f = qobject_cast<wField *>(o);
    if (!f)
        return;

    wField::tEditorType type = wField::Unknown;
    QString ntype;
    int idx = eType->currentIndex();

    ntype = otypes[idx];
    ntype = ntype.arg(eWidth->value()).arg(eDec->value());
    f->setFieldType(ntype);

    if (!otypes[idx].isEmpty() && otypes[idx].at(0) == 'N')
        type = wField::Numberic;
    else if (!otypes[idx].isEmpty() && otypes[idx].at(0) == 'C')
        type = wField::String;
    else if (!otypes[idx].isEmpty() && otypes[idx].at(0) == 'D')
        type = wField::Date;
    else if (!otypes[idx].isEmpty() && otypes[idx].at(0) == 'B')
        type = wField::Boolean;
    else if (!otypes[idx].isEmpty() && otypes[idx].at(0) == 'O')
    {
        QString tmp = otypes[idx].section(' ', 1, 1);
        long tid = tmp.toLong();
        aCfgItem item = md->find(tid);

        if (!item.isNull())
        {
            tmp = md->objClass(item);

            if (tmp == md_catalogue)
                type = wField::Catalogue;
            else if (tmp == md_document)
                type = wField::Document;
        }
    }

    f->setEditorType(type);
}

void eField::typechanged(const QString &s )
{
}

void eField::typeSelect( int idx )
{
    bool w = false, d = false;
    if ( otypes[ idx ][0] == 'N' ){
	w = true;
	d = true;

    } else
	if ( otypes[ idx ][0] == 'C' ){
	w = true;

    }
    eWidth->setShown( w );
    tWidth->setShown( w );
    eDec->setShown( d );
    tDec->setShown( d );
}
