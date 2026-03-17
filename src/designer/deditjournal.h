#ifndef DEDITJOURNAL_H
#define DEDITJOURNAL_H


#include "ui_deditjournal.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>

class dEditJournal : public QMainWindow, public Ui::dEditJournal
{
    Q_OBJECT

public:
    dEditJournal(QWidget* parent, Qt::WindowFlags fl);
    ~dEditJournal();

    aListViewItem *item;
    aAliasEditor *al;
    ananasTreeView * journalDocs;
    ananasTreeView * allDocs;
    aCfg *md;
    aCfgItem obj;
    QWidget * docPage;
    aRoleEditor *re;

public slots:
    virtual void setData( aListViewItem * o );
    virtual void updateMD();

protected slots:
    virtual void languageChange();

private:
    void init();
    void closeEditor();

private slots:
    virtual void typeChange();
    virtual void addDoc();
    virtual void removeDoc();
    virtual void moveUp();
    virtual void moveDown();

};

#endif // DEDITJOURNAL_H
