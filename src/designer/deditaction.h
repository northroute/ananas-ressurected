#ifndef DEDITACTION_H
#define DEDITACTION_H

#include "ui_deditaction.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QTabBar>
#include <QStatusBar>

class dEditAction : public QMainWindow, public Ui::dEditAction
{
    Q_OBJECT

public:
    dEditAction(QWidget* parent, Qt::WindowFlags fl);
    ~dEditAction();

    ActionListViewItem * item;
    aAliasEditor *al;
    QStringList otypes;
    QStringList formslist;
    aCfg * md;
    aCfgItem obj;
    QPixmap *dp;

public slots:
    virtual void setData( ActionListViewItem * o );
    virtual void updateMD();
    virtual void bSetActive_clicked();
    virtual void bSetPassive_clicked();
    virtual void bRemoveActive_clicked();
    virtual void bRemovePassive_clicked();

protected:
    virtual void selectStandart();
    virtual void selectUser();

protected slots:
    virtual void languageChange();

    virtual void init();
    virtual void destroy();


private slots:
    virtual void cbTypeChange();
    virtual void cbObjectChange();

};

#endif // DEDITACTION_H
