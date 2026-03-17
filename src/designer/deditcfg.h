#ifndef DEDITCFG_H
#define DEDITCFG_H

#include "ui_deditcfg.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QLayout>
#include <QStatusBar>
#include <QScriptEngine>

class dEditCfg : public QMainWindow, public Ui::dEditCfg
{
    Q_OBJECT

public:
    dEditCfg(QWidget* parent, Qt::WindowFlags fl);
    ~dEditCfg();

    aListViewItem *item;
    aCfgItem globals;

public slots:
    virtual void init();
    virtual void closeEditor();
    virtual void setData( aListViewItem * o );
    virtual void updateMD();

protected slots:
    virtual void languageChange();

};

#endif // DEDITCFG_H
