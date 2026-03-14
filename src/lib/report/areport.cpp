/****************************************************************************
** $Id: areport.cpp,v 1.2 2008/11/08 20:16:36 leader Exp $
**
** Report metadata object implementation file of
** Ananas application library
**
** Created : 20040701
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
** Copyright (C) 2003-2006 Grigory Panov <gr1313 at mail.ru>, Yoshkar-Ola.
**
** This file is part of the Designer application of the Ananas
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


#include	"acfg.h"
#include	"aobject.h"
#include	"adatabase.h"
#include	"areport.h"
#include	"acalctemplate.h"
#include 	"aservice.h"
#include 	"alog.h"
#include 	"stdlib.h"
/*!
*	\en
*	Constuct object
*	\_en
*	\ru
*	Создает объект
*	\_ru
*/
aReportBrowser::aReportBrowser(QWidget *parent, const char *name, Qt::WindowFlags f) : QMainWindow(parent, f)
{
    setObjectName(name ? QString::fromLocal8Bit(name) : QString());

    QAction *a;

    QToolBar *t = new QToolBar(tr("ReportTool"), this);
    addToolBar(t);

    a = new QAction(
        QPixmap(":/images/print.png"),
        tr("Print"),
        this
    );
    a->setShortcut(QKeySequence("Ctrl+P"));
    a->setStatusTip(tr("Print report"));

    t->addAction(a);
    connect(a, SIGNAL(triggered()), this, SLOT(print()));

    textBrowser = new QTextBrowser(this);
    textBrowser->setFocus();

    setCentralWidget(textBrowser);

    languageChange();
}



/*!
*	\en
*	Appends text to browser.
*	\_en
*	\ru
*	Добавляет текст в браузер
*	\_ru
*	\param text - \en text to append \_en \ru текст для добавления \_ru
*/
void
aReportBrowser::append( const QString &text )
{
	textBrowser->append( text );
}



/*!
*	\en
*	Clears browser.
*	\_en
*	\ru
*	Очищает браузер
*	\_ru
*/
void
aReportBrowser::clear()
{
	textBrowser->clear();
}



/*!
*	\en
*	Print browser content.
*	\_en
*	\ru
*	Печатает содержимое браузера.
*	\_ru
*/
void aReportBrowser::print()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QTextDocument doc;
    doc.setHtml(textBrowser->toHtml());
    doc.print(&printer);
}



/**!
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void aReportBrowser::languageChange()
{
}



/*!
*	\en
*	Constuct object
*	\_en
*	\ru
*	Создает объект
*	\param context - объект метаданных для инициализации.
*	\param report_type - тип отчета. RT_text - для отчета в формате HTML, RT_office - для отчета в формате OpenOffice
*	\param e - енжин.
*	\_ru
*/
aReport::aReport( aCfgItem context, RT_type report_type, aEngine * e )
:aObject( context, 0, 0, "aReport")
{
	engine = e;
	md = e->md;
	browser = new aReportBrowser( e->ws );
	type = report_type;
	if(report_type==RT_text)
	{
		tpl = new aTemplate();
	}
	if(report_type == RT_office_writer)
	{
		tpl = new aOOTemplate();
	}
	if(report_type==RT_office_calc)
	{
		tpl = new aCalcTemplate();
	}
}



/*!
*	\en
*	Constuct object
*	\_en
*	\ru
*	Создает объект
*	\param name - имя объекта.
*	\param report_type - тип отчета. RT_text - для отчета в формате HTML, RT_office - для отчета в формате OpenOffice
*	\param e - енжин.
*	\_ru
*/
aReport::aReport(QString name, RT_type report_type, aEngine * e )
:aObject( name, 0, 0, "aReport")
{
	engine = e;
	md = e->md;
	browser = new aReportBrowser( e->ws );
	type = report_type;
	if(report_type==RT_text)
	{
		tpl = new aTemplate();
	}
	if(report_type == RT_office_writer)
	{
		tpl = new aOOTemplate();
	}
	if(report_type == RT_office_calc)
	{
		tpl = new aCalcTemplate();
	}
}



