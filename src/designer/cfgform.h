#ifndef CFGFORM_H
#define CFGFORM_H

#include "ui_cfgform.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QTreeWidget>
#include <QLabel>
#include <QMenu>
#include <QStatusBar>
#include <QLineEdit>
#include <QMessageBox>
#include <QList>
#include <QBitmap>
#include <QTableWidget>
#include <QDrag>
#include <QMimeData>
#include <QFileDialog>
#include <QInputDialog>
#include <QByteArray>
#include <QGridLayout>
#include <QCloseEvent>
#include <QMdiArea>
#include <QMdiSubWindow>

class CfgForm : public QMainWindow, public Ui::CfgForm
{
    Q_OBJECT

public:
    CfgForm(QWidget* parent, Qt::WindowFlags fl);
    ~CfgForm();

    QHash<int, QListWidgetItem*> idList;
    InterfaceTreeView *interfacetree;
    aActionTreeView *actiontree;
    aCfg cfg;
    QMdiArea *ws;
    aMetadataTreeView *mdtree;
    InterfaceTreeView *toolbartree;
    QString rcfile;
    aWindowsList *wl;

public slots:
    virtual void fileNew();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void filePrint();
    virtual void fileExit();
    virtual void init( QString & rcf, bool flagNew );
    virtual void initImageCollection();
    virtual void bAddImage_clicked();
    virtual void bRemoveImage_clicked();
    virtual void vImageCollection_itemChanged(QListWidgetItem *item);
    virtual void initLang();
    virtual void initRoles();
    virtual void save();
    virtual void bAdd_clicked();
    virtual void bDelete_clicked();
    virtual void bEdit_clicked();
    virtual void tLang_doubleClicked( int, int, int, const QPoint & );
    virtual void bKill_clicked();
    virtual void newObj();
    virtual void listCfg_onItem( QTreeWidgetItem * );
    virtual void newField();
    virtual void newForm();
    virtual void newTable();
    virtual void tabWidget_selected( const QString & tab );
    virtual void bAddRole_clicked();
    virtual void bDeleteRole_clicked();
    virtual void bEditRole_clicked();
    virtual void bKillRole_clicked();
    virtual void tRole_doubleClicked( int, int, int, const QPoint & );

signals:
    void wActivate();
    void wDeactivate();

protected slots:
    virtual void languageChange();

private:
    void destroy();
    virtual void closeEvent( QCloseEvent * e );
    virtual int closeSubWindows();

};

#endif // CFGFORM_H
