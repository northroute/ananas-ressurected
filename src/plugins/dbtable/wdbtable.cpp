/****************************************************************************
** $Id: wdbtable.cpp,v 1.2 2008/11/08 20:16:36 leader Exp $
**
** Code file of the database table plugin of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
** Copyright (C) 2003-2006 Grigory Panov, <gr1313 at mail.ru>, Yoshkar-Ola.
** Copyright (C) 2003-2004 Andrey Paskal <app at lrn dot ru>, Yoshkar-Ola.
**
** This file is part of the Ananas Plugins of the Ananas
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

//--#include <qfocusdata.h>
//--#include "propertyeditor.h"
//--#include "command.h"
//--#include "formwindow.h"
//--#include "mainwindow.h"
//--#include "mainform.h"
#include "adocjournal.h"
#include "alog.h"

#include "wdbtable.h"
#include "edbtable.h"


/*!
 * \en	Constructor. \_en
 * \ru	Конструктор.
 * 	Задает значение ширины по умолчанию для столбца = 100. \_ru
 */
wDBTable::wDBTable(QString objtype, QWidget *parent, const char *name)
    : QTableWidget(parent)
{
    Q_UNUSED(objtype);

    if (name)
        setObjectName(name);

    defColWidth = 100;
    tableInd = -1;
    inEditMode = false;
    searchWidget = 0;
    searchMode = false;
    searchString = "";
    cur = 0;
    engine = 0;
    db = 0;
    md = 0;
    oid = 0;
    doc_id = 0;
    cat_group_id = 0;
    lastEditedRow = -1;
    lastEditedCol = -1;

    verticalHeader()->hide();
    setCornerButtonEnabled(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    setAlternatingRowColors(true);

    connect(this, SIGNAL(cellChanged(int, int)),
            this, SLOT(updateTableCellHandler(int, int)));
    connect(this, SIGNAL(currentCellChanged(int, int, int, int)),
            this, SLOT(lineChange(int, int)));
    connect(this, SIGNAL(cellDoubleClicked(int, int)),
            this, SLOT(onCellDoubleClicked(int, int)));

    init();

    aLog::print(aLog::Debug, tr("wDBTable init ok"));
}

void wDBTable::onCellDoubleClicked(int row, int col)
{
    Q_UNUSED(row);
    Q_UNUSED(col);
    doubleClickEventHandler(0, 0, 0, QPoint());
}

/*!
 * \en	Opens property editor. \_en
 * \ru	Открывает окно редактора свойсв, если fn == true \_ru
 */
void
wDBTable::setOpenEditor( bool fn )
{
	if(fn)	OpenEditor();
}


/*!
 *	\~english
 *	Gets list of defined fields.
 *	\~russian
 *	Получение списка заданных полей.
 *	\~
 */
QStringList
wDBTable::getDefFields() const
{
	return fname;
}


/*!
 *	\~english
 *	Gets list of defined headers.
 *	\~russian
 *	Получение списка заданных заголовков.
 *	\~
 */
QStringList
wDBTable::getDefHeaders() const
{
	return hname;
}


/*!
 *	\~english
 *	Gets list of defined column width.
 *	\~russian
 *	Получение списка заданных размеров столбцов.
 *	\~
 */
QStringList
wDBTable::getColWidth() const
{
	return colWidth;
}


/*!
 *	\~english
 *	Gets list of defined column id.
 *	\~russian
 *	Получение списка заданных идентификаторов столбцов.
 *	\~
 */
QStringList
wDBTable::getDefIdList() const
{
	return idList;
}

/*!
 *	\~english
 *	Gets metadata document id.
 *	\~russian
 *	Получение идентификатора документа метаданных, которому принадлежит виджет.
 *	\~
 */
qulonglong
wDBTable::getId() const
{
	return doc_id;
}

/*!
 *	\~english
 *	Gets current table id.
 *	\~russian
 *	Получение идентификатора текущей таблицы.
 *	\~
 */
int
wDBTable::getTblInd() const
{
	return tableInd;
}

/*!
 *	\~english
 *	Olways return false.
 *	\~russian
 *	Всегда возвращает false.
 *	\~
 *	\return false
 */
bool
wDBTable::getOpenEditor() const
{
	return false;
}

/*!
 *	\~english
 *	Gets default column width.
 *	\~russian
 *	Получение размера столбца по умолчанию.
 *	\~
 */
int
wDBTable::getDefColWidth() const
{
	return defColWidth;
}

/*!
 *	\~english
 *	Sets list of field names.
 *	\~russian
 *	Установка списка имен полей.
 *	\~
 */
void
wDBTable::setDefFields(QStringList lst)
{
	fname = lst;
}

/*!
 *	\~english
 *	Sets list of field headers.
 *	\~russian
 *	Установка списка заголовков полей.
 *	\~
 */
void
wDBTable::setDefHeaders(QStringList lst)
{
	hname = lst;
}

/*!
 *	\~english
 *	Sets list of columns width.
 *	\~russian
 *	Установка списка размеров столбцов.
 *	\~
 */
void
wDBTable::setColWidth(QStringList lst)
{
	colWidth = lst;
}

/*!
 *	\~english
 *	Gets list of column id.
 *	\~russian
 *	Установка списка идентификаторов столбцов.
 *	\~
 */
void
wDBTable::setDefIdList(QStringList lst)
{
	idList = lst;
}

/*!
 *	\~english
 *	Sets metadata document id.
 *	\~russian
 *	Установка идентификатора документа метаданных, которому принадлежит виджет.
 *	\~
 */
void
wDBTable::setId( qulonglong fn )
{
	doc_id = fn;
}


/*!
 *	\~english
 *	Gets current table id.
 *	\~russian
 *	Установка идентификатора текущей таблицы.
 *	\~
 */
void
wDBTable::setTblInd( int ind )
{
	tableInd = ind;
}


/*!
 *	\~english
 *	Sets default columns width.
 *	\~russian
 *	Установка размера столбцов по умолчанию.
 *	\~
 */
void
wDBTable::setDefColWidth( int fn )
{
	defColWidth = fn;
}



/*!
 * \en	Creates property editor dialog window. \_en
 * \ru	Создает диалоговое окно редактора свойств. \_ru
 */
void wDBTable::OpenEditor()
{
    setAvailableTables();

    eDBTable e(this->topLevelWidget(), 0);
    checkFields();
    e.setData(this, md);

    if (e.exec() == QDialog::Accepted)
    {
        e.getData(this);
    }
}



/*!
 *	\~endlish
 *	Destructor.
 *	\~russian
 *	Деструктор.
 *	\~
 */
wDBTable::~wDBTable()
{
	delete cur;
	cur=0;
}



/*!
 *	\~english
 *	Function init. Sets metadata object and tables object.
 *	\~russian
 *	Функция инициализации. Устанавливает объект метаданных и объект таблицы,
 *	к которому идет обращение для поиска полей или (в случае документа) таблиц
 *	\~
 */
void wDBTable::init()
{
    int id = 0;
    aCfgItem o;

    cur = 0; // старый Q3SqlCursor больше не используется

    md = aWidget::parentContainer(this)->getMd();
    if (md)
    {
        id = aWidget::parentContainer(this)->getId();
        o = md->find(id);

        QString objClass = md->objClass(o);

        if (objClass == md_document)
        {
            o = md->findChild(o, md_tables);
            tables = o;
        }
        else if (objClass == md_journal)
        {
            o = md->findChild(o, md_columns);
            tables = o;
        }
        else if (objClass == md_catalogue)
        {
            o = md->findChild(o, md_element);
            tables = o;
        }
    }
}



/*!
 * \en	Sets available tables, using object tables. \_en
 * \ru 	Заполняет внутренний список доступных таблиц, используя в качестве
 * 	источника информации объект tables.
 * 	Добавляет к имени уже забинденой таблицы звездочку(*). \_ru
 */
void wDBTable::setAvailableTables()
{
    aCfgItem o_table, o = tables;
    QString str;
    QStringList listIdTable;
    int res;
    QList<int> vList = getBindList();

    if (o.isNull())
        return;

    QString objClass = md->objClass(o);
    list_available_tables.clear();

    if (objClass == md_tables)
    {
        res = md->countChild(o, md_table);
        for (int i = 0; i < res; ++i)
        {
            o_table = md->findChild(o, md_table, i);
            listIdTable << QString::number(md->id(o_table));
        }
    }

    if (objClass == md_columns)
    {
        list_available_tables << tr("Available columns");
    }

    if (objClass == md_element)
    {
        list_available_tables << md_element;
    }

    res = listIdTable.count();
    for (int i = 0; i < res; ++i)
    {
        if (vList.contains(listIdTable[i].toInt()))
            str = "* ";
        else
            str.clear();

        list_available_tables
            << str + md->attr(md->find(listIdTable[i].toInt()), mda_name);
    }
}



/*!
 * \en 	Handler line current change signal. \_en
 * \ru	Обработчик сигнала изменения строки. Генерирует сигналы
 * deleteLine и saveLine. \_ru
 */
void wDBTable::lineUpdate(DbOp mode)
{
    if (!cur)
        return;

    QSqlRecord rec = cur->record();

    switch (mode)
    {
        case DbDelete:
            emit deleteLine(rec);
            return;

        case DbInsert:
        case DbUpdate:
        default:
            emit saveLine(rec);
            break;
    }
}



/*!
 *	\~english
 *	Function paint field in table.
 *	\~russian
 *	Переопределенная функция рисования поля таблицы.
 *	В типе DateTime не отображает время.
 *	Поддерживает вычисляемые поля.
 *	\~
 */
void wDBTable::paintField(QPainter *p, const QSqlField *field, const QRect &cr, bool selected)
{
    if (!p || !field)
        return;

    if (field->name().left(5) == "text_")
        return;

    if (field->name() == "system_icon")
    {
        p->drawPixmap(cr, systemIcon());
        return;
    }

    QString text;

    if (cur)
    {
        int textIdx = cur->record().indexOf("text_" + field->name());
        if (textIdx != -1)
        {
            text = cur->value(textIdx).toString();
        }
        else if (field->type() == QVariant::DateTime)
        {
            text = field->value().toDate().toString(Qt::SystemLocaleShortDate);
        }
        else
        {
            text = field->value().toString();
        }
    }
    else
    {
        if (field->type() == QVariant::DateTime)
            text = field->value().toDate().toString(Qt::SystemLocaleShortDate);
        else
            text = field->value().toString();
    }

    if (selected)
        p->fillRect(cr, palette().highlight());

    p->drawText(cr.adjusted(2, 0, -2, 0), Qt::AlignVCenter | Qt::AlignLeft, text);
}



/*!
 *	\~english
 *	Return system icon for display object record state.
 *	\russian
 *	Возвращает иконку для отображения состояния строки.
 *	\~
 */
QPixmap wDBTable::systemIcon()
{
    aWidget *container = aWidget::parentContainer(this);
    QString ctype;
    QPixmap pm;
    int df = 0, cf = 0, mf = 0;

    if (container)
        ctype = container->metaObject()->className();

    int row = currentRow();
    if (row < 0)
        return pm;

    int colDf = -1;
    int colCf = -1;
    int colMf = -1;
    int colId = -1;

    for (int i = 0; i < columnCount(); ++i)
    {
        QTableWidgetItem *hdr = horizontalHeaderItem(i);
        if (!hdr)
            continue;

        QString name = hdr->text();

        if (name == "df") colDf = i;
        else if (name == "cf") colCf = i;
        else if (name == "mf") colMf = i;
        else if (name == "id") colId = i;
    }

    if (colDf != -1 && item(row, colDf))
        df = item(row, colDf)->text().toInt();

    if (colCf != -1 && item(row, colCf))
        cf = item(row, colCf)->text().toInt();
    else if (ctype == "wJournal" && colId != -1 && item(row, colId))
    {
        aDocJournal *sysObj = new aDocJournal(db);
        if (sysObj)
        {
            qulonglong id = item(row, colId)->text().toULongLong();
            if (!id)
                id = item(row, colId)->data(Qt::UserRole).toULongLong();

            if (id && sysObj->findDocument(id))
            {
                aDocument *doc = sysObj->CurrentDocument();
                if (doc)
                {
                    cf = doc->IsConducted();
                    delete doc;
                }
            }

            delete sysObj;
        }
    }

    if (colMf != -1 && item(row, colMf))
        mf = item(row, colMf)->text().toInt();

    if (ctype == "wJournal")
    {
        pm = t_doc;
        if (df) pm = t_doc_d;
        if (cf && !df) pm = t_doc_t;
        if (mf && !df) pm = t_doc_m;
        if (cf && mf && !df) pm = t_doc_tm;
    }
    else if (ctype == "wCatalogue")
    {
        pm = t_cat_e;
        if (df) pm = t_cat_ed;
    }
    else if (ctype == "wCatGroupe")
    {
        pm = t_cat_g;
        if (df) pm = t_cat_gd;
    }

    return pm;
}



/*!
 * \~english
 *	Sets property `DefFields', `DefHeaders', `ColWidth', `DefIdList' to
 *	values from metadata.
 * \~russian
 *	Устанавливает свойства `DefFields', `DefHeaders', `ColWidth', `DefIdList'
 *	в значения, определенные в метаданных. Старые значения не сохраняются.
 *	Для документа используется idTable.
 *	\~
 *	\param idTable (in) - 	\~english table id
 *				\~russian id таблицы документа.  Не используется для каталогов и журналов.
 *	\~
 */
void wDBTable::setFields(int idTable)
{
    CHECK_POINT

    int field_count, j;
    int i;
    QString str;
    QStringList Cwidth, list_fields, list_id;
    aCfgItem o, o_table, o_field;
    QString mdtag = QString(md_field);

    list_fields.clear();
    list_id.clear();
    Cwidth.clear();

    o = tables; // object tables
    QString objClass = md->objClass(o);

    if (objClass == md_tables)
    {
        o_table = md->find(idTable);
    }
    else if (objClass == md_columns)
    {
        o_table = o;
        mdtag = QString(md_column);
    }
    else if (objClass == md_element)
    {
        o_table = o;
    }

    colWidth.clear();

    clear();
    setRowCount(0);
    setColumnCount(0);

    if (!o_table.isNull())
    {
        field_count = md->countChild(o_table, mdtag);

        for (j = 0; j < field_count; ++j)
        {
            o_field = md->findChild(o_table, mdtag, j);

            str = md->attr(o_field, mda_name);

            list_fields << str;
            list_id << md->attr(o_field, mda_id);

            insertColumn(j);
            setHorizontalHeaderItem(j, new QTableWidgetItem(str));
            setColumnWidth(j, getDefColWidth());

            Cwidth << QString::number(getDefColWidth());
        }
    }

    setProperty("DefFields", list_fields);
    setProperty("DefHeaders", list_fields);
    setProperty("ColWidth", Cwidth);
    setProperty("DefIdList", list_id);
}



/*!
 * \en	Gets table id at position table. \_en
 * \ru	Получение id таблицы по ее позиции в метаданных.
 * 	Эта позиция совпадает с индексом таблицы в комбобоксе редактора свойств
 * 	таблицы. \_ru
 * 	\param numTable (in) - 	\en Position table in metadata \_en
 * 				\ru Положение таблицы в метаданных.
 * 				Не используется в каталоге и журнале. \_ru
 * 	\return \en table id \_en \ru id таблицы \_ru
 */
int
wDBTable::getTableId(int numTable)
{
	int res = -1;
	aCfgItem o, o_table;
	if ( numTable == -1 ) return res;
	o = tables; // object tables
	QString objClass = md->objClass(o);
	if(objClass==md_tables)
	{
		o_table = md->findChild(o,md_table,numTable);
		res = md->id(o_table);
	}
	if(objClass==md_columns)
	{
		res = md->id(o);
		//printf("parent name %s\n", (const char*)md->attr(md->parent(o),mda_name));
	}
	if(objClass==md_element)
	{
		res = md->id(o);
	}
	//printf("get table id=%i\n", res);
	return res;
}



/*!
 * \en	Gets table position at id table. \_en
 * \ru	Получение позиции таблицы в метаданных по ее id.
 * 	Эта позиция совпадает с индексом таблицы в комбобоксе редактора свойств
 * 	таблицы. \_ru
 * 	\param id (in) - 	\en Id table \_en
 * 				\ru id таблицы.
 * 				Не используется в каталоге и журнале. \_ru
 * 	\return \en table position for documet or 0 for catalogue and journal\_en
 * 		\ru Для документа возвращает позицию таблицы, для каталога и
 * 		журнала документов возвращает 0 \_ru
 */
int
wDBTable::getTableInd(int id)
{
int j,tableCount;
aCfgItem o, o_table;
	o = tables; // object tables
	QString objClass = md->objClass(o);
	if(objClass==md_columns || objClass==md_element)
	{
		//printf("TableInd=%i\n",tableInd);
		return tableInd;
	}
	tableCount = md->count(o,md_table);
	for(j=tableCount-1; j>=0; j--)
	{
		o_table = md->findChild(o,md_table,j);
		if(md->id(o_table)==id) break;
	}
return j;
}



/*!
 * \en 	Gets list of id fields or list of name fields. \_en
 * \ru 	Получение списка id или имен полей таблицы.
 * 	Возвращаемое значение зависит от параметра GetId. \_ru
 * 	\param idTable (in) - 	\en id of table \_en
 * 				\ru id таблицы документа. Для каталога и журнала
 * 				не используется. \_ru
 * 	\param GetId (ib) - 	\en if GetId == true returns list of id, else
 * 				returns list of names. \_en
 * 				\ru если параметр равен true, возвращается
 * 				список id полей таблицы, иначе возвращается
 * 				список имен полей таблицы. \_ru
 * 	\return \en list of id fields or list of name fields. \_en
 * 		\ru Список id или имен полей таблицы. \_ru
 */
QStringList
wDBTable::getFields(int idTable, bool GetId)
{
	QStringList lst;
	if (idTable==-1) {
		return lst;
	}

	int items_count,j;//,tableCount;
	aCfgItem o, o_table, o_item;
	QString	mdtag=md_field;


//aWidget *wo = aWidget::parentContainer( this );
//	if ( !wo ) return lst;
//	o_table = md->objTable( wo->getId(), idTable );

	o = tables; // object tables
	QString objClass = md->objClass(o);
	if(objClass==md_tables)
	{
		o_table = md->find(idTable);
	}
	else
	if(objClass==md_columns)
	{
		o_table = o;
		mdtag = md_column;
	}
	else
	if(objClass==md_element)
	{
		o_table = o;
	}
	if(!o_table.isNull())
	{
		items_count = md->countChild(o_table,mdtag);
		for (j=0; j<items_count; j++)
		{
			o_item = md->findChild(o_table,mdtag,j);
			if(GetId)
			  lst << md->attr(o_item,mda_id);
			else
			  lst << md->attr(o_item,mda_name);
		}
	}
	else {
		debug_message("Table not found!\n");
	}
return lst;
}



/*!
 *	\~english
 *	Gets field type.
 *	\~russian
 *	Получение типа поля.
 *	\~
 *	\param id - \~english field id \~russian идентификатор поля \~
 *	\return \~english field type \~russian тип поля \~
 */
QString
wDBTable::getFieldType(long id)
{
	aCfgItem o;
	QString str="";

	QString objClass = md->objClass(tables);
	if(objClass==md_columns) {
		id= journalFieldId(id);
	}
	o = md->find(id);
	if(!o.isNull())
	{
		str = md->attr(o,mda_type);
	}
return str;
}



/*!
 *	\~english
 *	Gets field name.
 *	\~russian
 *	Получение имени поля.
 *	\~
 *	\param id - \~english field id \~russian идентификатор поля \~
 *	\return \~english field name \~russian имя поля \~
 */
QString
wDBTable::getFieldName(long id)
{
	aCfgItem o;
	QString str="";
	o = md->find(id);
	if(!o.isNull())
	{
		str = md->attr(o,mda_name);
	}
return str;
}



/*!
 *\~english
 *	function check property name, id, header and width.
 *	If have incorrect field name or id, then print message and all.
 *	If have less or more items in DefHeaders or ColWidth, then set them normal.
 *\~russian
 *	Функция проверяет свойства name, id, DefHeaders и ColWidth.
 *	Если находит ошибки в свойствах name или id, она выводит сообщение об этом, но не исправляет ошибки.
 *	Если находит ошибки в свойствах DefHeaders или ColWidth, то исправляет.
 *\~
 */
void wDBTable::checkFields()
{
    QStringList fl, hl, cl, il;
    int i;
    QString str;

    fl = property("DefFields").toStringList();
    il = property("DefIdList").toStringList();
    cl = property("ColWidth").toStringList();
    hl = property("DefHeaders").toStringList();

    // проверка на наличие field in metadata
    for (i = 0; i < il.count(); ++i)
    {
        str = getFieldName(il[i].toInt());

        if (i < fl.count())
        {
            if (str != fl[i])
            {
                aLog::print(
                    aLog::Debug,
                    QString("wDBTable unknown field name `%1' or (and) id `%2'\n")
                        .arg(str)
                        .arg(il[i]));
            }
        }
        else
        {
            il.removeAt(i);
            --i;
            continue;
        }

        if (i >= hl.count())
            hl << str;

        if (i >= cl.count())
            cl << QString::number(getDefColWidth());
    }

    while (i < hl.count())
        hl.removeAt(i);

    while (i < cl.count())
        cl.removeAt(i);

    setProperty("DefFields", fl);
    setProperty("DefHeaders", hl);
    setProperty("ColWidth", cl);
    setProperty("DefIdList", il);
}


/*!
 *\~english
 *	Initialisation the widget on form loaded in engine.
 *\~russian
 *	Инициализация виджета при загрузке в форму инжина.
 *\~
 */
void wDBTable::init(aDatabase *adb, aEngine *e)
{
    aLog::print(aLog::Debug, tr("wDBTable init in engine"));

    aCfgItem o;
    QString ctype;
    QStringList lst, lstHead, lstWidth;
    int tid;
    aWidget *container = 0;

    t_doc    = rcIcon("t_doc.png");
    t_doc_d  = rcIcon("t_doc_d.png");
    t_doc_t  = rcIcon("t_doc_t.png");
    t_doc_m  = rcIcon("t_doc_m.png");
    t_doc_tm = rcIcon("t_doc_tm.png");
    t_cat_e  = rcIcon("t_cat_e.png");
    t_cat_ed = rcIcon("t_cat_ed.png");
    t_cat_g  = rcIcon("t_cat_g.png");
    t_cat_gd = rcIcon("t_cat_gd.png");

    engine = e;
    db = adb;
    md = &adb->cfg;

    tid = property("TableInd").toInt();

    container = aWidget::parentContainer(this);
    if (!container)
    {
        aLog::print(aLog::Error, tr("wDBTable not in Ananas object container"));
        return;
    }

    o = md->objTable(container->getId(), tid);
    if (o.isNull())
    {
        aLog::print(aLog::Error, tr("wDBTable init meta object not found"));
    }

    ctype = container->metaObject()->className();
    aLog::print(aLog::Info, tr("wDBTable container type is %1").arg(ctype));
    setContainerType(ctype);

    if (o.isNull())
        return;

    clear();
    setRowCount(0);
    setColumnCount(0);

    lst     = property("DefIdList").toStringList();
    lstHead = property("DefHeaders").toStringList();
    lstWidth= property("ColWidth").toStringList();

    int col = 0;

    if (containerType() == "wJournal")
    {
        insertColumn(col);
        setHorizontalHeaderItem(col, new QTableWidgetItem(""));
        setColumnWidth(col, 20);
        ++col;

        if (md->objClass(*(container->getMDObject())) == md_journal &&
            !((aDocJournal*)container->dataObject())->type())
        {
            insertColumn(col);
            setHorizontalHeaderItem(col, new QTableWidgetItem(tr("Date")));
            setColumnWidth(col, 100);
            ++col;

            insertColumn(col);
            setHorizontalHeaderItem(col, new QTableWidgetItem(tr("Prefix")));
            setColumnWidth(col, 200);
            ++col;

            insertColumn(col);
            setHorizontalHeaderItem(col, new QTableWidgetItem(tr("Number")));
            setColumnWidth(col, 100);
            ++col;
        }
    }

    if (containerType() == "wCatalogue")
    {
        insertColumn(col);
        setHorizontalHeaderItem(col, new QTableWidgetItem(""));
        setColumnWidth(col, 20);
        ++col;
    }

    if (md->objClass(*(container->getMDObject())) != md_journal ||
        ((aDocJournal*)container->dataObject())->type())
    {
        for (int i = 0; i < lst.count(); ++i)
        {
            QString str;

            if (containerType() == "wJournal")
                str = "uf" + QString::number(journalFieldId(lst[i].toLong()));
            else
                str = "uf" + lst[i];

            insertColumn(col);
            setHorizontalHeaderItem(col, new QTableWidgetItem(lstHead.value(i)));

            if (i < lstWidth.count())
                setColumnWidth(col, lstWidth[i].toInt());

            ++col;
        }
    }

    if (containerType() == "wDocument")
    {
        QString flt = QString("idd=%1").arg(container->uid());
        aLog::print(aLog::Info, tr("wDBTable filter is %1").arg(flt));
        searchString = flt;
    }
    else if (containerType() == "wCatalogue")
    {
        searchString = "idg=0";
        newDataId(0);
    }
    else
    {
        searchString = QString();
    }

    cur = 0;
    setWFieldEditor();
    viewport()->update();

    aLog::print(aLog::Debug, tr("wDBTable init in engine ok"));
}



/*!
 *	\~english
 *	Set custom field editor (wField) used property `value'.
 *	\~russian
 *	Устанавливает свой редактор ячейки таблицы (wField).
 *	\~
 */
void wDBTable::setWFieldEditor()
{
    aEditorFactory *f = new aEditorFactory();
    f->setMd(md);
}



/*!
 *	\~english
 *	Construct cell editor. Creates new wField object and inited it.
 *	\~russian
 *	Конструктор редактора ячейки. Создает новый экземпляр объекта wField и инициализирует его.
 *	\~
 */
QWidget *aEditorFactory::createEditor(QWidget *parent, const QSqlField *field)
{
    wField *tmp = 0;
    wDBTable *t = 0;
    QString str, stmp;
    wField::tEditorType type = wField::Unknown;

    if (md)
    {
        if (parent && parent->parent() && parent->parent()->parent())
            t = qobject_cast<wDBTable *>(parent->parent()->parent());

        QWidget *p = 0;
        if (parent && parent->parent())
            p = qobject_cast<QWidget *>(parent->parent());

        tmp = new wField(p, "", 0);

        if (t && field)
        {
            str = field->name();
            str.remove(0, 2);

            tmp->setFieldType(t->getFieldType(str.toInt()));

            str = t->getFieldType(str.toInt());
            stmp = str.section(' ', 0, 0);

            if (stmp == "C") type = wField::String;
            else if (stmp == "N") type = wField::Numberic;
            else if (stmp == "D") type = wField::Date;
            else if (stmp == "B") type = wField::Boolean;
            else if (stmp == "O")
            {
                int tid = str.section(' ', 1, 1).toInt();
                aCfgItem o = md->find(tid);

                if (!o.isNull())
                {
                    str = md->objClass(o);

                    if (str == md_catalogue)
                        type = wField::Catalogue;
                    else if (str == md_document)
                        type = wField::Document;
                }
                else
                {
                    aLog::print(aLog::Error, QObject::tr("aEditorFactory field metaobject not found"));
                }
            }

            tmp->setEditorType(type);
            tmp->initObject(t->db);
            tmp->engine = t->engine;
        }
    }
    else
    {
        aLog::print(aLog::Error, QObject::tr("aEditorFactory metadata in null"));
    }

    if (parent && parent->parent() && parent->parent()->parent() && tmp)
    {
        QWidget *tabParent = qobject_cast<QWidget *>(parent->parent()->parent());
        if (tabParent)
            QWidget::setTabOrder(tabParent, tmp);
    }

    return tmp;
}


/*!
 *	\~english
 *	Gets list of id binding table.
 *	\~russian
 *	Получение списка уже присутствующих в форме таблиц. Необходимо для того, чтобы одну таблицу не добавили два раза.
 *	\~
 *	\return \~english list of id binding table. \~russian список таблиц \~
 */
QList<int> wDBTable::getBindList()
{
    QObject *wd = aWidget::parentContainer(this);
    listBindings.clear();

    QList<wDBTable*> tables = wd->findChildren<wDBTable*>();

    for (QList<wDBTable*>::iterator it = tables.begin(); it != tables.end(); ++it)
    {
        wDBTable *wtable = *it;

        if (wtable == this)
            continue;

        QString objName = wtable->objectName();

        if (objName.startsWith("qt_dead_widget_"))
            continue;

        int id = wtable->property("TableInd").toInt();

        if (id >= 0)
            listBindings << id;
    }

    return listBindings;
}



/*!
 *	\~english
 *	Get field value slot. Don't implemented.
 *	\~russian
 *	Получение значения поля. Не реализовано.
 *	\~
 */
QVariant
wDBTable::Value( const QString &colname )
{
	aLog::print(aLog::Error, tr("wDBTable: function Value() call"));
	return QVariant("");
}



/*!
 *\~english
 *	Emit signal on select record.
 *	Signal give information about selected record database id.
 *\~russian
 *	Выдает сигнал при выборе записи.
 *	Сигнал содержит информацию об id, выбранного объекта в базе данных.
 *\param -\~english Not use, only for signal compatibility.\~russian
 *	Не используются, заданы только для совместимости с сигналом таблицы.\~
 *\~
 */
void wDBTable::lineChange(int row, int /*col*/)
{
    if (row < 0)
        return;

    QTableWidgetItem *it = item(row, 0);
    if (!it)
        return;

    qulonglong id = it->data(Qt::UserRole).toULongLong();
    if (!id)
        id = it->text().toULongLong();

    aLog::print(aLog::Info, tr("wDBTable: select document %1").arg(id));

    emit selectRecord(id);
}


/*!
 *\ru
 *	Устанавливает значение системного поля idd во вновь добавляемую запись табличной части документа.
 *\_ru
 */
void wDBTable::lineInsert(QSqlRecord *rec)
{
    if (!rec)
        return;

    if (containerType() == "wDocument")
    {
        if (rec->indexOf("idd") != -1)
            rec->setValue("idd", QVariant::fromValue(doc_id));

        if (rec->indexOf("ln") != -1)
            rec->setValue("ln", rowCount() - 1);
    }

    if (containerType() == "wCatalogue")
    {
        if (rec->indexOf("idg") != -1)
            rec->setValue("idg", QVariant::fromValue(cat_group_id));
    }
}


/*!
 *	\~english
 *	Delete line function
 *	\~russian
 *	Функция удаления строки.
 *	\~
 *	\return \~english true, if no error \~russian true, если ошибок не было \~
 */
bool wDBTable::deleteCurrent()
{
    if (!confirmDelete())
        return false;

    int row = currentRow();
    if (row < 0)
        return false;

    removeRow(row);
    return true;
}


/*!
 *	\~english
 *	Key press handler.
 *	\~russian
 *	Обрабатывает события при нажатии кнопок клавиатуры.
 *	\~
 */
void wDBTable::keyPressEvent(QKeyEvent *e)
{
    qulonglong id = 0;

    if (!searchMode && !e->text().isEmpty() && e->text().at(0).isPrint())
        searchOpen(e->text());
    else
        searchClose();

    if (containerType() == "wJournal")
    {
        e->ignore();
        QTableWidget::keyPressEvent(e);
        return;
    }

    if (containerType() == "wCatalogue")
    {
        switch (e->key())
        {
            case Qt::Key_Escape:
                e->ignore();
                break;

            case Qt::Key_Return:
            case Qt::Key_Enter:
            {
                int row = currentRow();
                if (row >= 0)
                {
                    QTableWidgetItem *it = item(row, 0);
                    if (it)
                    {
                        id = it->data(Qt::UserRole).toULongLong();
                        if (!id)
                            id = it->text().toULongLong();

                        if (id)
                        {
                            if (e->modifiers() & Qt::ShiftModifier)
                                EditElement();
                            else
                                emit selected(id);

                            e->accept();
                            return;
                        }
                    }
                }

                aLog::print(aLog::Info, tr("wDBTable: current record not setted"));
                break;
            }

            default:
                e->ignore();
                break;
        }
    }

    QTableWidget::keyPressEvent(e);
}



/*!
 *\ru
 *	Слот обрабатывает ситуацию смены/установки идентификатора документа, устанавливая новый фильтр.
 *	Кроме переустановки фильта следует вызывать слот newDataId, для смены/установки
 *	значения идентификатора документа. Для журнала этого можно не делать.
 *	\param flt - строка вида "idd=999", задающая условия отбора записей в таблицу по значению поля idd.
 *\_ru
 */
void wDBTable::newFilter(const QString &flt)
{
    searchString = flt;
    viewport()->update();
}

/*!
 *\ru
 *	Слот обрабатывает ситуацию смены/установки идентификатора документа, запоминая новое значение.
 *	Для смены фильта следует вызывать слот newFilter.
 *	\param id - идентификатор документа, который следует запомнить.
 *\_ru
 */
void
wDBTable::newDataId(const qulonglong id)
{
	if(containerType() == "wDocument")
	{
		setId(id);
	}
	if(containerType() == "wCatalogue")
	{
		cat_group_id = id;
	}
}


/*!
 *	\~english
 *	Opens form for edit catalogue element.
 *	\~russian
 *	Открывает форму по умолчанию для объекта контейнера и настраивает ее на работу с текущей строкой таблицы. Используется только в настраиваемом редакторе каталога.
 *	\~
 */
void wDBTable::EditElement()
{
    ANANAS_UID id = 0;
    aForm *f = 0;

    int row = currentRow();
    if (row < 0)
        return;

    QTableWidgetItem *it = item(row, 0);
    if (!it)
        return;

    id = it->data(Qt::UserRole).toULongLong();

    if (id && engine)
    {
        f = engine->openForm(
                aWidget::parentContainer(this)->getId(),
                0,
                md_action_edit,
                md_form_elem,
                id,
                (aWidget *) this);

        if (f)
        {
            connect(f, SIGNAL(update(ANANAS_UID)),
                    this, SLOT(updateItem(ANANAS_UID)));
        }
    }
}


long
wDBTable::journalFieldId(long columnId){
	aCfgItem item;

	item= md->find(md->find(columnId),md_fieldid);
	return md->text(item).toLong();

}

QString wDBTable::journalFieldName(long columnId)
{
    aCfgItem item;

    item = md->find(md->find(columnId), md_fieldid);
    item = md->find(md->text(item).toLong());

    if (!item.isNull())
    {
        QString s = md->attr(item, mda_type);
        QChar ch = s.at(0);

        if (ch.toUpper() == 'O')
            return QString("text_uf%1").arg(md->attr(item, mda_id));
        else
            return QString("uf%1").arg(md->attr(item, mda_id));
    }

    return QString("uf0");
}

/*!
 *	\~english
 *	Hanler table cell end edit.
 *	\~russian
 *	Обрабатывает окончание редактирования ячейки таблицы.
 *	\~
 */
//bool
void
wDBTable::updateTableCellHandler(int r, int c)
{
//	printf(">>>>update curr row %d  col %d!\n",r,c);
	lastEditedRow= r;
	lastEditedCol= c;
}

/*!
 *	\~english
 *	Reimplemented QDataTable function.
 *	\~russian
 *	Переопределяет функцию QDataTable. Испускает сигнал updateCurr().
 *	\~
 */
bool wDBTable::updateCurrent()
{
    int row = currentRow();
    int col = currentColumn();

    if (row < 0 || col < 0)
        return false;

    emit updateCurr(row, col);
    return true;
}


/*!
 *	\~english
 *	Reimplemented QDataTable function.
 *	\~russian
 *	Переопределяет функцию QDataTable. Отрабатывет после нажатия Enter в редактируемой ячейке.
 *	Перемещает фокус ввода на следующую ячейку.
 *	\~
 */
void
wDBTable::activateNextCell()
{
	//int r = currentRow(), c = currentColumn();
	//if(r<0) r=0;
	//if(c<0) c=0;
	//tCurrentCell(r,c+1);
}




/*!
 *	\~english
 *	Reimplemented QDataTable function.
 *	\~russian
 *	Переопределяет функцию QDataTable. Выделяет текст в ячейке.
 *	\~
 */
//>>>>>>> 1.71.2.13
QWidget *wDBTable::beginUpdate(int row, int col, bool replace)
{
    Q_UNUSED(replace);

    setCurrentCell(row, col);

    QTableWidgetItem *it = item(row, col);
    if (!it)
    {
        it = new QTableWidgetItem;
        setItem(row, col, it);
    }

    editItem(it);

    QWidget *editor = focusWidget();
    wField *wd = qobject_cast<wField *>(editor);
    if (wd)
        wd->selectAll();

    return editor;
}


void wDBTable::doubleClickEventHandler(int /*row*/, int /*col*/, int /*button*/, const QPoint & /*mousePos*/)
{
    if (containerType() == "wCatalogue" || containerType() == "wJournal")
    {
        int row = currentRow();
        if (row < 0)
            return;

        QTableWidgetItem *it = item(row, 0);
        if (!it)
            return;

        qulonglong id = it->data(Qt::UserRole).toULongLong();
        if (id != 0)
            emit selected(id);
    }
}

/**
 * \ru
 * 		\brief Переопределяем метод базового класса, вызывающий диалог подтверждения.
 * 		Переопределен только диалог подтверждения удаления.
 * \_ru
 */
bool wDBTable::confirmDelete()
{
    return QMessageBox::question(
               this,
               tr("Remove record?"),
               tr("You are going to remove record <br>"
                  "Are you sure?"),
               tr("&Yes, remove"),
               tr("&No"),
               QString(),
               0,
               1) == 0;
}

/**
 * \ru
 * 		\brief Переопределяем метод базового класса, осуществляющий вставку новой строки в таблицу.
 * 		Наш метод будет всегда помещать новую строку в начале таблицы.
 * \_ru
 */
bool wDBTable::beginInsert()
{
    if (columnCount() == 0)
        return false;

    int row = rowCount();
    insertRow(row);

    for (int col = 0; col < columnCount(); ++col)
    {
        if (!item(row, col))
            setItem(row, col, new QTableWidgetItem);
    }

    setCurrentCell(row, 0);
    editItem(item(row, 0));

    return true;
}


/*!
 *	\~english
 *	Reimplemented QDataTable function.
 *	\~russian
 *	Переопределяет функцию QDataTable. Если контейнер wJournal, может испускаеть сигналы insertRequest(), updateRequest(), deleteRequest(), viewRequest()
 *	\~
 */
void wDBTable::contentsContextMenuEvent(QContextMenuEvent *e)
{
    QString ctype = containerType();

    if (ctype == "wDocument" || ctype == "wCatalogue")
    {
        enum
        {
            IdInsert = 0,
            IdUpdate,
            IdDelete
        };

        QMenu popupForDoc(this);
        QAction *actions[3];

        actions[IdInsert] = popupForDoc.addAction(tr("New"));
        actions[IdUpdate] = popupForDoc.addAction(tr("Edit"));
        actions[IdDelete] = popupForDoc.addAction(tr("Delete"));

        bool enabled = columnCount() > 0;
        actions[IdInsert]->setEnabled(enabled);
        actions[IdUpdate]->setEnabled(enabled);
        actions[IdDelete]->setEnabled(enabled);

        QAction *res = popupForDoc.exec(e->globalPos());

        if (res == actions[IdInsert])
        {
            beginInsert();
        }
        else if (res == actions[IdUpdate])
        {
            QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_F2, Qt::NoModifier);
            keyPressEvent(&keyEvent);
        }
        else if (res == actions[IdDelete])
        {
            deleteCurrent();
        }

        e->accept();
        return;
    }

    if (ctype == "wJournal")
    {
        enum
        {
            IdInsert = 0,
            IdUpdate,
            IdDelete,
            IdView,
            IdRefresh
        };

        QMenu popup(this);
        QAction *actions[5];

        actions[IdInsert]  = popup.addAction(tr("New"));
        actions[IdUpdate]  = popup.addAction(tr("Edit"));
        actions[IdDelete]  = popup.addAction(tr("Delete"));
        actions[IdView]    = popup.addAction(tr("View"));
        actions[IdRefresh] = popup.addAction(tr("Refresh"));

        QAction *res = popup.exec(e->globalPos());

        if (res == actions[IdInsert])
            emit insertRequest();
        else if (res == actions[IdUpdate])
            emit updateRequest();
        else if (res == actions[IdDelete])
            emit deleteRequest();
        else if (res == actions[IdView])
            emit viewRequest();
        else if (res == actions[IdRefresh])
            viewport()->update();

        e->accept();
        return;
    }

    QTableWidget::contextMenuEvent(e);
}


