#ifndef DSELECTUSER_H
#define DSELECTUSER_H


#include <QVariant>
#include <QImage>
#include <QPixmap>

#include "ui_dselectuser.h"

class dSelectUser : public QDialog, public Ui::dSelectUser
{
    Q_OBJECT

public:
    dSelectUser(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~dSelectUser();

public slots:
    virtual void setData( aRole * i );
    virtual quint64 getData();

protected slots:
    virtual void languageChange();

private:
    quint64 userId;
    QStringList listId;
    aRole *role;

private slots:
    virtual void onSelect();

};

#endif // DSELECTUSER_H
