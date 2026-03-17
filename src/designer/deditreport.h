#ifndef DEDITREPORT_H
#define DEDITREPORT_H

#include "ui_deditreport.h"


#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>
#include <QScriptEngine>

class dEditReport : public QMainWindow, public Ui::dEditReport
{
    Q_OBJECT

public:
    dEditReport(QWidget* parent, Qt::WindowFlags fl);
    ~dEditReport();

    aListViewItem *item;
    aAliasEditor *al;

public slots:
    virtual void setData( aListViewItem * o );
    virtual void updateMD();

protected slots:
    virtual void languageChange();

    virtual void init();
    virtual void closeEditor();


};

#endif // DEDITREPORT_H