/*!
*	\en
*	Destructor
*	\_en
*	\ru
*	Деструктор
*	\_ru
*/
aReport::~aReport()
{
	if(tpl)
	{
		delete tpl;
		tpl=0;
	}
}



/*!
*	\en
*	Init object.
*	\_en
*	\ru
*	Инициализирует объект. Объект не содержит никаких таблиц в базе данных.
*	\_ru
*/
ERR_Code
aReport::initObject()
{
	ERR_Code err = aObject::initObject();
	if ( err ) return err;
	return err_noerror;
}



/*!
*	\en
*	Set template.
*	\_en
*	\ru
*	Задает имя файла шаблона. Расширение указывать необходимо.
*	\_ru
*	\param name - \en template name \_en \ru имя шаблона \_ru
*	\see path2workdir()
*/
void
aReport::setTable( const QString &name )
{
	tpl->setDir(path2workdir());
	tpl->open( name );
	tplName = name;
}



/*!
*	\en
*	Sets tag value.
*	\_en
*	\ru
*	Задает значение тега.
*	\_ru
*	\param name - \en tag name \_en \ru имя тега \_ru
*	\param value - \en tag value \_en \ru значение тега \_ru
*/
void
aReport::setValue( const QString &name, const QString &value )
{
	tpl->setValue( name, value );
}



/*!
*	\en
*	Gets tag value.
*	\_en
*	\ru
*	Возвращает значение тега.
*	\_ru
*	\param name - \en tag name \_en \ru имя тега \_ru
*	\return \en tag value \_en \ru значение тега \_ru
*/
QString
aReport::value( const QString &name )
{
	return tpl->getValue( name );
}



/*!
*	\en
*	Clear browser content.
*	\_en
*	\ru
*	Очищает содержимое браузра. Используется для текстово отчета.
*	\_ru
*/
void
aReport::clear()
{
	browser->clear();
}



/*!
*	\en
*	Update report value.
*	\_en
*	\ru
*	Обновляет репорт на основе заданных перед этим функцией setValue() значений.
*	\_ru
*/
void
aReport::exec( const QString &section )
{
	tpl->exec( section );
}



