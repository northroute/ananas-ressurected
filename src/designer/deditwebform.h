#ifndef DEDITWEBFORM_H
#define DEDITWEBFORM_H

#include "ui_deditwebform.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>
#include <QScriptEngine>

class dEditWebForm : public QMainWindow, public Ui::dEditWebForm
{
    Q_OBJECT

public:
    dEditWebForm(QWidget* parent, Qt::WindowFlags fl);
    ~dEditWebForm();

    aAliasEditor *al;
    aListViewItem *item;

public slots:
    virtual void setData( aListViewItem * o );
    virtual void updateMD();

protected slots:
    virtual void languageChange();

private:
    void init();
    void closeEditor();

};

#endif // DEDITWEBFORM_H
