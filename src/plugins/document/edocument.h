#ifndef EDOCUMENT_H
#define EDOCUMENT_H

#include "ui_edocument.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>

class eDocument : public QDialog, public Ui::eDocument
{
    Q_OBJECT

public:
    eDocument(QWidget *parent, Qt::WindowFlags fl);
    ~eDocument();

    QStringList otypes;

public slots:
    virtual void init();
    virtual void setData( QWidget * o, aCfg * md );
    virtual void getData( QWidget * o );

protected slots:
    virtual void languageChange();

};

#endif // EDOCUMENT_H