/*!
*	\en
*	Shows report.
*	\_en
*	\ru
*	Показывает отчет.
*	\_ru
*/
void aReport::show()
{
    if (type == RT_text)
    {
        browser->textBrowser->setText(tpl->result());
        browser->show();
    }

    if (type == RT_office_writer || type == RT_office_calc)
    {
        QString fileName = getName4NewTemplate();
        tpl->cleanUpTags();
        tpl->save(fileName);

        QString startCatalog = "/usr/bin";
        QString filter;
        bool ok = false;
        QString oowriter = aService::readConfigVariable("OpenOfficeExecutable", &ok);

        if (!ok || oowriter.isEmpty())
        {
#ifdef Q_OS_WIN32
            aLog::print(aLog::Info, tr("aReport OpenOfficeExecutable not found in registery"));
            startCatalog = QString("%1\\Program Files\\OpenOffice.org1.1.1\\program").arg(qgetenv("HOMEDRIVE").constData());
            QFile soffice(startCatalog + "\\soffice.exe");

            if (!soffice.exists())
            {
                startCatalog = QString("%1\\Program Files\\OpenOffice.org 2.0\\program").arg(qgetenv("HOMEDRIVE").constData());
                soffice.setFileName(startCatalog + "\\soffice.exe");
                if (!soffice.exists())
                {
                    startCatalog = QString("%1\\Program Files").arg(qgetenv("HOMEDRIVE").constData());
                }
                else
                {
                    aService::writeConfigVariable("OpenOfficeExecutable", startCatalog + "\\soffice.exe");
                    aLog::print(aLog::Info, tr("aReport OpenOfficeExecutable found %1").arg(startCatalog + "\\soffice.exe"));
                }
            }
            else
            {
                aService::writeConfigVariable("OpenOfficeExecutable", startCatalog + "\\soffice.exe");
                aLog::print(aLog::Info, tr("aReport OpenOfficeExecutable found %1").arg(startCatalog + "\\soffice.exe"));
            }
            filter = "Executable files (*.exe)";
#else
            aLog::print(aLog::Info, tr("aReport OpenOfficeExecutable not found in ~/.ananas/configrc"));
            QFile soffice(startCatalog + "/ooffice");
            if (!soffice.exists())
            {
                soffice.setFileName(startCatalog + "/ooffice2");
                if (soffice.exists())
                {
                    aService::writeConfigVariable("OpenOfficeExecutable", startCatalog + "/ooffice2");
                    aLog::print(aLog::Info, tr("aReport OpenOfficeExecutable found %1").arg(startCatalog + "/ooffice2"));
                }
            }
            else
            {
                aService::writeConfigVariable("OpenOfficeExecutable", startCatalog + "/ooffice");
                aLog::print(aLog::Info, tr("aReport OpenOfficeExecutable found %1").arg(startCatalog + "/ooffice"));
            }
#endif
        }

        oowriter = aService::readConfigVariable("OpenOfficeExecutable", &ok);

        if (!ok || oowriter.isEmpty())
        {
            oowriter = QFileDialog::getOpenFileName(
                0,
                QString::fromUtf8("Для отображения отчета необходим OpenOffice. Укажите исполняемый файл OpenOffice"),
                startCatalog,
                filter);

            if (!oowriter.isEmpty())
            {
                bool started = QProcess::startDetached(
                    oowriter,
                    QStringList() << "-n" << QDir::toNativeSeparators(fileName));

                if (!started)
                {
                    QMessageBox::warning(
                        0,
                        tr("Warning"),
                        tr("Unable to start OpenOffice (%1)").arg(oowriter),
                        QMessageBox::Ok,
                        QMessageBox::NoButton);
                }
                else
                {
                    aService::writeConfigVariable("OpenOfficeExecutable", oowriter);
                    aLog::print(aLog::Info, tr("aReport OpenOfficeExecutable set to %1").arg(oowriter));
                }
            }
        }
        else
        {
            bool started = QProcess::startDetached(
                oowriter,
                QStringList() << "-n" << QDir::toNativeSeparators(fileName));

            if (!started)
            {
                QMessageBox::warning(
                    0,
                    tr("Warning"),
                    tr("Unable to start OpenOffice (%1)").arg(oowriter),
                    QMessageBox::Ok,
                    QMessageBox::NoButton);
            }
            else
            {
                aService::writeConfigVariable("OpenOfficeExecutable", oowriter);
                aLog::print(aLog::Info, tr("aReport OpenOfficeExecutable set to %1").arg(oowriter));
            }
        }
    }
}



/*!
*	\en
*	Return path to working dir.
*	\_en
*	\ru
*	Возвращает путь к рабочей директории.
*	\_ru
* 	\see setTable( const QString &name )
*/
QString
aReport::path2workdir()
{
	QString res;
#ifndef Q_OS_WIN32
	res = "/usr/share/ananas/";
#else
	res = "";
#endif
	if(md!=NULL)
	{
		res = md->rc.value("workdir");
	}
	aLog::print(aLog::Debug, tr("aReport working dir = %1").arg(res));
	return res;
}



/*!
*	\en
*	Close report.
*	\_en
*	\ru
*	Закрывает отчет. Удаляет временные файлы.
*	\_ru
*/
void
aReport::close()
{
	tpl->close();
}


/*!
*	\en
*	Gets name for new template. Need for OpenOffice v2.
*	\_en
*	\ru
*	Получение имени для нового шаблона. Нужна из-за блокировок в OpenOffice v2.
*	\_ru
*/
QString aReport::getName4NewTemplate()
{
    uint count = 0;
    QFile tmpf;
    QString suff = ".odt";
    QString fname;

    if (type == RT_office_calc) suff = ".ods";

    do
    {
        fname = QDir::toNativeSeparators(
            QString("%1.ananas-report%2%3")
                .arg(tpl->getDir())
                .arg(count)
                .arg(suff));

        tmpf.setFileName(fname);

        if (tmpf.exists())
        {
            if (tmpf.remove()) break;
            else ++count;
        }
        else
        {
            break;
        }

    } while (count < 100);

    aLog::print(aLog::Debug, tr("aReport name for template = %1").arg(fname));
    return fname;
}
