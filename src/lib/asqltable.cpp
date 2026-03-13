/****************************************************************************
** $Id: asqltable.cpp,v 1.4 2009/05/25 16:35:10 app Exp $
**
** Code file of the Ananas database table of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
**
** This file is part of the Library of the Ananas
** automation accounting system.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.leaderit.ru/page=ananas or email sales@leaderit.ru
** See http://www.leaderit.ru/gpl/ for GPL licensing information.
**
** Contact org@leaderit.ru if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
//#include <stream.h>

#include "asqltable.h"

#include "adatabase.h"
#include "acatalogue.h"
#include "adocument.h"
#include "aaregister.h"
#include "alog.h"

/*!
 *	\~english
 *	Constructor
 *	\~russian
 *	Создает таблицу доступа к базе данных, позволяющую работать с одной
 *	sql таблицей или одним sql представлением (view).
 *	\param context - объект метаданных, который описывает таблицу.
 *	\param adb - ссылка на объект базы данных, которой
 *	принадлежит sql таблица.
 *	\~
 */
aDataTable::aDataTable(aCfgItem context, aDatabase *adb)
{
    db = adb;
    md = &db->cfg;
    mdobjId = 0;
    tableName = db->tableDbName(db->cfg, context, &mdobjId);

    if (!tableName.isEmpty()) {
        init(context, adb);
    }

    selected = false;
}



/*!
 *	\~english
 *	Constructor
 *	\~russian
 *	Создает таблицу доступа к базе данных, позволяющую работать с одной
 *	sql таблицей или одним sql представлением (view).
 *	\param tname - имя sql таблицы в базе данных
 *	\param adb - ссылка на объект базы данных, которой
 *	принадлежит sql таблица.
 *	\~
 *
 */
aDataTable::aDataTable(const QString &tname, aDatabase *adb)
{
    db = adb;
    md = &db->cfg;
    tableName = tname;
    mdobjId = 0;
    selected = false;
}

/*!
 *	\~english
 *	Destructor
 *	\~russian
 *	Деструктор
 *	\~
 *
 */
aDataTable::~aDataTable()
{
    qDeleteAll(p_reg);
    qDeleteAll(p_cat);
    qDeleteAll(p_doc);

    p_reg.clear();
    p_cat.clear();
    p_doc.clear();
}

/*!
 *	\~english
 *	Inits table object.
 *	\~russian
 *	Инициализирует объект.
 *	\see setObject()
 *	\param context - объект метаданных, который описывает таблицу.
 *	\param adb - ссылка на объект базы данных, которой
 *	принадлежит sql таблица.
 *	\~
 */
void aDataTable::init(aCfgItem context, aDatabase *adb)
{
    db = adb;

    fnames.clear();
    userFilter.clear();

    setObject(context);
}



/*!
 *	\~english
 *	Sets md object to table.
 *	\~russian
 *	Задает объект метаданных для таблицы.
 *	Заполняет внутренние объекты именами полей для последующего использования в
 *	функциях SetValue() и Value(), добавляет информацию о них в sql курсор.
 *	В случае, если добавляется накопительный регистр, к объетку добавляются также виртуальные поля - ресурсы регистра
 *	После вызова этой функции с таблицей можно начинать работу.
 *	\param context - объект метаданных, который описывает таблицу.
 *	\~
 */
