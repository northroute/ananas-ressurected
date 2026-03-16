/****************************************************************************
** $Id: wfield.cpp,v 1.1 2008/11/05 21:16:30 leader Exp $
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

//--#include "command.h"
//--#include "mainwindow.h"
//--#include "formwindow.h"

#include "ananas.h"
#include "wfield.h"
#include "wdateedit.h"
#include "wcatalogeditor.h"
#include "efield.h"



/*!
 * \en	Constructs object with parent=parent, name=name and flags=fl \_en
 * \ru	Создает объект с родителем parent, именем name и флагом fl. \_ru
 */
wField::wField(QWidget *parent, const char *name, Qt::WFlags fl) : aWidget(parent, name, fl)
{
    loaded = 0;
    md_oid = 0;
    md_fid = 0;

    setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    setFocusPolicy(Qt::StrongFocus);

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    lineEdit = new QLineEdit(this);
    lineEdit->hide();

    dateEdit = new wDateEdit(this, "");
    dateEdit->hide();

    objButton = new wCatButton("...", this);
    objButton->hide();

    objLabel = new QLabel(this);
    objLabel->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred));
    objLabel->setFrameShape(QFrame::Box);
    objLabel->setText("UnknownField");
    objLabel->show();

    checkBox = new wCheckBox(this);
    checkBox->hide();

    vEditorType = Unknown;
}

/*
wField::wField( QWidget *parent, const char *name, WFlags fl, bool dbf )
    : aWidget( parent, name, fl )
{
    setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) );
    //vValue = "";
    //vFieldType="";
    new QHBoxLayout( this, 0, 0 );
    loaded = 0;
    fieldEditor = NULL;
    fieldSelector = NULL;
    cfgobj_name = NULL;
    cfgobj_type = NULL;
    vEditorType = Unknown;
    setMode( Mode0 );
    if ( dbf ) widgetInit();
}
*/


/*!
 * \en 	Destructor. \_en
 */
wField::~wField()
{
}


/*!
 * \en	Sets widget visual type, depend of property fieldType.
 * 	This property must be set above call this function. \_en
 * \ru	Устанавливает визуальный тип виджета в зависимости от свойства
 * 	`fieldType'. Это свойсто должно быть установлено до вызова этой функции,
 * 	иначе тип виджета будет установлен в Unknown. \_ru
 */
