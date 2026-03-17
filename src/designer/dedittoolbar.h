#ifndef DEDITTOOLBAR_H
#define DEDITTOOLBAR_H


#include "ui_dedittoolbar.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>
#include <QHeaderView>

class dEditToolbar : public QMainWindow, public Ui::dEditToolbar
{
    Q_OBJECT

public:
    dEditToolbar(QWidget* parent, Qt::WindowFlags fl);
    ~dEditToolbar();

    ananasTreeView *vComActions;
    aAliasEditor *al;
    InterfaceListViewItem *item;
    aActionTreeView *actiontree;
    aCfg *md;
    aCfgItem obj;

public slots:
    virtual void updateMD();
    virtual void setData( InterfaceListViewItem * o );
    virtual void bAddAction_clicked();
    virtual void bRemoveAction_clicked();
    virtual void bMoveUp_clicked();
    virtual void bMoveDown_clicked();

protected slots:
    virtual void languageChange();

private:
    void closeEditor();
    void init();

};

#endif // DEDITTOOLBAR_H
