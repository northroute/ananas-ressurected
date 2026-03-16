#ifndef EREPORT_H
#define EREPORT_H

#include "ui_ereport.h"


#include <QVariant>
#include <QImage>
#include <QPixmap>

class eReport : public QDialog, public Ui::eReport
{
    Q_OBJECT

public:
    eReport(QWidget *parent, Qt::WindowFlags fl);
    ~eReport();

    QStringList otypes;

public slots:
    virtual void init();
    virtual void setData( QWidget * o, aCfg * md );
    virtual void getData( QWidget * o );

protected slots:
    virtual void languageChange();

};

#endif // EREPORT_H
