#ifndef DEDITDOC_H
#define DEDITDOC_H


#include "ui_deditdoc.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>
#include <QScriptEngine>
#include <QHash>

class dEditDoc : public QMainWindow, public Ui::dEditDoc
{
    Q_OBJECT

public:
    dEditDoc(QWidget* parent, Qt::WindowFlags fl);
    ~dEditDoc();

    aListViewItem *item;
    aAliasEditor *al;
    QHash<int, int> fields;
    aRoleEditor *re;

public slots:
    virtual void setData( aListViewItem * o );
    virtual void updateMD();
    virtual void eSv_activated( int index );

protected slots:
    virtual void languageChange();

    virtual void init();
    virtual void closeEditor();


};

#endif // DEDITDOC_H
