#include "mainform.h"

#include "ananas.h"
//#include "adatabase.h"
//#include "acfgrc.h"
#include "cfgform.h"
//#include "messageswindow.h"
#include "dselectdb.h"
#include "alog.h"

//#include "qananastable.h"

extern CfgForm *configform;
//extern QApplication *application;
extern void messageproc(int n, const char *msg);

/*
 *  Constructs a MainForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
MainForm::MainForm(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
MainForm::~MainForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MainForm::languageChange()
{
    retranslateUi(this);
}

void MainForm::filePrint()
{

}

void MainForm::fileExit()
{
//    cfg_done();
}

void MainForm::editUndo()
{

}

void MainForm::editRedo()
{

}

void MainForm::editCut()
{

}

void MainForm::editCopy()
{

}

void MainForm::editPaste()
{

}

void MainForm::editFind()
{

}

void MainForm::helpIndex()
{

}

void MainForm::helpContents()
{

}

void MainForm::helpAbout()
{
    QMessageBox::about(
        this,
        tr("About Ananas.Designer"),
        tr("<h4>Ananas.Designer %1</h4> is a program<br>"
           "for adjusting accounting automation system<br><br>"
           "Copyright 2003-2006 Leader Infotech, Valery Grazdankin <br>"
           "Copyright 2003-2006 Project Ananas, Andrey Paskal, Grigory Panov, Andrey Strelnikov<br>"
           "License: GPL<br><br>"
           "technical support:<br>"
           "lider-infotech@narod.ru<br>"
           "app@linux.ru.net<br>"
           "Web sites:<br>"
           "<a href=\"http://ananas.lrn.ru\">http://ananas.lrn.ru</a><br>"
           "<a href=\"http://www.leaderit.ru\">http://www.leaderit.ru</a>"
           "<br><br>Available extensions:<br>%2")
            .arg(ananas_libversion())
            .arg("" /* AExtensionFactory::keys().join(", ") */)
    );
}

void MainForm::init()
{
    setObjectName("ananas-designer_mainwindow");
    rcfile = "";

    windowsMenu = new QMenu(tr("&Windows"), this);
    windowsMenu->setSeparatorsCollapsible(true);

    connect(windowsMenu, SIGNAL(aboutToShow()),
            this, SLOT(windowsMenuAboutToShow()));

    menuBar()->addMenu(windowsMenu);

    QMenu *help = menuBar()->addMenu(tr("&Help"));
    help->addAction(tr("&About"), this, SLOT(helpAbout()), Qt::Key_F1);
    help->addSeparator();

    QSettings designer_settings;
    designer_settings.beginGroup("designer");

    bool maximize = designer_settings.value("maximize", false).toBool();
    int width = designer_settings.value("geometry/width", 600).toInt();
    int height = designer_settings.value("geometry/height", 480).toInt();
    int offset_x = designer_settings.value("geometry/x", 0).toInt();
    int offset_y = designer_settings.value("geometry/y", 0).toInt();

    designer_settings.endGroup();

    resize(width, height);
    move(offset_x, offset_y);

    if (maximize)
        setWindowState(windowState() | Qt::WindowMaximized);

    setWindowIcon(QIcon(rcIcon("a-system.png")));
    setWindowTitle(tr("Ananas: Designer"));

    ws = new QMdiArea(this);
    ws->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ws->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(ws);

    wl = new aWindowsList();
    cfgform = 0;

    MessagesWindow *msgWindow = new MessagesWindow(this);
    addDockWidget(Qt::BottomDockWidgetArea, msgWindow);
    setMessageHandler(true);
    msgWindow->hide();

    tbMetadata->setVisible(false);
    tbActions->setVisible(false);
    tbInterface->setVisible(false);
    tbRights->setVisible(false);
    tbLanguages->setVisible(false);
    tbImageCollection->setVisible(false);
    TBToolbar->setVisible(false);
    tbTabs->setVisible(false);

    lastTabId = 0;
    configSaveAction->setVisible(false);
}

void MainForm::configNew()
{
    CfgForm *cf = cfgForm();
    if (cf)
    {
        if (!cf->close())
            return;
    }

    cf = new CfgForm(0, Qt::WindowFlags());
    if (!cf)
        return;

    QMdiSubWindow *sub = ws->addSubWindow(cf);

    cf->init(rcfile, true);
    connectSignals(cf);
    cf->ws = ws;

    cf->show();
    sub->move(0, 0);

    addTab(cf);
}

void MainForm::configOpen()
{
    CfgForm *cf = cfgForm();
    if (cf)
    {
        if (!cf->close())
            return;
    }

    if (rcfile.isEmpty())
        return;

    cf = new CfgForm(0, Qt::WindowFlags());
    if (!cf)
        return;

    QMdiSubWindow *sub = ws->addSubWindow(cf);

    cf->init(rcfile, false);
    connectSignals(cf);
    cf->ws = ws;

    cf->show();
    sub->move(0, 0);

    addTab(cf);
}

void MainForm::configSave()
{
    emit tosave();
    CfgForm *cf = cfgForm();
    if ( cf ) cf->save();	//	get configuration form and save its data
}

void MainForm::configSaveAs()
{
}



void MainForm::fileOpen()
{

}

void MainForm::fileSave()
{

}

void MainForm::fileSaveAs()
{

}

void MainForm::fileNew()
{

}

void MainForm::windowsMenuActivated(int id)
{
    QList<QMdiSubWindow*> windows = ws->subWindowList();

    if (id < 0 || id >= windows.count())
        return;

    QMdiSubWindow *sub = windows.at(id);
    if (!sub)
        return;

    sub->showNormal();
    ws->setActiveSubWindow(sub);
    sub->widget()->setFocus();
}

