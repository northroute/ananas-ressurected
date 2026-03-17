#ifndef DEDITCOMMAND_H
#define DEDITCOMMAND_H

#include "ui_deditcommand.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>
#include <QHeaderView>

class dEditCommand : public QMainWindow, public Ui::dEditCommand
{
    Q_OBJECT

public:
    dEditCommand(QWidget* parent, Qt::WindowFlags fl);
    ~dEditCommand();

    ananasTreeView *vComActions;
    aActionTreeView *actiontree;
    InterfaceListViewItem * item;
    aAliasEditor *al;
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

#endif // DEDITCOMMAND_H
