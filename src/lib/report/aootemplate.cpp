/****************************************************************************
** $Id: aootemplate.cpp,v 1.2 2008/11/08 20:16:36 leader Exp $
**
** Report metadata object header file of
** Ananas application library
**
** Created : 20050202
**
** Copyright (C) 2003 by HaPK, email : theHaPK@yandex.ru
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

#include "aootemplate.h"
#include "alog.h"


aOOTemplate::aOOTemplate() : iTemplate()
{

}

aOOTemplate::~aOOTemplate()
{
}

/**
 * \en
 * Open pattern.
 * \_en
 * \ru
 * Открывает шаблон. Распаковывает его во временный каталог в директории /tmp
 * \_ru
 * \param fname \ru имя шаблона\_ru
 */
bool aOOTemplate::open(const QString &fname)
{
    QDir dir;
    QString temp;

#ifndef Q_OS_WIN32
    temp = qgetenv("TMPDIR");
    if (temp.isEmpty())
        temp = P_tmpdir;
#else
    temp = qgetenv("TEMP");
#endif

    copyName = QString("%1/%2").arg(temp).arg(QDateTime::currentDateTime().toTime_t());
    copyName = QDir::toNativeSeparators(copyName);

    aLog::print(aLog::Debug, tr("aOOTemplate temporary directory is %1").arg(copyName));

    if (!dir.mkdir(copyName))
    {
        aLog::print(aLog::Error, tr("aOOTemplate create temporary directory %1").arg(copyName));
        return false;
    }
    else
    {
        aLog::print(aLog::Debug, tr("aOOTemplate create temporary directory %1").arg(copyName));
    }

    QProcess process;
    process.setWorkingDirectory(templateDir);

#ifndef Q_OS_WIN32
    QString program = "unzip";
    QStringList arguments;
    arguments << fname << "-d" << copyName;
#else
    QString program = "7z";
    QStringList arguments;
    arguments << "x" << "-y" << QString("-o%1").arg(copyName) << fname;
#endif

    process.start(program, arguments);

    if (!process.waitForStarted())
    {
        aLog::print(aLog::Error, tr("aOOTemplate start unzip"));
        return false;
    }

    if (!process.waitForFinished(-1))
    {
        aLog::print(aLog::Error, tr("aOOTemplate unzip dead"));
        return false;
    }
    else
    {
        aLog::print(aLog::Debug, tr("aOOTemplate unzip normal"));
    }

    if (process.exitCode() != 0)
    {
        return false;
    }

    QFile content(QDir::toNativeSeparators(copyName + "/content.xml"));
    if (!content.open(QIODevice::ReadOnly) || !docTpl.setContent(&content, false))
    {
        aLog::print(aLog::Error, tr("aOOTemplate read content.xml"));
        return false;
    }
    content.close();

    QFile style(QDir::toNativeSeparators(copyName + "/styles.xml"));
    if (!style.open(QIODevice::ReadOnly) || !docStyle.setContent(&style, false))
    {
        aLog::print(aLog::Error, tr("aOOTemplate read styles.xml"));
        return false;
    }
    style.close();

    aLog::print(aLog::Info, tr("aOOTemplate open"));
    return true;
}

/**
 * \en
 * Closes pattern. Deletes temporary files.
 * \_en
 *
 * \ru
 * Закрывает шаблон. Удаляет временные файлы.
 * \_ru
 */
void aOOTemplate::close()
{
    values.clear();
    docTpl.clear();
    docStyle.clear();

#ifndef Q_OS_WIN32
    QProcess::execute("rm", QStringList() << "-Rf" << copyName);
#else
    QProcess::execute("cmd", QStringList() << "/C" << "rd" << "/S" << "/Q" << copyName);
#endif
}

/**
 * \en
 * clears internal variable.
 * \_en
 * \ru
 * Обнуляет внутренние переменные.
 * \_ru
 */
