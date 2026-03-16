#ifndef EADDOBJ_H
#define EADDOBJ_H

#include "ui_eaddobj.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>


class eAddObj : public QMainWindow, public Ui::eAddObj
{
    Q_OBJECT

public:
    eAddObj(QWidget* parent = 0, Qt::WindowFlags fl = 0);
    ~eAddObj();

protected slots:
    virtual void languageChange();

private:
    void init();
    void destroy();

};

#endif // EADDOBJ_H
