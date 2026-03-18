/****************************************************************************
** $Id: mainform.cpp,v 1.2 2008/12/06 22:56:39 leader Exp $
**
** Code file of the Main form of Ananas Engine application
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

/****************************************************************************
****************************************************************************/

#include "mainform.h"

#include <QMdiSubWindow>

#include "ananas.h"

MainForm *mainform=NULL;
QMdiArea *mainformws=NULL;
aWindowsList *mainformwl=NULL;
//extern void messageproc(int n, const char *msg);

/*
 *  Constructs a AnanasMainWindow as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
MainForm::MainForm(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setObjectName("MainForm");

    QWidget *central = new QWidget(this);
    QVBoxLayout *vb = new QVBoxLayout(central);

    setWindowIcon(rcIcon("a-system.png"));

    ws = new QMdiArea(central);
    wl = new aWindowsList();

    vb->addWidget(ws);
    central->setLayout(vb);
    setCentralWidget(central);

    ws->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ws->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    statusBar()->setObjectName("statusbar");

    engine_settings.beginGroup("engine");
    bool maximize = engine_settings.value("maximize", false).toBool();
    int width = engine_settings.value("geometry/width", 600).toInt();
    int height = engine_settings.value("geometry/height", 480).toInt();
    int offset_x = engine_settings.value("geometry/x", 0).toInt();
    int offset_y = engine_settings.value("geometry/y", 0).toInt();
    engine_settings.endGroup();

    resize(QSize(width, height));
    move(offset_x, offset_y);

    if (maximize) {
        // showMaximized();
    }

    rcfile = "";
}

bool MainForm::init()
{
    MessagesWindow *msgWindow = new MessagesWindow(this);
    addDockWidget(Qt::BottomDockWidgetArea, msgWindow);
    setMessageHandler(true);
    msgWindow->hide();

    if (!initEngine()) return false;

    connect(menubar, SIGNAL(activated(int)), &engine, SLOT(on_MenuBar(int)));
    languageChange();

    engine.project->globalObject().setProperty(
        "menubar",
        engine.project->newQObject(menubar)
    );
    engine.project->globalObject().setProperty(
        "mainForm",
        engine.project->newQObject(this)
    );
    engine.project->globalObject().setProperty(
        "statusBar",
        engine.project->newQObject(statusBar())
    );

    languageChange();
    engine.on_systemstart();
    return true;
}

bool MainForm::initEngine()
{
    if (rcfile.isEmpty()) return false;
    if (!engine.init(rcfile)) return false;

    engine.ws = ws;
    engine.wl = wl;

    connect(&engine, SIGNAL(statusMessage(const QString &)), this, SLOT(statusMessage(const QString &)));

    md = engine.md;
    if (!md) return false;

    initMenuBar();
    initStatusBar();
    initActionBar();
    return true;
}

void MainForm::initMenuBar()
{
    QMenu *m = new QMenu(this);
    windowsMenu = new QMenu(this);

    connect(windowsMenu, SIGNAL(aboutToShow()),
            this, SLOT(windowsMenuAboutToShow()));

    m->addAction(rcIcon("a-system.png"), tr("About"), this, SLOT(helpAbout()));

    menubar = new AMenuBar(md, this, "menubar");

    InsertMainMenu(tr("&Help"), m);
    InsertMainMenu(tr("&Windows"), windowsMenu);
}

void
MainForm::initStatusBar()
{
}

void MainForm::initActionBar()
{
    aCfgItem obj, item;

    obj = md->find(md->find(mdc_interface), md_toolbars);
    item = md->firstChild(obj);

    while (!item.isNull()) {
        new aToolBar(md, item, &engine, this);
        item = md->nextSibling(item);
    }
}

void MainForm::helpAbout()
{
    QMessageBox::about(
        this,
        tr("About Ananas.Engine program"),
        tr("<h4>Ananas.Engine %1</h4> is a program "
           "for executing application of accounting automation system<br><br>"
           "Copyright 2003-2008 Leader Infotech, Valery Grazdankin, "
           "Copyright 2003-2008 Project Ananas, Andrey Paskal, Grigory Panov, Andrey Strelnikov<br>"
           "License: GPL<br><br>"
           "technical support:<br>"
           "lider-infotech@narod.ru<br>"
           "app@linux.ru.net<br>"
           "Web sites:<br>"
           "<a href=\"http://ananas.lrn.ru\">http://ananas.lrn.ru</a><br>"
           "<a href=\"http://www.leaderit.ru\">http://www.leaderit.ru</a>"
           "<br>Available extensions:<h4>%2</h4><br>")
            .arg(ananas_libversion())
            .arg(AExtensionFactory::keys().join(", "))
    );
}

void MainForm::InsertMainMenu(QString text, QObject *pop){
    menubar->insertItem(text, (QMenu *) pop);

}


void MainForm::Exit(int code)
{
	qApp->exit(code);
}

void MainForm::close()
{
    engine_settings.beginGroup("engine");
    engine_settings.setValue("geometry/width", width());
    engine_settings.setValue("geometry/height", height());
    engine_settings.setValue("geometry/x", pos().x());
    engine_settings.setValue("geometry/y", pos().y());
    engine_settings.endGroup();
    printf("exit\n");
}

void MainForm::statusMessage(const QString &msg)
{
    statusBar()->showMessage(msg);
}

void MainForm::setBackground(const QPixmap &pix)
{
    ws->setBackground(QBrush(pix));
}

/*
 *  Destroys the object and frees any allocated resources
 */