void wDBTable::updateItem(ANANAS_UID db_uid)
{
    Q_UNUSED(db_uid);

    viewport()->update();

    emit updateCurr(currentRow(), currentColumn());
}

int wDBTable::Select(ANANAS_UID db_uid)
{
    int curr = currentRow();
    int curc = currentColumn();

    for (int row = 0; row < rowCount(); ++row)
    {
        QTableWidgetItem *item = this->item(row, 0); // колонка id
        if (!item) continue;

        if (item->data(Qt::UserRole).toULongLong() == db_uid)
        {
            setCurrentCell(row, curc);
            return 0;
        }
    }

    setCurrentCell(curr, curc);
    return 0;
}


bool wDBTable::searchColumn(const QString &text, bool FromCurrent, bool Forward)
{
    int curRow = currentRow();
    int curCol = currentColumn();

    if (curRow < 0 || curCol < 0)
        return false;

    int row = FromCurrent ? curRow : (Forward ? 0 : rowCount() - 1);

    if (FromCurrent)
        row += Forward ? 1 : -1;

    while (row >= 0 && row < rowCount())
    {
        QTableWidgetItem *item = this->item(row, curCol);
        QString s = item ? item->text() : QString();

        if (s.startsWith(text))
        {
            setCurrentCell(row, curCol);
            return true;
        }

        row += Forward ? 1 : -1;
    }

    return false;
}



