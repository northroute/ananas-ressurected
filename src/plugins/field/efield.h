#ifndef EFIELD_H
#define EFIELD_H

#include "ui_efield.h"


#include <stdlib.h>
#include <QVariant>
#include <QImage>
#include <QPixmap>

class eField : public QDialog, public Ui::eField
{
    Q_OBJECT

public:
    eField(QWidget *parent, Qt::WindowFlags fl);
    ~eField();

    QStringList otypes;

public slots:
    virtual void init();
    virtual void setData( QWidget * o, aCfg * cfg );
    virtual void getData( QWidget * o );
    virtual void typechanged( const QString & s );
    virtual void typeSelect( int idx );

protected:
    aCfg * md;

protected slots:
    virtual void languageChange();

};

#endif // EFIELD_H
