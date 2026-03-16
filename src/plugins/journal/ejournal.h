#ifndef EJOURNAL_H
#define EJOURNAL_H

#include "ui_ejournal.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>

class eJournal : public QDialog, public Ui::eJournal
{
    Q_OBJECT

public:
    eJournal(QWidget *parent, Qt::WindowFlags fl);
    ~eJournal();

    QStringList otypes;

public slots:
    virtual void init();
    virtual void setData( QWidget * o, aCfg * md );
    virtual void getData( QWidget * o );

protected slots:
    virtual void languageChange();

};

#endif // EJOURNAL_H
