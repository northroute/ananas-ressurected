#ifndef DEDITAREG_H
#define DEDITAREG_H

#include "ui_deditareg.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>

class dEditAReg : public QMainWindow, public Ui::dEditAReg
{
    Q_OBJECT

public:
    dEditAReg(QWidget* parent, Qt::WindowFlags fl);
    ~dEditAReg();

    aListViewItem *item;
    aAliasEditor *al;

public slots:
    virtual void setData( aListViewItem * o );
    virtual void updateMD();

protected slots:
    virtual void languageChange();

private:
    void init();
    void closeEditor();

};

#endif // DEDITAREG_H