void
aOOTemplate::clear()
{
	values.clear();
//	cleanUpTags();
}
/**
 * \en
 * Returns value parameter `name`
 * \_en
 * \ru
 * Возвращает значение параметра по его имени.
 * \_ru
 */
QString aOOTemplate::getValue(const QString &name)
{
    if (values.contains(name))
    {
        return values.value(name);
    }
    else
    {
        aLog::print(aLog::Info, tr("aOOTemplate value for field %1 not setted").arg(name));
        return QString();
    }
}

/**
 * \en
 * Sets value parameter `name'
 * \_en
 * \ru
 * Устанавливает значение параметра по его имени.
 * \_ru
 */
void
aOOTemplate::setValue( const QString &name, const QString &value )
{
	values[name]=value;
}
/**
 * \en
 * Execute replace tags to values.
 * \_en
 * \ru
 * Выполняет подстановку значения параметра в шаблоне.
 * Есть 2 типа тегов
 *  \arg обычные теги
 *  \arg секции - могут находиться ТОЛЬКО в строках таблицы.

 * Для подстановки значений обычных тегов необходимо выполнить setValue() где name = PARAM (сейчас #define PARAM "param") а value - значение для подстановки. Потом выполнить exec() с параметром = имени тега.
 * Для подстановки секций необходимо задать нужные параметры, используя setValue()
 * а потом выполнить exec() с именем секции.
 * exec может вызываться нужное число раз как для обычных тегов, так и для секций
 * \_ru
 */
QString
aOOTemplate::exec( const QString &sname )
{
	setValue(sname, getValue(PARAM));
	//search tags in content
	QDomNode n = docTpl.lastChild();
	while( !n.isNull() )
	{
		searchTags(n, sname);
		n = n.previousSibling();
	}
	//search tags in colontituls
	n = docStyle.lastChild();
	while( !n.isNull() )
	{
		searchTags(n, sname);
		n = n.previousSibling();
	}
	return docTpl.toString();

}
/**
 *	\~english
 *	Added value to end tag with name `sname'. Don't deletes tag.
 *	\~russian
 *	Рекурсивная функция поиска и замены тегов на их значения в node.
 *	Не заменяет теги, а дописывает значения в конец.
 *	Для удаления тэгов используйте функцию cleanUpTags()
 * 	\~
 *	\param node - \~english context for searching \~russian узел, с которого осуществляется поиск. \~
 *	\param sname - \~english tag name \~russian имя тега для поиска \~
 *	\see cleanUpTags() getNodeTags() insertRowValues()
 */
void
aOOTemplate::searchTags(QDomNode node, const QString &sname )
{
QDomNode n = node.lastChild();
	while( !n.isNull() )
	{

	//	printf("n->name=%s\n",n.nodeName().ascii());
		bool res = getNodeTags(n, sname, false);
		if( res )
		{
			insertRowValues(n);
		}
		else
		{
			res = getNodeTags(n, sname, true);
			if(res)
			{
				insertTagsValues(n, sname);
			}
			else
			{
				searchTags(n, sname);
			}
		}
		n = n.previousSibling();
	}
}
/**
 *	\~english
 *	Return true, if node contain tag
 *	\~russian
 *	Возвращает истину, когда текст ноды содержит тег с заданным именем.
 *	\~
 *	\param node - \~english context for searching \~russian узел, с которого осуществляется поиск. \~
 *	\param sname - \~english tag name \~russian имя тега для поиска \~
 *	\param params - \~english true, if find simple tag and false, if section
 *			\~russian true, если ищется обычный тег и false, если ищется тег секции \~
 */
bool aOOTemplate::getNodeTags(QDomNode node, const QString &tagname, bool params)
{
    if (node.isText())
    {
        QString str = node.nodeValue();
        QRegExp re;

        if (params)
        {
            re.setPattern(QString("%1.*%2").arg(open_token).arg(close_token));
        }
        else
        {
            re.setPattern(QString("%1.*%2").arg(open_token_section).arg(close_token_section));
        }

        re.setMinimal(true);
        int pos = re.indexIn(str, 0);

        while (pos != -1)
        {
            if (tagname == str.mid(pos + 2, re.matchedLength() - 4))
            {
                return true;
            }
            pos += re.matchedLength();
            pos = re.indexIn(str, pos);
        }
    }
    return false;
}