void wField::widgetInit()
{
    int n1 = 0;
    int n2 = 0;

    lineEdit->hide();
    disconnect(lineEdit, SIGNAL(textChanged(const QString &)),
               this, SLOT(setValue(const QString &)));
    lineEdit->setReadOnly(false);
    layout()->removeWidget(lineEdit);

    dateEdit->hide();
    disconnect(dateEdit, SIGNAL(dateChanged(const QDate &)),
               this, SLOT(setValue(const QDate &)));
    layout()->removeWidget(dateEdit);

    objLabel->hide();
    objLabel->disconnect();
    layout()->removeWidget(objLabel);

    objButton->hide();
    disconnect(objButton, SIGNAL(clicked()),
               this, SLOT(fieldSelect()));
    layout()->removeWidget(objButton);

    checkBox->hide();
    disconnect(checkBox, SIGNAL(valueChanged(const QString &)),
               this, SLOT(setValue(const QString &)));
    disconnect(checkBox, SIGNAL(toggled(bool)),
               checkBox, SLOT(on_toggled()));
    layout()->removeWidget(checkBox);

    if (!vFieldType.isEmpty())
    {
        QStringList parts = vFieldType.split(' ', QString::SkipEmptyParts);
        if (parts.count() > 1)
            n1 = parts[1].toInt();
        if (parts.count() > 2)
            n2 = parts[2].toInt();
    }

    switch (vEditorType)
    {
    case Numberic:
        lineEdit->setText(vValue);

        if (vFieldType.isEmpty())
        {
            QString rx = tr("^\\-{0,1}\\d{0,%1}\\.{1}\\d{0,%2}$").arg(3).arg(3);
            QRegExp rexp(rx);
            lineEdit->setValidator(new QRegExpValidator(rexp, lineEdit));
        }
        else
        {
            if (n2 == 0)
            {
                QString rx = tr("^\\-{0,1}\\d{0,%1}$").arg(n1);
                QRegExp rexp(rx);
                lineEdit->setValidator(new QRegExpValidator(rexp, lineEdit));
            }
            else
            {
                QString rx = tr("^\\-{0,1}\\d{0,%1}\\.{1}\\d{0,%2}$").arg(n1).arg(n2);
                QRegExp rexp(rx);
                lineEdit->setValidator(new QRegExpValidator(rexp, lineEdit));
            }
        }

        connect(lineEdit, SIGNAL(textChanged(const QString &)),
                this, SLOT(setValue(const QString &)));

        setFocusProxy(lineEdit);
        layout()->addWidget(lineEdit);
        lineEdit->show();
        break;

    case String:
        if (vFieldType.isEmpty())
            lineEdit->setMaxLength(20);
        else
            lineEdit->setMaxLength(n1);

        connect(lineEdit, SIGNAL(textChanged(const QString &)),
                this, SLOT(setValue(const QString &)));

        setFocusProxy(lineEdit);
        layout()->addWidget(lineEdit);
        lineEdit->show();
        break;

    case Date:
    case DateTime:
        connect(dateEdit, SIGNAL(dateChanged(const QDate &)),
                this, SLOT(setValue(const QDate &)));

        setFocusProxy(dateEdit);
        layout()->addWidget(dateEdit);
        dateEdit->show();
        break;

    case Catalogue:
        md_oid = n1;
        objLabel->setFrameShape(QFrame::Box);
        objLabel->setLineWidth(1);
        objLabel->setFocusPolicy(Qt::NoFocus);
        objButton->setMaximumWidth(25);
        objButton->setFocusPolicy(Qt::StrongFocus);

        connect(objButton, SIGNAL(clicked()),
                this, SLOT(fieldSelect()));

        setFocusProxy(objButton);
        layout()->addWidget(objLabel);
        layout()->addWidget(objButton);
        objLabel->show();
        objButton->show();
        break;

    case Document:
        md_oid = n1;
        objLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        objLabel->setLineWidth(1);
        objLabel->setFocusPolicy(Qt::NoFocus);
        objButton->setMaximumWidth(25);
        objButton->setFocusPolicy(Qt::StrongFocus);

        connect(objButton, SIGNAL(clicked()),
                this, SLOT(fieldSelect()));

        setFocusProxy(objButton);
        layout()->addWidget(objLabel);
        layout()->addWidget(objButton);
        objLabel->show();
        objButton->show();
        break;

    case Boolean:
        connect(checkBox, SIGNAL(valueChanged(const QString &)),
                this, SLOT(setValue(const QString &)));
        connect(checkBox, SIGNAL(toggled(bool)),
                checkBox, SLOT(on_toggled()));

        setFocusProxy(checkBox);
        layout()->addWidget(checkBox);
        checkBox->show();
        break;

    default:
        objLabel->setText("UnknownField");
        objLabel->setFrameShape(QFrame::Box);
        setFocusPolicy(Qt::NoFocus);
        layout()->addWidget(objLabel);
        objLabel->show();
        break;
    }

    setValue(vValue);
}



/*!
 * \en	Sets property fieldType. \_en
 * \ru 	Устанавливает значение свойства `fieldType'.\_ru
 */
void
wField::setFieldType( QString n )
{
    vFieldType=n;
    if (!loaded)
    {
	    loaded = true;
    }
    //widgetInit();
}


/*!
 * \en	Sets property editorType. \_en
 * \ru 	Устанавливает значение свойства `editorType'.\_ru
 */
void
wField::setEditorType( tEditorType n )
{
    vEditorType=n;
    widgetInit();
}



//<<<<<<< wfield.cpp

/*
void
wField::load()
{
}

*/

/*!
 * \en	Creates property editor dialog window. \_en
 * \ru 	Создает окно диалога редактора свойств.\_ru
 * 	\param parent - \en parent \_en \ru родитель \_ru
 */
QDialog *wField::createEditor(QWidget *parent)
{
    return new eField(parent, 0);
}


/*!
 * \en	Sets value type DateTime. \_en
 * \ru	Устанавливает значения типа DateTime.
 * 	Так как в базе хранится тип DateTime, значение дополняется
 * 	нулевым временем для корректной конвертации и хранении в базе.
 * 	Формат даты в базе данных должен быть ISO. \_ru
 * 	\param newDate (in) - \en date for set \_en \ru Дата для установки \_ru
 */
