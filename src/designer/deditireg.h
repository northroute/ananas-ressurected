#ifndef DEDITIREG_H
#define DEDITIREG_H


#include "ui_deditireg.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>

class dEditIReg : public QMainWindow, public Ui::dEditIReg
{
    Q_OBJECT

public:
    dEditIReg(QWidget* parent, Qt::WindowFlags fl);
    ~dEditIReg();

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

#endif // DEDITIREG_H