/**
 *	\~english
 *	insert new row in table and replace tag to value
 *	\~russian
 *	Вставляет новую строку в таблицу, заменяет теги на значения, удаляет тег секции из строки таблицы.
 *	Выполняет рекурсивный поиск узла, содержащего строку таблицы. У этого узла в OpenOffic'е есть
 *	специальное имя, которое распознается функцией. После того, как узел найден, строка строка дублируется,
 *	а из текущей строки удаляются все теги секции, чтобы избежать мнократного размножения строк таблицы.
 *	\~
 *	\see searchTags()
 */
void
aOOTemplate::insertRowValues(QDomNode node)
{
	QDomNode n = node;
	while(!n.parentNode().isNull())
	{
		n = n.parentNode();
		if( n.nodeName()=="table:table-row" )
		{
			n.parentNode().insertAfter(n.cloneNode(true),n);
			clearTags(n,true);
			QMap<QString,QString>::Iterator it;
			for ( it = values.begin(); it != values.end(); ++it )
			{
				searchTags(n,it.key());
			}

		}
	}
}


 /**
 *	\~english
 *	Addes to tag value of parametr \a tagName
 *	\~russian
 *	Добавляет к тегу значение параметра \a tagName. После вызова этой функции тег не исчезает,
 *	и к нему можно добавить еще значения, которые добавятся к концу текста, содержащего тег.
 *	\~
 *	\param node - \~english context \~russian узел к которому добавляется значение \~
 *	\param sname - \~english tag name \~russian имя тега \~
 */
void
aOOTemplate::insertTagsValues(QDomNode node, const QString &tagName)
{
	QDomNode n = node;
	n.setNodeValue(n.nodeValue()+getValue(tagName));
}

/**
 * \en
 * clears all tags from pattern
 * \_en
 * \ru
 * Удаляет все теги из документа, а также строки, в которых содержится тег секции
 * \_ru
 */
void
aOOTemplate::cleanUpTags()
{

	//clear tags in content
	QDomNode n = docTpl.lastChild();
	while( !n.isNull() )
	{
		clearTags(n,false);
	//	clearRow(n);
		n = n.previousSibling();
	}
	n = docTpl.lastChild();
	while( !n.isNull() )
	{
	//	clearTags(n,false);
		clearRow(n);
		n = n.previousSibling();
	}

	//clear tags in colontituls
	n = docStyle.lastChild();
	while( !n.isNull() )
	{
		clearTags(n,false);
	//	clearRow(n);
		n = n.previousSibling();
	}
	n = docStyle.lastChild();
	while( !n.isNull() )
	{
	//	clearTags(n,false);
		clearRow(n);
		n = n.previousSibling();
	}
}



 /**
 *	\~english
 *	Deletes tags from \a node
 *	\~russian
 *	Удаляет рекурсивно теги из \a node.
 *	\~
 *	\param node - \~english context \~russian узел из которого нужно удалить теги \~
 * 	\param section - \~russian true, если надо удалить тег секции \~
 */
void aOOTemplate::clearTags(QDomNode node, bool section)
{
    if (node.isNull()) return;

    QDomNode n = node.lastChild();
    while (!n.isNull())
    {
        if (n.isText())
        {
            QString str = n.nodeValue();
            QRegExp re;

            if (section)
            {
                re.setPattern(QString("%1.*%2").arg(open_token_section).arg(close_token_section));
            }
            else
            {
                re.setPattern(QString("%1.*%2").arg(open_token).arg(close_token));
            }

            re.setMinimal(true);
            int pos = re.indexIn(str, 0);

            while (pos != -1)
            {
                str.remove(re);
                pos = re.indexIn(str, 0);
            }

            n.setNodeValue(str);
        }
        else
        {
            clearTags(n, section);
        }
        n = n.previousSibling();
    }
}