void
aDataTable::setObject( aCfgItem context )
{
	aCfgItem cobj, parent;
	parent = obj = context;


	mdobjId = md->id(obj);
	while ( !mdobjId )
	{
		parent = md->parent(parent);
		mdobjId = md->id(parent);
	}
	if ( context.isNull() )
	{
		aLog::print(aLog::Error,QObject::tr("aDataTable try set mdobject to null"));
		return;
	}
	mapCat.clear();
	mapDoc.clear();
	mapReg.clear();
	mapDim.clear();
	mapSum.clear();
	//TODO:  test to memory leak
	p_cat.clear();
	p_reg.clear();
	p_doc.clear();
//	printf("before delete p_cat\n");
//	if(p_cat)
	//delete p_cat;

//	p_cat.clear();
//	printf("after delete p_cat\n");
//	if(p_reg)
	//delete p_reg;
//	p_reg.clear();
	if(md->objClass(context) == md_field && md->objClass(md->parent(context))== md_dimensions)
	{
		insertFieldInfo(context,false);
		aCfgItem res;
		aCfgItem ress = md->findChild(md->parent(md->parent(context)),md_resources);
		uint n = md->count( ress, md_field );
		for ( uint i = 0; i < n; i++ )
		{
			res = md->find( ress, md_field, i );
			insertFieldInfo(res,false);
		}
	}
	uint n = md->count( context, md_field );
	for ( uint i = 0; i < n; i++ )
	{
		cobj = md->find( context, md_field, i );
		insertFieldInfo(cobj);

	}

//	r = *this;
}


/*!
 *	\~english
 *	Appends info about field to object.
 *	\~russian
 *	Добавляет информацию о поле к объекту.
 *	\param obj - объект метаданных, который описывает поле.
 *	\param calculatd - указывает будет ли поле вычисляемым.
 *	\~
 */
