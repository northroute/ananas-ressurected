#ifndef ESELECTDOCTYPE_H
#define ESELECTDOCTYPE_H

#include "ui_eselectdoctype.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>

class eSelectDocType : public QDialog, public Ui::eSelectDocType
{
    Q_OBJECT

public:
    eSelectDocType(QWidget *parent, Qt::WindowFlags fl);
    ~eSelectDocType();

    QStringList dlist;
    int docId;

public slots:
    virtual void init();
    virtual void setJournal( aCfg * md, int id );
    virtual void accepted();

protected slots:
    virtual void languageChange();

};

#endif // ESELECTDOCTYPE_H