/**
 *	\~english
 *	Deletes row, having section tag
 *	\~russian
 *	Рекурсивная функция. Удаляет строки, содержащие тег секции
 *	\~
*	\param node - \~english context \~russian узел из которого нужно удалить строки \~
 */
void aOOTemplate::clearRow(QDomNode node)
{
    QDomNode n = node.lastChild();
    while (!n.isNull())
    {
        if (n.isText())
        {
            QString str = n.nodeValue();
            QRegExp re;
            re.setPattern(QString("%1.*%2").arg(open_token_section).arg(close_token_section));
            re.setMinimal(true);
            int pos = re.indexIn(str, 0);
            if (pos != -1)
            {
                QDomNode tmp = n;
                while (!tmp.parentNode().isNull())
                {
                    tmp = tmp.parentNode();
                    if (tmp.nodeName() == "table:table-row")
                    {
                        tmp.parentNode().removeChild(tmp);
                        break;
                    }
                }
            }
        }
        else
        {
            clearRow(n);
        }
        n = n.previousSibling();
    }
}

/**
 * \en
 * Returns patten
 * \_en
 * \ru
 * Возвращает результат, т.е. шаблон в текстовом виде. Включена в интерфейс временно.
 * \_ru
 */
QString
aOOTemplate::result()
{
	return docTpl.toString();
}
/**
 * \en
 * Save pattern
 * \_en
 * \ru
 * Сохраняет шаблон в файл с заданным именем. Перед сохранением необходимо выполнить функцию cleanUpTags() чтобы удалить тэги из сохраняемого документа.
 * \_ru
 */
bool aOOTemplate::save(const QString &fname)
{
    QFile fContent(QDir::toNativeSeparators(copyName + "/content.xml"));
    if (!fContent.open(QIODevice::WriteOnly))
    {
        aLog::print(aLog::Error, tr("aOOTemplate save %1 open for write").arg(fContent.fileName()));
        return false;
    }
    QTextStream stream4content(&fContent);
    docTpl.save(stream4content, 2);
    fContent.close();

    QFile fStyle(QDir::toNativeSeparators(copyName + "/styles.xml"));
    if (!fStyle.open(QIODevice::WriteOnly))
    {
        aLog::print(aLog::Error, tr("aOOTemplate save %1 open for write").arg(fStyle.fileName()));
        return false;
    }
    QTextStream stream4styles(&fStyle);
    docStyle.save(stream4styles, 2);
    fStyle.close();

    QString curDir = QDir::toNativeSeparators(templateDir);
    aLog::print(aLog::Debug, tr("aOOTemplate save working dir =%1").arg(curDir));

    QProcess process;
    process.setWorkingDirectory(copyName);

#ifndef Q_OS_WIN32
    QString program = "zip";
    QStringList arguments;
    arguments << "-r" << fname << ".";
#else
    QString program = "7z";
    QStringList arguments;
    arguments << "a" << "-tzip" << fname << "-r" << ".";
#endif

    process.start(program, arguments);

    if (!process.waitForStarted())
    {
        aLog::print(aLog::Error, tr("aOOTemplate start zip"));
        return false;
    }

    if (!process.waitForFinished(-1))
    {
        aLog::print(aLog::Error, tr("aOOTemplate zip dead"));
        return false;
    }
    else
    {
        aLog::print(aLog::Debug, tr("aOOTemplate zip normal to %1").arg(fname));
    }

    if (process.exitCode() != 0)
    {
        return false;
    }

    return true;
}

void
aOOTemplate::setDir(const QString &dir)
{
	templateDir = dir;
}

QString aOOTemplate::getDir()
{
    QString homeDir = QDir::homePath();

#ifdef Q_OS_WIN32
    if (!homeDir.endsWith("\\")) homeDir.append("\\");
#else
    if (!homeDir.endsWith("/")) homeDir.append("/");
#endif

    return QDir::toNativeSeparators(homeDir);
}