MainForm::~MainForm()
{
    engine.on_systemstop();
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MainForm::languageChange()
{
    setWindowTitle(tr("Ananas") + QString(" ") + QString(ananas_libversion()) + ": " + md->info(md_info_name));
}

void MainForm::windowsMenuAboutToShow()
{
    windowsMenu->clear();

    QAction *cascadeAction = windowsMenu->addAction(tr("&Cascade"), ws, SLOT(cascadeSubWindows()));
    QAction *tileAction = windowsMenu->addAction(tr("&Tile"), ws, SLOT(tileSubWindows()));
    QAction *horTileAction = windowsMenu->addAction(tr("Tile &horizontal"), this, SLOT(tileHorizontal()));

    QList<QMdiSubWindow*> windows = ws->subWindowList();

    if (windows.isEmpty()) {
        cascadeAction->setEnabled(false);
        tileAction->setEnabled(false);
        horTileAction->setEnabled(false);
    }

    windowsMenu->addSeparator();

    if (windows.isEmpty()) return;

    QMdiSubWindow *active = ws->activeSubWindow();

    for (int i = 0; i < windows.count(); ++i)
    {
        QMdiSubWindow *window = windows.at(i);
        if (!window || window->isHidden())
            continue;

        QAction *action = windowsMenu->addAction(window->windowTitle(), this, SLOT(windowsMenuActivated()));
        action->setCheckable(true);
        action->setChecked(active == window);
        action->setData(i);
    }
}

void MainForm::tileHorizontal()
{
    QList<QMdiSubWindow*> windows = ws->subWindowList();
    int count = 0;

    for (int i = 0; i < windows.count(); ++i)
    {
        QMdiSubWindow *window = windows.at(i);
        if (!window) continue;
        if (!window->isHidden()) count++;
    }

    if (!count) return;

    int heightForEach = ws->height() / count;
    int y = 0;

    for (int i = 0; i < windows.count(); ++i)
    {
        QMdiSubWindow *window = windows.at(i);
        if (!window) continue;
        if (window->isHidden()) continue;

        if (window->windowState() == Qt::WindowMaximized)
        {
            window->hide();
            window->showNormal();
        }

        int preferredHeight = window->minimumHeight();
        int actHeight = qMax(heightForEach, preferredHeight);

        window->setGeometry(0, y, ws->width(), actHeight);
        y += actHeight;
    }
}

void MainForm::windowsMenuActivated()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action) return;

    bool ok = false;
    int id = action->data().toInt(&ok);
    if (!ok) return;

    QList<QMdiSubWindow*> windows = ws->subWindowList();
    if (id < 0 || id >= windows.count()) return;

    QMdiSubWindow *w = windows.at(id);
    if (!w) return;

    w->showNormal();
    ws->setActiveSubWindow(w);
    w->setFocus();
}