void
wField::setValue(const QDate& newDate)
{
//CHECK_POINT
	vValue = newDate.toString(Qt::ISODate);
	vValue+="T00:00:00"; //for correct converting to date-time
	emit valueChanged(vValue);
	emit valueChanged(QVariant(vValue));
}



/*!
 * \en 	Sets value. \_en
 * \ru	Устанавливает значение виджета. \_ru
 * 	\param newvalue (in) - \en value for set \_en \ru значение для установки \_ru
 */
void
wField::setValue(const QString &newvalue)
{
    int pos;
    QString  str;
    QDateTime dt;
//<<<<<<< wfield.cpp
//    if (!fieldEditor) return;
//    emit valueChanged(newvalue);
//    emit valueChanged(QVariant(newvalue));
///=======
//>>>>>>> 1.49.2.4
    switch (vEditorType)
    {
    case Unknown:
	break;
    case Numberic:
    case String:
	vValue = newvalue;
	pos = lineEdit->cursorPosition();
	lineEdit->setText(vValue);
	lineEdit->setCursorPosition(pos);
	break;
    case Date:
    case DateTime:
	// conventering date-time to date, and if new value is NULL set up current locale date.
	// date must be stored in server in format ISO
	 str = newvalue;
	 if(newvalue.isEmpty())
	 {
		str = QDateTime::currentDateTime().toString(Qt::ISODate);
	 }
	 dt = QDateTime::fromString(str,Qt::ISODate);
	 vValue = dt.toString(Qt::ISODate);
	 dateEdit->setDate(dt.date());
	 break;
    case Catalogue:
    case Document:
	vValue = newvalue;
	objLabel->setText( ananas_objectstr( db, vValue.toULongLong(), md_oid ) );
	break;
    case Boolean:
	vValue = newvalue;
	if(newvalue == "1")
	{
		checkBox->setChecked(true);
	}
	else
	{
		checkBox->setChecked(false);
	}
	break;
	//emit valueChanged(QVariant(vValue));

    }
    emit valueChanged(vValue);
    emit valueChanged(QVariant(vValue));
}



/*!
 * \en	Gets value. \_en
 * \ru	Возвращает значение виджета. \_ru
 * 	\return \en value \_en \ru значение виджета \_ru
 */
QString
wField::value() const
{
   QString str = vValue;
   return  str; //str.setUnicode(vValue.unicode(),vValue.length());
}



/*!
 * \en	Gets text value. \_en
 * \ru	Возвращает текстовое значение виджета. \_ru
 * 	\return \en value \_en \ru значение виджета \_ru
 */
QString
wField::text() const
{
   QString str;
	switch (vEditorType)
	{
	case Date:
	case DateTime:
		str = dateEdit->date().toString(Qt::ISODate);
		break;
	case String:
	case Numberic:
		str = lineEdit->text();
		break;
	case Catalogue:
	case Document:
		str = objLabel->text();
		break;
	case Boolean:
		str = checkBox->isChecked()?"1":"0";
		break;
	default:
		str = "";
		break;
	}
	return  str; //str.setUnicode(vValue.unicode(),vValue.length());
}

/*!
 * \en	Calls handler onClick event for Catalogue or Document object. \_en
 * \ru	Вызывает обработчик события onClick для объектов Каталог или Документ. \_ru
 */
void wField::fieldSelect()
{
    aForm *f = 0;

    switch (vEditorType)
    {
        case Catalogue:
            if (engine)
            {
                int fid = md->getDefaultFormId(md->find(md_oid), md_action_view);
                if (!fid)
                {
                    engine->openEmbedCatalogueEditor(md_oid, this, true);
                    return;
                }

                f = engine->openForm(md_oid, 0, md_action_view, 0, false);
                if (f)
                {
                    connect(f, SIGNAL(selected(qulonglong)),
                            this, SLOT(on_selected(qulonglong)));
                    f->closeAfterSelect = true;
                }
            }
            else
            {
                printf("No Engine\n");
            }
            break;

        case Document:
            if (engine)
            {
                aCfgItem journ = md->findJournal(1, md->find(md_oid));
                if (journ.isNull())
                {
                    printf("special journal not found, find system journal\n");
                    journ = md->findJournal(0, md->find(md_oid));
                }
                else
                {
                    printf("found special journal %s with class %s\n",
                           qPrintable(md->attr(journ, mda_name)),
                           qPrintable(md->objClass(journ)));
                }

                f = engine->openForm(md->id(journ), 0, md_action_view, 0, false);
                if (f)
                {
                    connect(f, SIGNAL(selected(qulonglong)),
                            this, SLOT(on_selected(qulonglong)));
                    f->closeAfterSelect = true;
                }
            }
            else
            {
                printf("No engine\n");
            }
            break;

        default:
            break;
    }
}




