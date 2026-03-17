/****************************************************************************
** $Id: main.cpp,v 1.7 2009/05/22 08:03:55 app Exp $
**
** Main file of Ananas Designer application
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
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

#include <aapplication.h>

#include <QTranslator>
#include <QSplashScreen>
#include <QTextCodec>
#include <QPixmap>
#include <QMdiArea>

#include "alog.h"
#include "ananas.h"
#include "mainform.h"
#include "dselectdb.h"
#include "formdesigner.h"

MainForm *mainform = 0;
aWindowsList *mainformwl = 0;
QMdiArea *mainformws = 0;
// QApplication *application = 0;
QTranslator *translator = 0, tr_app(0), tr_lib(0), tr_plugins(0);
aFormDesigner *formdesigner = 0;
QString lang = "en",
        rcfile = "",
        username = "",
        userpassword = "";


int setTranslator(QString langdir, QString lang)
{
    tr_app.load(langdir + "ananas-designer-" + lang + ".qm", ".");
    tr_lib.load(langdir + "ananas-lib-" + lang + ".qm", ".");
    tr_plugins.load(langdir + "ananas-plugins-" + lang + ".qm", ".");
    return 0;
}


int parseCommandLine(AApplication *a)
{
    QString param, name, value;
    int i, argc;
    char **argv;

    argc = a->argc();
    argv = a->argv();

    setTranslator(a->langDir(), a->lang());

    QString str_ru, str_en;
    bool help_setted = false;

    for (i = 1; i < argc; i++)
    {
        param = QString::fromLocal8Bit(argv[i]);
        name = param.section("=", 0, 0).toLower();
        value = param.section("=", 1);

        if (param == "--help")
        {
            str_ru = QString::fromUtf8("Использование: ananas-designer [--help] [--lang=<LANG>] [--rc=<RC_PATH>]\n");
            str_ru += QString::fromUtf8("LANG=ru|en\n");
            str_ru += QString::fromUtf8("RC_PATH=путь к *.rc файлу описания бизнес схемы\n");

            str_en = "Usage: ananas-designer [--help] [--lang=<LANG>] [--rc=<RC_PATH>]\n";
            str_en += "LANG=ru|en\n";
            str_en += "RC_PATH=path to *.rc file of paticular business scheme\n";
            help_setted = true;
        }

        if (name == "--lang") {
            lang = value;
            setTranslator(a->langDir(), lang);
        }

        if (name == "--rc")
            rcfile = value;
    }

    if (help_setted)
    {
        if (lang == "ru")
            printf("%s", str_ru.toLocal8Bit().constData());
        else
            printf("%s", str_en.toLocal8Bit().constData());

        return 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    AApplication app(argc, argv, AApplication::Designer);
    MainForm *w = new MainForm(0, Qt::WindowFlags());

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    app.setOrganizationName("ananasgroup");
    app.setApplicationName("ananas");

    mainform = w;
    mainformws = mainform->ws;
    mainformwl = mainform->wl;

    int rc;
    QPixmap pixmap;

    if (parseCommandLine(&app))
        return 1;

    qApp->installTranslator(&tr_lib);
    qApp->installTranslator(&tr_plugins);
    qApp->installTranslator(&tr_app);

    pixmap = QPixmap(":/images/designer-splash-" + lang + ".png");
    if (pixmap.isNull()) {
#ifdef Q_OS_WIN32
        pixmap = QPixmap(qApp->applicationDirPath() + "/designer-splash-" + lang + ".png");
        qApp->addLibraryPath(qApp->applicationDirPath());
#else
        pixmap = QPixmap("/usr/share/ananas4/designer/locale/designer-splash-" + lang + ".png");
#endif
    }

    foreach (QString path, app.libraryPaths())
        printf("%s\n", path.toUtf8().constData());

    if (pixmap.isNull())
        pixmap = QPixmap(":/images/designer-splash-en.png");

    QSplashScreen *splash = new QSplashScreen(pixmap);

    if (ananas_login(rcfile, username, userpassword, 0, AApplication::Designer)) {
        splash->show();
        splash->showMessage(QObject::tr("Init application"), Qt::AlignBottom, Qt::white);

        w->rcfile = rcfile;

        splash->showMessage(QObject::tr("Init forms designer"), Qt::AlignBottom, Qt::white);
        formdesigner = new aFormDesigner();

        formdesigner->setWindowTitle(QObject::tr("Dialog forms designer"));
        formdesigner->hide();

        w->show();
        splash->clearMessage();
        splash->finish(w);
        delete splash;

        QObject::connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));

        mainform->configOpen();

        rc = app.exec();

        if (w)
            delete w;
        w = 0;

        aLog::close();
        ananas_logout();
        return rc;
    }

    delete splash;
    aLog::close();
    return 0;
}
