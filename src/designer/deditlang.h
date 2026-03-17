#ifndef DEDITLANG_H
#define DEDITLANG_H


#include "ui_deditlang.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>
#include <QMessageBox>

class dEditLang : public QMainWindow, public Ui::dEditLang
{
    Q_OBJECT

public:
    dEditLang(QWidget* parent, Qt::WindowFlags fl);
    ~dEditLang();

public slots:
    virtual void setData( CfgForm * c, aCfgItem o );
    virtual void updateMD();

protected slots:
    virtual void languageChange();

private:
    aCfgItem obj;
    int langCount;
    CfgForm *cf;

    void init();
    void closeEditor();

};

#endif // DEDITLANG_H
