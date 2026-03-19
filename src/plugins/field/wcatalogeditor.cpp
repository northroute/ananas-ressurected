/****************************************************************************
** $Id: wcatalogeditor.cpp,v 1.2 2008/11/08 20:16:36 leader Exp $
**
** Code file of the field plugin of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
** Copyright (C) 2003-2004 Grigory Panov, Yoshkar-Ola.
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

#include "ananas.h"
#include "wcatalogeditor.h"
#include "wfield.h"
#include "wdbtable.h"
#include "catalogform.h"
#include "alog.h"
#include "../ananas/mainform.h"


/*!
 * \en 	Creates catalog editor widget. \_en
 * \ru 	Создает виджет для редактирования каталога.
 * 	В принципе он не предусмотрен использоваться ни как иначе, кроме как часть
 * 	объекта wField, т.е его родительский объект должен быть wField
 * 	или его наследник для обеспечения корректной инициализации.
 * 	При создании объекта runtime не предусмотрено, за исключением случая,
 * 	когда виджет используется в составе wField в объекте wDBTable.
 * 	\param parent - \en parent object. \_en \ru родительский объект. \_ru
 * 	\param name - \en name \_en \ru имя \_ru
 * 	\param catname \en not used \_en \ru не используется \_ru
 */
wCatalogEditor::wCatalogEditor(wField *parent,
                               const char *name,
                               const char *catname)
    : QWidget(parent)
{
    Q_UNUSED(catname);

    if (name)
        setObjectName(name);

    QWidget *w = 0;
    md = 0;
    QString str;

    label = new QLabel(parent);
    if (name)
        label->setObjectName(name);

    label->setFrameShape(QFrame::Box);

    if (parent && parent->parent() && parent->parent()->parent())
        w = qobject_cast<QWidget *>(parent->parent()->parent());

    str = parent->getFieldType();
    str.remove(0, 2);
    catId = str.toInt();

    if (w)
    {
        wDBTable *table = qobject_cast<wDBTable *>(w);
        if (table)
            initCat(table->db);
    }
}

wCatalogEditor::wCatalogEditor(QWidget* parent,int cat) : QWidget()
{
	catId = cat;
	label = 0;
	ws = parent;
}


/*!
 * \en	Destructor catalog editor. \_en
 */
wCatalogEditor::~wCatalogEditor()
{
	delete label;
	label = 0;
}



/*!
 * \en	Initialisation catalog editor. \_en
 * \ru	Инициализация редактора каталога.
 * 	Инициализирует внутренние объекты метаданных и базы банных. \_ru
 */
void
wCatalogEditor::initCat(aDatabase * adb)
{
	md = &adb->cfg;
	db = adb;
}


/*!
 * \en	Handler signal click().
 * 	Creates and shows form for select catalog. \_en
 * \ru	Обработчик события нажатия кнопки,
 * 	которая является второй составной частью виджета.
 * 	Сигнал коннектится в объекте wField.
 * 	Создает и отображет в рабочем пространстве енжина форму для
 * 	редактирования каталога или выбора из каталога.
 * 	Производит заполнение дерева групп и элементов группами и формирует
 * 	данные для передачи в форму редактирования каталога.
 * 	Создает и отображет в рабочем пространстве енжина эту форму.\_ru
 */
void
wCatalogEditor::select()
{
	openForm(true);
}
/*!
 * \en	Creates and shows form for edit  catalog. \_en
 * \ru	Обработчик события нажатия кнопки,
 * 	которая является второй составной частью виджета.
 * 	редактирования каталога.
 * 	Производит заполнение дерева групп и элементов группами и формирует
 * 	данные для передачи в форму редактирования каталога.
 * 	Создает и отображет в рабочем пространстве енжина эту форму.\_ru
 */
void
wCatalogEditor::edit()
{
	openForm(false);
}