void aDataTable::insertFieldInfo(aCfgItem cobj, bool calculated)
{
    QString fname, fdbname, objt;
    int fid;

    if (!cobj.isNull())
    {
        fid = md->id(cobj);
        fname = md->attr(cobj, mda_name);
        objt = md->attr(cobj, mda_type).toUpper();
        fdbname = QString("uf%1").arg(fid);

        if (objt[0] == 'O')
        {
            fnames.insert(fname, fdbname);
            fdbname = QString("text_uf%1").arg(fid);
            // append(QSqlField(fdbname, QVariant::String));
            // setCalculated(fdbname, calculated);

            int ftid = objt.section(" ", 1, 1).toInt();
            aCfgItem fto = md->find(ftid);
            if (!fto.isNull())
            {
                if (md->objClass(fto) == md_catalogue)
                    mapCat[fid] = fto;

                if (md->objClass(fto) == md_document)
                    mapDoc[fid] = fto;
            }
        }
        else if (objt[0] == ' ')
        {
            fdbname = QString("text_uf%1").arg(fid);
            // append(QSqlField(fdbname, QVariant::String));
            // setCalculated(fdbname, calculated);

            fnames.insert(fname, fdbname);

            int ftid = objt.section(" ", 1, 1).toInt();
            aCfgItem fto = md->find(ftid);
            if (!fto.isNull())
            {
                if (md->objClass(fto) == md_aregister)
                {
                    aCfgItem s_field = md->find(objt.section(" ", 2, 2).toInt());
                    aCfgItem dim_fields = md->find(fto, md_dimensions);

                    if (!dim_fields.isNull())
                    {
                        int cnt = md->count(dim_fields, md_field);
                        for (int k = 0; k < cnt; k++)
                        {
                            aCfgItem dim_field = md->find(dim_fields, md_field, k);
                            QString type = md->attr(dim_field, mda_type);

                            if (type[0] == 'O')
                            {
                                if (type.section(" ", 1, 1).toInt() == mdobjId)
                                {
                                    mapReg[fid] = fto;
                                    mapDim[fid] = md->attr(dim_field, mda_name);
                                    mapSum[fid] = md->attr(s_field, mda_name);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            fnames.insert(fname, fdbname);
        }
    }
}


/*!
 *	\~english
 *	Gets metadata object.
 *	\~russian
 *	Получение объекта метаданных, которым инициализировали таблицу.
 *	\return объект метаданных, который описывает таблицу.
 *	\~
 */
qulonglong
aDataTable::getIdd(void)
{
	return 0;
}
long
aDataTable::getMdObjId()
{
	return mdobjId;
}



/*!
 *	\~english
 *	Stub.
 *	\~russian
 *	Заглушка.
 *	\~
 */
void
aDataTable::appendField( aSQLField * ) // fieldinfo )
{

}



/*!
 *	\~english
 *	Stub.
 *	\~russian
 *	Заглушка.
 *	\~
 */
void
aDataTable::insertField( int /*pos*/, aSQLField * ) // fieldinfo )
{

}



/*!
 *	\~english
 *	Stub.
 *	\~russian
 *	Заглушка.
 *	\~
 */
void
aDataTable::removeField( int ) // pos )
{

}



/*!
 *	\~english
 *	Stub.
 *	\~russian
 *	Заглушка.
 *	\~
 */
void
aDataTable::clearFields()
{

}



/*!
 *	\~english
 *	Stub.
 *	\~russian
 *	Заглушка.
 *	\~
 */
bool aDataTable::checkStructure(bool)
{
    bool rc = false;

    if (tableName.isEmpty())
    {
    }
    else
    {
    }

    return rc;
}


/*!
 *	\~english
 *	Return field falue.
 *	\~russian
 *	Возвращает значение поля с номером \a i.
 *	\param i - номер поля.
 *	\return значение поля
 *	\~
 */
QVariant aDataTable::value(int i)
{
    QSqlRecord rec = query.record();
    if (i < 0 || i >= rec.count())
        return QVariant();

    return sysValue(rec.fieldName(i));
}



/*!
 *	\~english
 *	Return field falue.
 *	\~russian
 *	Возвращает значение поля с именем \a name.
 *	Для получения значения необходимо указывать имя поля в метаданных
 *	\param name - имя поля в метаданных.
 *	\return значение поля или QVariant::Invalid, если поля не существует.
 *	\~
 */
QVariant aDataTable::value(const QString &name)
{
    if (!fnames.contains(name))
    {
        aLog::print(aLog::Error,
            QObject::tr("aDataTable get value of unknown field `%1'").arg(name));
        return QVariant();
    }

    QString fname = fnames.value(name);
    return sysValue(fname);
}



/*!
 * Return true if column name exists in database table.
 */
 /*!
 *	\~english
 *	Check field existing.
 *	\~russian
 *	Проверяет существование поля в таблице
 *	\param name - имя поля в таблице (не в метаданных!).
 *	\return \~engish true if column name exists in database table \~russian true, если столбец с данным именем есть в таблице. \~
 *	\~
 */

bool aDataTable::sysFieldExists(const QString &name)
{
    QSqlRecord rec = query.record();
    return rec.indexOf(name) != -1;
}



/*!
 * Return value of the column of the database table.
 */
QVariant aDataTable::sysValue(const QString &name)
{
    int idx = query.record().indexOf(name);
    if (idx != -1)
        return query.value(idx);

    return QVariant();
}


void aDataTable::setSysValue(const QString &name, QVariant value)
{
    if (name == QString("pnum"))
    {
        aLog::print(aLog::Info,
            QObject::tr("aDataTable get document prefix to `%1'").arg(value.toString()));
    }

    QSqlRecord rec = query.record();
    if (rec.indexOf(name) != -1)
        rec.setValue(name, value);
}



/*!
 * Set value of the column of the database table.
 */
void aDataTable::setValue(int i, QVariant value)
{
    QSqlRecord rec = query.record();
    if (i >= 0 && i < rec.count())
        rec.setValue(i, value);
}


bool aDataTable::setValue(const QString &name, QVariant value)
{
    if (!fnames.contains(name))
        return false;

    QString fname = fnames.value(name);
    QSqlRecord rec = query.record();

    if (rec.indexOf(fname) == -1)
        return false;

    rec.setValue(fname, value);
    return true;
}


QSqlRecord *aDataTable::primeInsert()
{
    QSqlRecord *rec = new QSqlRecord(query.record());

    if (sysFieldExists("id"))
        rec->setValue("id", QVariant(db->uid(mdobjId)));

    return rec;
}


/*!
 *
 *//*
QSqlRecord *
aDataTable::primeUpdate()
{
	QSqlRecord *rec;

	rec = QSqlCursor::primeUpdate();
	*rec = r;
	return rec;
}
*/


bool aDataTable::select(const QString &filter, bool usefltr)
{
    QString flt = getFilter();

    if (usefltr)
    {
        if (flt.isEmpty())
            flt = filter;
        else if (!filter.isEmpty())
            flt = flt + " AND " + filter;
    }
    else
    {
        flt = filter;
    }

    QString sql = QString("SELECT * FROM %1").arg(tableName);
    if (!flt.isEmpty())
        sql += " WHERE " + flt;

    query = QSqlQuery(*db->db());
    bool res = query.exec(sql);

    if (!res)
    {
        QSqlError err = query.lastError();
        aLog::print(aLog::Error,
            QString("%1 %2").arg(err.text()).arg(err.driverText()));
    }

    return res;
}


bool aDataTable::select(qulonglong id)
{
    if (!db || !db->db())
        return false;

    QString sql = QString("SELECT * FROM %1 WHERE id=%2")
                  .arg(tableName)
                  .arg(id);

    query = QSqlQuery(*db->db());
    bool res = query.exec(sql);

    if (!res)
    {
        QSqlError err = query.lastError();
        aLog::print(aLog::Error,
            QString("%1 %2").arg(err.text()).arg(err.driverText()));
    }

    return res;
}


/*!
 *
 */
void
aDataTable::clearFilter()
{
	aLog::print(aLog::Debug, QObject::tr("aDataTable clear filter"));
	userFilter.clear();
}


/*QDict<QVariant>
aDataTable::getUserFilter() {
	return this.userFilter;
};

void
setUserFilter( QDict<QVariant> newFilter) {
	this.userFilter = newFilter;
};
*/


void aDataTable::setFilter(const QString &newFilter)
{
    currentFilter = newFilter;
    userFilter.clear();

    Q_UNUSED(newFilter);
}


bool aDataTable::setFilter(const QString& name, const QVariant& value)
{
    aLog::print(aLog::Debug,
        QObject::tr("aDataTable set filter %1='%2'").arg(name).arg(value.toString()));

    if (!fnames.contains(name))
    {
        aLog::print(aLog::Error,
            QObject::tr("aDataTable set filter %1='%2', %3 not exist")
            .arg(name).arg(value.toString()).arg(name));
        return false;
    }

    userFilter.insert(fnames.value(name), value);
    return true;
}



QString aDataTable::getFilter()
{
    QString filter = "", fid, type;
    aCfgItem field;

    QMap<QString, QVariant>::const_iterator it = userFilter.begin();

    if (it != userFilter.end())
    {
        fid = it.key().mid(2);
        field = md->find(fid.toLong());
        if (!field.isNull())
        {
            type = md->attr(field, mda_type);
            if (type[0] == 'N' || type[0] == 'O')
                filter = it.key() + "=" + it.value().toString();
            else
                filter = it.key() + "='" + it.value().toString() + "'";
        }
        ++it;
    }

    for (; it != userFilter.end(); ++it)
    {
        fid = it.key().mid(2);
        field = md->find(fid.toLong());
        if (!field.isNull())
        {
            type = md->attr(field, mda_type);
            if (type[0] == 'N' || type[0] == 'O')
                filter += " and " + it.key() + "=" + it.value().toString();
            else
                filter += " and " + it.key() + "='" + it.value().toString() + "'";
        }
    }

    return filter;
}


QString aDataTable::getNFilter()
{
    QString filter = "", fid, type;
    aCfgItem field;
    QMap<QString, QVariant>::const_iterator it = userFilter.begin();

    if (it != userFilter.end())
    {
        fid = it.key().mid(2);
        field = md->find(fid.toLong());
        if (!field.isNull())
        {
            type = md->attr(field, mda_type);
            if (type[0] == 'N' || type[0] == 'O')
                filter = tableName + "." + it.key() + "=" + it.value().toString();
            else
                filter = tableName + "." + it.key() + "='" + it.value().toString() + "'";
        }
        ++it;
    }

    for (; it != userFilter.end(); ++it)
    {
        fid = it.key().mid(2);
        field = md->find(fid.toLong());
        if (!field.isNull())
        {
            type = md->attr(field, mda_type);
            if (type[0] == 'N' || type[0] == 'O')
                filter += " and " + tableName + "." + it.key() + "=" + it.value().toString();
            else
                filter += " and " + tableName + "." + it.key() + "='" + it.value().toString() + "'";
        }
    }

    return filter;
}


void aDataTable::printRecord()
{
    QSqlRecord rec = query.record();

    for (int i = 0; i < rec.count(); i++)
    {
        QString fname = "";
        QString sname = rec.fieldName(i);

        QMap<QString, QString>::const_iterator it;
        for (it = fnames.begin(); it != fnames.end(); ++it)
        {
            if (it.value() == sname)
            {
                fname = it.key();
                break;
            }
        }

        printf("%s(%s)=%s\n",
            fname.toLocal8Bit().constData(),
            sname.toLocal8Bit().constData(),
            value(i).toString().toLocal8Bit().constData());
    }
}


bool aDataTable::exec(QString q)
{
    query = QSqlQuery(*db->db());
    bool res = query.exec(q);

    if (!res)
    {
        QSqlError err = query.lastError();
        aLog::print(aLog::Error, QString("%1 %2").arg(err.text()).arg(err.driverText()));
    }

    return res;
}



QVariant
aDataTable::calc_rem(int fid, qulonglong id)
{
	aCfgItem o,fto;
	QString t,oclass;
	int ftid,oid;
	QVariant v="";
	fto = mapReg[fid];// = md->find( ftid );
	if ( !fto.isNull() )
	{
		if(p_reg[QString("%1").arg(fid)]==0)
		{
			p_reg.insert(QString("%1").arg(fid), new aARegister( fto, db ));
		}
		v = ((aARegister*)p_reg[QString("%1").arg(fid)])->getSaldo( QDateTime::currentDateTime(),
					mapDim[fid],
					id,
					mapSum[fid]);//md->attr(s_field,mda_name));
		if(!v.isValid()) v = "";
	}
	return v;
}


QVariant
aDataTable::calc_obj(int fid, qulonglong idd)
{
	aCfgItem o,fto;
	QString t,oclass;
	int ftid;
	QVariant v="";
//	printf("calculate cat %d, %llu\n",fid, idd);
	o = mapCat[fid];
	if( !o.isNull() )
	{
		aCatalogue *pCat = (aCatalogue*)p_cat[QString("%1").arg(fid)];
//		printf("obj not null\n");
		if(pCat==0)
		{
			p_cat.insert(QString("%1").arg(fid), new aCatalogue( o, db ));
			pCat = (aCatalogue*)p_cat[QString("%1").arg(fid)];
		}
		pCat->select( idd );

		if ( pCat->selected() )
		{
//			printf("select ok\n");
			v = QVariant( pCat->displayString() );
		}
	}
	else
	{
		o = mapDoc[fid];
		if(!o.isNull())
		{
			aDocument *pDoc = (aDocument*)p_doc[QString("%1").arg(fid)];
//			printf("obj not null\n");
			if(pDoc==0)
			{
				p_doc.insert(QString("%1").arg(fid), new aDocument( o, db ));
				pDoc = (aDocument*)p_doc[QString("%1").arg(fid)];
			}
			pDoc->select( idd );
			if ( pDoc->selected() )
			{
				v = QVariant( pDoc->displayString() );
			}
		}
	}

	return v;
}

/*!
 * Calculate value object type fields.
 */
QVariant
aDataTable::calculateField( const QString &name )
{
	QVariant v;
	if(name=="system_icon") return v;
	else return QVariant("");
//	return QVariant::Invalid;
}

/*!
 * Calculate value object type fields.
 */
QVariant
aDataTable::calcFieldValue( const QString &name )
{

        QVariant v="", fv=0;
	int fid = 0; //, ftid;
	qulonglong id = 0;
	aCfgItem o, fto;
	QString t, oclass;
        if ( name.left(5)=="text_" )
	{
                if(sysFieldExists(name.mid(5)))
		{
			fv = sysValue( name.mid( 5 ) );
			id = fv.toULongLong();
		}
		else
		{
			id = 0;
		}
		fid = name.mid( 7 ).toInt();
		v = QVariant("");
		if ( fid && id )
		{
			v=calc_obj(fid,id);
		}
		else
		{
			if(fid)
			{
				id = sysValue("id").toULongLong();
				v=calc_rem(fid,id);
			}
		}
        }
//	printf("calculate field %s\n", name.ascii());
        return v;
}



/*!
 * Gets list of user and calculation fields.
 */
QStringList aDataTable::getUserFields()
{
    QStringList lst;
    const QString text_uf = "text_uf";
    QSqlRecord rec = query.record();

    for (int i = 0; i < rec.count(); i++)
    {
        QString fname = rec.fieldName(i);

        if (fname.left(2) == "uf" || fname.left(text_uf.length()) == text_uf)
        {
            if (fname.left(text_uf.length()) == text_uf)
            {
                int idx = lst.indexOf("uf" + fname.mid(text_uf.length()));
                if (idx != -1)
                {
                    lst.removeAt(idx);
                }
            }
            lst << fname;
        }
    }
    return lst;
}



/*!
 *
 */
bool
aDataTable::isMarkDeleted()
{
        if ( sysFieldExists( "df" ) ) return sysValue( "df" ).toInt() == 1;
        return false;
}



/*!
 *
 */
ERR_Code
aDataTable::setMarkDeleted( bool Deleted )
{
        if ( sysFieldExists( "df" ) ) {
                QString v = "";
                if ( Deleted ) v = "1";
                setSysValue( "df", QVariant( v ) );
                return err_noerror;
        }
        return err_incorrecttype; // Object can not be mark deleted
}


bool aDataTable::seek(int i, bool relative)
{
    if (relative)
        return query.seek(query.at() + i);
    return query.seek(i);
}

bool aDataTable::next()
{
    return query.next();
}

bool aDataTable::prev()
{
    return query.previous();
}

bool aDataTable::first()
{
    return query.first();
}

bool aDataTable::last()
{
    return query.last();
}


bool aDataTable::New()
{
    qulonglong Uid = 0;
    bool res = false;

    if (tableName.isEmpty() || !db || !db->db())
        return false;

    if (sysFieldExists("id"))
    {
        Uid = db->uid(mdobjId);
        aLog::print(aLog::Debug,
                    QString("aDataTable new record with id=%1 for meta object with id=%2")
                    .arg(Uid).arg(mdobjId));
    }

    QString sql;
    if (sysFieldExists("id"))
        sql = QString("INSERT INTO %1 (id) VALUES (%2)").arg(tableName).arg(Uid);
    else
        sql = QString("INSERT INTO %1 DEFAULT VALUES").arg(tableName);

    QSqlQuery q(*db->db());
    if (!q.exec(sql))
    {
        QSqlError err = q.lastError();
        aLog::print(aLog::Error, QString("%1 %2").arg(err.text()).arg(err.driverText()));
        return false;
    }

    if (sysFieldExists("id"))
    {
        if (select(QString("id=%1").arg(Uid), false))
        {
            if (first())
                res = true;
            else
                aLog::print(aLog::Error, QString("aDataTable record with id=%1 not found").arg(Uid));
        }
    }
    else
    {
        res = true;
    }

    return res;
}


bool aDataTable::Copy()
{
    QSqlRecord rec = query.record();

    if (rec.isEmpty())
        return false;

    if (New())
    {
        for (int i = 0; i < rec.count(); i++)
        {
            if (rec.field(i).name() != QString("id"))
            {
                setValue(i, rec.value(i));
            }
        }
        return Update();
    }

    return false;
}


bool aDataTable::Delete()
{
    return true;
}


bool aDataTable::Update()
{
    QSqlError err = query.lastError();
    if (err.type() != QSqlError::NoError)
    {
        aLog::print(aLog::Error, QString("%1 %2").arg(err.text()).arg(err.driverText()));
    }
    return true;
}

QString aDataTable::sqlFieldName(const QString &userFieldName) const
{
    return fnames.value(userFieldName, QString(""));
}

QSqlError aDataTable::lastError() const
{
    return query.lastError();
}

int aDataTable::position(const QString &name) const
{
    return query.record().indexOf(name);
}

QString aDataTable::filter() const
{
    return currentFilter;
}
