/****************************************************************************
** $Id: main.cpp,v 1.6 2008/12/25 19:08:03 leader Exp $
**
** Main file of Ananas Engine application
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
**
** This file is part of the Engine application of the Ananas
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

#include <QMessageBox>
#include <QTranslator>
#include <QSplashScreen>
#include <QTextCodec>
#include <QPixmap>

#include <aapplication.h>

#include "mainform.h"
#include "dselectdb.h"
#include "dlogin.h"
#include "ananas.h"
#include "alog.h"
#include "aextensionfactory.h"

QTranslator tr_app, tr_lib, tr_plugins;

QString lang = "en",
        rcfile = "",
        username = "",
        userpassword = "";

int setTranslator(QString langdir, QString lang)
{
    tr_app.load(langdir + "ananas-engine-" + lang.toLower() + ".qm", ".");
    tr_lib.load(langdir + "ananas-lib-" + lang.toLower() + ".qm", ".");
    tr_plugins.load(langdir + "ananas-plugins-" + lang.toLower() + ".qm", ".");
    return 0;
}

int parseCommandLine( AApplication *a )
{
    QString param, name, value;
    int i, argc;
    char **argv;

    argc = a->argc();
    argv = a->argv();
    setTranslator( a->langDir(), a->lang() );

    QString str_ru, str_en;
    bool lang_setted = false;
    bool help_setted = false;

    for ( i = 1; i < argc; i++ )
    {
        param = QString::fromLocal8Bit(argv[i]);
        name = param.section("=", 0, 0).toLower();
        value = param.section("=", 1);

        if ( param == "--help" )
        {
            str_ru = "Использование: ananas [--help] [--lang=<LANG>] [--rc=<RC_PATH>]\n";
            str_ru += "LANG=ru|en\n";
            str_ru += "RC_PATH=путь к *.rc файлу описания бизнес схемы\n";

            str_en = "Usage: ananas [--help] [--lang=<LANG>] [--rc=<RC_PATH>]\n";
            str_en += "LANG=ru|en\n";
            str_en += "RC_PATH=path to *.rc file of particular business scheme\n";

            help_setted = true;
        }

        if ( name == "--lang" ) {
            lang = value;
            lang_setted = true;
            setTranslator( a->langDir(), lang );
        }

        if ( name == "--rc" ) {
            rcfile = value;
        }
    }

    if ( help_setted )
    {
        if ( lang == "ru" ) {
            printf("%s", str_ru.toLocal8Bit().constData());
        } else {
            printf("%s", str_en.toLocal8Bit().constData());
        }
        return 1;
    }

    return 0;
}

int main( int argc, char ** argv )
{
    AApplication a( argc, argv, AApplication::Ananas );
    dLogin dlogin;
    int rc = 1;
    bool ok;
    QPixmap pixmap;

    QTextCodec::setCodecForCStrings( QTextCodec::codecForName("UTF8") );

    a.setOrganizationName("ananasgroup");
    a.setApplicationName("ananas");

    if ( parseCommandLine( &a ) ) return 1;
    qApp->installTranslator( &tr_app );
    qApp->installTranslator( &tr_lib );
    qApp->installTranslator( &tr_plugins );

    pixmap = QPixmap( ":/images/engine-splash-" + lang + ".png" );

    if ( pixmap.isNull() ) {
#ifdef Q_OS_WIN32
        pixmap = QPixmap( qApp->applicationDirPath() + "/engine-splash-" + lang + ".png" );
        qApp->addLibraryPath( qApp->applicationDirPath() );
#else
        pixmap = QPixmap( "/usr/share/ananas/designer/locale/engine-splash-" + lang + ".png" );
        qApp->addLibraryPath( "/usr/lib/ananas/" );
#endif
    }

    if ( pixmap.isNull() )
        pixmap = QPixmap( ":/images/engine-splash-en.png" );

    QSplashScreen *splash = new QSplashScreen( pixmap );

    if ( ananas_login( rcfile, username, userpassword, 0, AApplication::Ananas ) ) {
        splash->show();
        splash->showMessage( QObject::tr("Init application"), Qt::AlignBottom, Qt::white );

        MainForm *w = new MainForm(0);
        mainform = w;
        mainformws = mainform->ws;
        mainformwl = mainform->wl;

        // qApp->setMainWidget( w );
        w->rcfile = rcfile;
        w->show();

        ok = w->init();

        splash->clearMessage();
        splash->finish( w );
        delete splash;

        if ( ok ) {
            qApp->connect( qApp, SIGNAL( lastWindowClosed() ), qApp, SLOT( quit() ) );
            rc = qApp->exec();
            ananas_logout( w->engine.db );
            delete w;
            w = 0;
        } else {
            QMessageBox::critical(0, "error", "Error init Ananas engine");
        }

        aLog::close();
        return rc;
    } else {
        aLog::close();
        return 0;
    }
}
