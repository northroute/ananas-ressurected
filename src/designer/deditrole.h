#ifndef DEDITROLE_H
#define DEDITROLE_H


#include "ui_deditrole.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>
#include <QScriptEngine>

class dEditRole : public QMainWindow, public Ui::dEditRole
{
    Q_OBJECT

public:
    dEditRole(QWidget* parent, Qt::WindowFlags fl);
    ~dEditRole();

public slots:
    virtual void setData( CfgForm * c, aCfgItem o );
    virtual void updateMD();

protected slots:
    virtual void languageChange();

private:
    aCfgItem obj;
    CfgForm *cf;
    int roleCount;

    void init();
    void closeEditor();

};

#endif // DEDITROLE_H
