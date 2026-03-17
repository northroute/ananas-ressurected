#ifndef DEDITIMAGECOLLECTION_H
#define DEDITIMAGECOLLECTION_H


#include "ui_deditimagecollection.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>

class dEditImageCollection : public QMainWindow, public Ui::dEditImageCollection
{
    Q_OBJECT

public:
    dEditImageCollection(QWidget* parent, Qt::WindowFlags fl);
    ~dEditImageCollection();

    aCfgItem obj;

public slots:
    virtual void setData();

protected slots:
    virtual void languageChange();

private:
    void init();

};

#endif // DEDITIMAGECOLLECTION_H
