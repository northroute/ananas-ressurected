#ifndef DEDITCOLUMNS_H
#define DEDITCOLUMNS_H

#include "ui_deditcolumns.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>
#include <QHeaderView>
#include <QGridLayout>

class dEditColumns : public QMainWindow, public Ui::dEditColumns
{
    Q_OBJECT

public:
    dEditColumns(QWidget* parent, Qt::WindowFlags fl);
    ~dEditColumns();

public slots:
    virtual void setData( ananasListViewItem * o );
    virtual void updateMD();
    virtual void bAddAction_clicked();
    virtual void bRemoveAction_clicked();
    virtual void bMoveUp_clicked();
    virtual void bMoveDown_clicked();

protected slots:
    virtual void languageChange();

private:
    aCfgItem obj;
    aCfg *md;
    aAliasEditor *al;
    ananasListViewItem * item;
    ananasTreeView *availableFields;
    ananasTreeView *usedFields;
    QStringList fieldsList;

    void closeEditor();
    void init();
    virtual void addField( aCfgItem addItem );

};

#endif // DEDITCOLUMNS_H