void MainForm::windowsMenuAboutToShow()
{
    windowsMenu->clear();

    QAction *cascadeAct = windowsMenu->addAction(tr("&Cascade"), ws, SLOT(cascadeSubWindows()));
    QAction *tileAct = windowsMenu->addAction(tr("&Tile"), ws, SLOT(tileSubWindows()));
    QAction *horTileAct = windowsMenu->addAction(tr("Tile &horizontal"), this, SLOT(tileHorizontal()));

    QList<QMdiSubWindow*> windows = ws->subWindowList();

    if (windows.isEmpty())
    {
        cascadeAct->setEnabled(false);
        tileAct->setEnabled(false);
        horTileAct->setEnabled(false);
    }

    windowsMenu->addSeparator();

    for (int i = 0; i < windows.count(); ++i)
    {
        QMdiSubWindow *sub = windows.at(i);
        QWidget *child = sub->widget();

        QString title = child ? child->windowTitle() : QString();

        QAction *act = windowsMenu->addAction(title, this, SLOT(windowsMenuActivated(int)));
        act->setData(i);
        act->setCheckable(true);
        act->setChecked(ws->activeSubWindow() == sub);
    }
}

void MainForm::tileHorizontal()
{
    QList<QMdiSubWindow*> windows = ws->subWindowList();
    if (windows.isEmpty())
        return;

    int heightForEach = ws->height() / windows.count();
    int y = 0;

    for (int i = 0; i < windows.count(); ++i)
    {
        QMdiSubWindow *sub = windows.at(i);

        if (sub->windowState() == Qt::WindowMaximized)
        {
            sub->hide();
            sub->showNormal();
        }

        QWidget *child = sub->widget();
        int preferredHeight = child ? child->minimumHeight() : 0;
        int actHeight = qMax(heightForEach, preferredHeight);

        sub->setGeometry(0, y, ws->width(), actHeight);
        y += actHeight;
    }
}

void MainForm::closeEvent(QCloseEvent *e)
{
    if (configform)
    {
        if (!configform->close())
        {
            e->ignore();
            return;
        }
    }

    QSettings designer_settings;
    designer_settings.beginGroup("designer");

    designer_settings.setValue("geometry/width", width());
    designer_settings.setValue("geometry/height", height());
    designer_settings.setValue("geometry/x", pos().x());
    designer_settings.setValue("geometry/y", pos().y());

    designer_settings.endGroup();

    QMainWindow::closeEvent(e);
}

void MainForm::connectSignals(CfgForm *cf)
{
    connect(objNewFormAction, SIGNAL(triggered()), cf, SLOT(newForm()));
    connect(objNewTableAction, SIGNAL(triggered()), cf, SLOT(newTable()));
    connect(objNewFieldAction, SIGNAL(triggered()), cf, SLOT(newField()));

    connect(objRenameAction, SIGNAL(triggered()), cf->mdtree, SLOT(itemRename()));
    connect(objNewObjAction, SIGNAL(triggered()), cf->mdtree, SLOT(itemNew()));
    connect(objEditAction, SIGNAL(triggered()), cf->mdtree, SLOT(itemEdit()));
    connect(objDeleteAction, SIGNAL(triggered()), cf->mdtree, SLOT(itemDelete()));
}

void MainForm::getMd(aCfg **md)
{
    CfgForm *cf = cfgForm();
    if (!cf)
        return;

    *md = &cf->cfg;
}

CfgForm* MainForm::cfgForm()
{
    QList<QMdiSubWindow*> windows = ws->subWindowList();

    for (int i = 0; i < windows.count(); ++i)
    {
        QMdiSubWindow *sub = windows.at(i);
        QWidget *child = sub->widget();

        if (!child)
            continue;

        if (child->inherits("CfgForm"))
            return static_cast<CfgForm*>(child);
    }

    return 0;
}

QWidget* MainForm::activeWindow()
{
    QMdiSubWindow *sub = ws->activeSubWindow();
    if (!sub)
        return 0;

    return sub->widget();
}

int MainForm::getId()
{
    return  lastFormId;
}


void MainForm::addTab(int uid, const QString &winName)
{
    QList<QMdiSubWindow*> windows = ws->subWindowList();
    QToolButton *bt = 0;

    QString s = winName;
    for (int i = 0; i < windows.count(); ++i)
    {
        QMdiSubWindow *sub = windows.at(i);
        QWidget *child = sub->widget();

        if (!child)
            continue;

        if (child->objectName() == s)
        {
            QString newName = child->objectName() + QString("_%1").arg(uid);
            child->setObjectName(newName);

            bt = new QToolButton(tbTabs);
            bt->setObjectName(newName);
            bt->setText(child->windowTitle());
            bt->setToolButtonStyle(Qt::ToolButtonTextOnly);

            connect(bt, SIGNAL(clicked()), child, SLOT(setFocus()));
            break;
        }
    }

    tbTabs->setVisible(!tbTabs->findChildren<QToolButton*>().isEmpty());
}


void MainForm::removeTab(const QString &winName)
{
    QObject *button = tbTabs->findChild<QObject*>(winName);
    if (button)
    {
        delete button;
    }
}

void MainForm::closeChildWindows()
{
    QList<QMdiSubWindow*> windows = ws->subWindowList();
    for (int i = 0; i < windows.count(); ++i)
    {
        QMdiSubWindow *sub = windows.at(i);
        QWidget *child = sub->widget();

        if (!child)
            continue;

        if (!child->inherits("CfgForm"))
        {
            sub->close();
        }
    }
}


void MainForm::fileNewAction_activated()
{

}