/*!
 * \en	Handler signal lostFocus. \_en
 * \ru	Обработчик сигнала lostFocus. \ru
 */
void
wField::focusInEvent()
{
	switch (vEditorType) {
	case Catalogue:
		objLabel->setLineWidth( 2 );
		break;
	case Document:
		break;
	default:
		break;
    }
//emit(lostFocus());
}


/*!
 * \en	Handler signal lostFocus. \_en
 * \ru	Обработчик сигнала lostFocus. \ru
 */
void
wField::focusOutEvent()
{
	switch (vEditorType) {
	case Catalogue:
		objLabel->setLineWidth( 1 );
		break;
	case Document:
		break;
	default:
		break;
    }
//emit(lostFocus());
}


void
wField::on_selected( qulonglong uid )
{
	setValue( QString::number( uid ) );
	setFocus();
}

void
wField::selectAll()
{
	switch (vEditorType) {
	case Numberic:
	case String:
//		((QLineEdit *) fieldEditor)->home( false );
		lineEdit->selectAll();
		break;
	default:
		break;
    }
}

void
wField::SetReadOnly(bool fl)
{
	switch (vEditorType) {
	case Numberic:
	case String:
		lineEdit->setReadOnly(fl);
		break;
	case Date:
	case DateTime:
		dateEdit->setDisabled(fl);
		break;
	case Catalogue:
		objButton->setDisabled( fl );
		break;
	case Document:
		objButton->setDisabled( fl );
		break;
	case Boolean:
		checkBox->setDisabled( fl );
		break;
	default:
		break;
    }

}

/*<<<<<<< wfield.cpp
void
wField::SetReadOnly(bool fl)
{
	switch (vEditorType) {
	case Numberic:
	case String:
//		((QLineEdit *) fieldEditor)->home( false );
		((QLineEdit *) fieldEditor)->setReadOnly(fl);
		break;
	case Date:
		((QDateEdit*)fieldEditor)->setDisabled(fl);
		break;
	case DateTime:
		((QDateTimeEdit*)fieldEditor)->setDisabled(fl);
		break;
	case Catalogue:
//		((QLabel *) fieldEditor)->setFrameShape( QFrame::NoFrame );
		((wCatButton *) fieldSelector)->setDisabled( fl );
		break;
	case Document:
		break;
	default:
		break;
    }

}

=======

>>>>>>> 1.49.2.4
*/
/*!
 *\~english
 *	Proces value changed.
 *	Proces value changed in fields diferent types.
 *\~russian
 *	Обрабатывает изменение значения.
 *	Обрабатывает изменение значения в поле, вне зависимости от типа поля.
 *\~
 */

wCatButton::wCatButton(const char * caption, QWidget *parent):QPushButton(caption,parent)
{
}

wCatButton::~wCatButton()
{
}

void
wCatButton::keyPressEvent ( QKeyEvent * e )
{
	switch(e->key())
	{
		case Qt::Key_Left:
		case Qt::Key_Up:
		case Qt::Key_Right:
		case Qt::Key_Down:
			e->accept();
			break;
		case Qt::Key_Space:
			e->accept();
			emit( clicked() );
			break;
		case Qt::Key_Tab:
//			printf("press tab\n");
			e->accept();
//<<<<<<< wfield.cpp
			//new QKeyEvent( QEvent::KeyPress, Qt::Key_Return, 10, 0);
			//new QKeyEvent( QEvent::KeyRelease, Qt::Key_Return, 10, 0);
//=======
//>>>>>>> 1.49.2.4
			break;
		default:
			e->ignore();
			break;

	}
}

wCheckBox::wCheckBox(QWidget *parent, const char *name) : QCheckBox(parent)
{
    if (name)
        setObjectName(name);

    connect(this, SIGNAL(toggled(bool)), this, SLOT(on_toggled()));
}

wCheckBox::~wCheckBox()
{
}

void
wCheckBox::on_toggled()
{
//	printf("wCheckBox state is %d\n",this->state());
	if(this->isChecked()) emit(valueChanged("1"));
	else emit(valueChanged("0"));
}