void wCatalogEditor::openForm(const bool toSelect)
{
    aLog::print(aLog::Debug, tr("wCatalog Editor open form for select=%1 ").arg((int)toSelect));

    MainForm *mainform = 0;
    if (parent())
    {
        QWidget *tlw = topLevelWidget();
        if (tlw && tlw->inherits("MainForm"))
        {
            mainform = (MainForm *) tlw;
            ws = mainform->ws;
        }
    }

    CatalogForm *newform = new CatalogForm((QWidget *)ws);
	newform->setAttribute(Qt::WA_DeleteOnClose, true);

    connect(newform, SIGNAL(selected(qulonglong)),
            this, SLOT(on_selected(qulonglong)));
    connect(newform, SIGNAL(destroyed()),
            this, SLOT(on_destroyed_form()));

    aCatalogue *cat = new aCatalogue(md->find(catId), db);
    int count = 0;
    bool est = true;
    QMap<qulonglong, QTreeWidgetItem*> map, map_el;
    aCfgItem tmp, tmp_f, tmp_el, tmp_group, o;
    QTreeWidgetItem *item = 0;
    QTreeWidgetItem *p_item = 0;
    qulonglong idGrForm = 0, idElForm = 0;
    QStringList listPos, listPosGroup;

    newform->ListHint->hide();

    QIcon icon(newform->getGroupPixmap());
    QIcon icon_mark_deleted(newform->getMarkDeletedPixmap());

    tmp = md->find(catId);
    newform->setWindowTitle(md->attr(tmp, mda_name));

    o = md->findChild(tmp, md_forms);
    if (!o.isNull())
    {
        count = md->count(o, md_form);
        for (int i = 0; i < count; ++i)
        {
            tmp_f = md->findChild(o, md_form, i);
            if (!tmp_f.isNull() && md->attr(tmp_f, mda_type).toInt() == md_form_elem)
            {
                aLog::print(aLog::Debug, tr("wCatalog Editor found element forms"));
                idElForm = md->id(tmp_f);
            }
            if (!tmp_f.isNull() && md->attr(tmp_f, mda_type).toInt() == md_form_group)
            {
                aLog::print(aLog::Debug, tr("wCatalog Editor found group forms"));
                idGrForm = md->id(tmp_f);
            }
        }
    }
    else
    {
        aLog::print(aLog::Error, tr("wCatalog Editor meta object forms not found"));
    }

    tmp_el = md->findChild(tmp, md_element);
    tmp_group = md->findChild(tmp, md_group);
    tmp = md->findChild(tmp_el, md_field);

    int count_fields = md->count(tmp_el, md_field);
    Q_UNUSED(count_fields);
    Q_UNUSED(tmp_group);
    Q_UNUSED(map_el);

    listPosGroup = cat->getGroupUserFields();

    int level = 0;
    cat->Select();

    while (est)
    {
        est = false;
        ++level;
        cat->selectByLevel(level - 1);

        if (!cat->FirstInGroupTable())
            break;

        do
        {
            if (cat->GroupSysValue("level").toInt() == level - 1)
            {
                est = true;

                QString displayString = cat->GroupSysValue(listPosGroup[0]).toString();

                if (map.contains(cat->GroupSysValue("idp").toULongLong()))
                {
                    p_item = map[cat->GroupSysValue("idp").toULongLong()];
                    item = new QTreeWidgetItem(p_item);
                }
                else
                {
                    item = new QTreeWidgetItem(newform->ListView);
                }

                item->setText(0, displayString);

                if (cat->isGroupMarkDeleted())
                    item->setIcon(0, icon_mark_deleted);
                else
                    item->setIcon(0, icon);

                map.insert(cat->GroupSysValue("id").toULongLong(), item);
            }
        }
        while (cat->NextInGroupTable());
    }

    listPos = cat->getUserFields();
    checkUserFields(listPos);

    int fid;
    for (int i = 0; i < listPos.count(); ++i)
    {
        QString fieldName = listPos[i];

        if (fieldName.startsWith("text_"))
            fieldName.remove(0, 5);

        if (fieldName.startsWith("uf"))
            fieldName.remove(0, 2);

        fid = fieldName.toInt();

        if (fid)
        {
            tmp = md->find(fid);
            newform->ListView->headerItem()->setText(i, md->attr(tmp, mda_name));
        }
    }

    listPos.clear();
    listPos = cat->getUserFields();
    checkUserFields(listPos);

    newform->setData(cat,
                     map,
                     listPos,
                     cat->getGroupUserFields(),
                     idElForm,
                     idGrForm,
                     toSelect);

    newform->setId(value().toULongLong());

    if (ws)
        ((QWorkspace *)ws)->addWindow(newform);

    newform->show();

    if (newform->parentWidget())
        newform->parentWidget()->move(0, 0);
}

void wCatalogEditor::checkUserFields(QStringList &lst)
{
    aCfgItem item = md->find(catId);
    if (item.isNull())
        return;

    item = md->findChild(item, md_element);

    for (int i = 0; i < md->count(item, md_field); ++i)
    {
        aCfgItem mdi = md->findChild(item, md_field, i);

        QString uf = QString("uf%1").arg(md->attr(mdi, mda_id));
        int ind = lst.indexOf(uf);

        if (ind != -1)
        {
            lst.insert(i, lst[ind]);
            lst.removeAt(ind + 1);
        }
        else
        {
            QString tuf = QString("text_uf%1").arg(md->attr(mdi, mda_id));
            ind = lst.indexOf(tuf);

            if (ind != -1)
            {
                lst.insert(i, lst[ind]);
                lst.removeAt(ind + 1);
            }
        }
    }
}

void
wCatalogEditor::on_selected( qulonglong uid )
{
	emit( selected(uid) );
}

void
wCatalogEditor::on_destroyed_form( )
{
	emit( destroyed_form() );
}


/*!
 * \en 	Gets catalog editor value. \_en
 * \ru 	Получение значения редактора каталога.
 * 	Т.е. то, что было выбрано или утановлено функцией setValue().
 * 	Сейчас возвращает стринг с id, но может быть
 * 	Для получения осмысленного значения используй функцию displayValue() \_ru
 */
QString
wCatalogEditor::value() const
{
	return vValue;
}


/*!
 * \en 	Gets value for displaying. \_en
 * \ru	Получение осмысленного представления id.
 * 	Строка является представлением уникального id для элемента
 * 	или группы каталога. \_ru
 * 	\param id_element (in) -\en unique id element or group \_en
 * 				\ru уникальный идентификатор элемента или группы \_ru
 */
QString
wCatalogEditor::displayValue(qulonglong id_element) const
{
const QString no_select = "  ";
	if(md)
	{
	 /* 	aCfgItem tmp,field;
		QString res;
		tmp = md->find(catId);
 		tmp = md->findChild(tmp, md_element);
		field = md->findChild(tmp,md_field);
  	 	aSQLTable* te = new aSQLTable(tmp,db);
		te->select(QString("id=%1").arg(id_element));
		if(te->first())
		{
			res = te->sysValue(QString("uf%1").arg(md->id(field))).toString();
		}
		else res = no_select;
		delete te;*/
		return QString("%1").arg(id_element);
	}
	else return no_select;
}


/*!
 * \en 	Sets id for displaying. \_en
 * \ru	Установка id для показа.
 * 	Хранится в стринге для совмесимости с wField. \_ru
 */
void
wCatalogEditor::setValue(QString newvalue)
{
	if(vValue==newvalue) return;
	vValue = newvalue;
	if(label) label->setText(displayValue(newvalue.toULongLong()));
	emit valueChanged(value());
}