void
wDBTable::searchOpen( const QString & text )
{
	searchWidget = new aSearchWidget( aWidget::parentContainer( this ), this );
	searchMode = TRUE;
	searchWidget->setFocus();
	searchWidget->search( text );
}


void
wDBTable::searchClose()
{
	if ( searchWidget ) {
		setFocus();
		searchWidget->deleteLater();
		searchMode = FALSE;
		searchWidget = 0;
	}
}


/*!
 *\~english
 *
 *
 *\~russian
 *	Осуществляет интерактивный поиск по первым буквам
 *	текстового значения текущего столбца таблицы.
 *	Пока не реализован поиск для столбцов типа "Объект".
 *	Up 	- предыдущее значение.
 *	Down 	- следующее значение.
 *	ESC,Enter - конец поиска.
 *\~
 */
aSearchWidget::aSearchWidget(QWidget *parent, wDBTable *table) : QFrame(parent)
{
    t = table;
    ftext.clear();

    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setFocusPolicy(Qt::StrongFocus);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    l = new QLineEdit(this);
    l->installEventFilter(this);
    setFocusProxy(l);
    layout->addWidget(l);

    int col = t->currentColumn();
    int x = 3 + t->x() + t->columnViewportPosition(col);
    int y = t->y() + 1;

    move(x, y);
    resize(t->columnWidth(col) - 2, 25);

    connect(l, SIGNAL(textChanged(const QString &)),
            this, SLOT(setText(const QString &)));
}


aSearchWidget::~aSearchWidget()
{
}


void
aSearchWidget::search( const QString &t )
{
	show();
	l->setText( t );
}


void
aSearchWidget::setText( const QString &text )
{
	if ( t->searchColumn( text ) ) ftext = text;
	else l->setText( ftext );
}


bool
aSearchWidget::eventFilter( QObject *obj, QEvent *ev )
{
	if ( obj == l ) {
		if ( ev->type() == QEvent::FocusOut ) {
			t->searchClose();
			return TRUE;
		}
		if ( ev->type() == QEvent::KeyPress ) {
		QKeyEvent *e = ( QKeyEvent *) ev;
			switch ( e->key() ){
			case Qt::Key_Return:
			case Qt::Key_Escape:
				//printf("OK\n");
				t->searchClose();
				break;
			case Qt::Key_Up:
				t->searchColumn( ftext, TRUE, FALSE );
				break;
			case Qt::Key_Down:
				t->searchColumn( ftext, TRUE, TRUE );
				break;
			default:
				return FALSE;
				break;
			}
                return TRUE;
            } else {
                return FALSE;
            }
        } else {
            // pass the event on to the parent class
            return QFrame::eventFilter( obj, ev );
        }
}
