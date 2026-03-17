#ifndef DEDITDIALOG_H
#define DEDITDIALOG_H

#include "ui_deditdialog.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QTextCodec>
#include <QString>
#include <QFile>
#include <QStatusBar>
#include <QDialog>
#include <QFormBuilder>
#include <QObject>
#include <QTextStream>
#include <QBuffer>
#include <QProcess>
#include <QApplication>
#include <QScriptEngine>

class dEditDialog : public QMainWindow, public Ui::dEditDialog
{
    Q_OBJECT

public:
    dEditDialog(QWidget* parent, Qt::WindowFlags fl);
    ~dEditDialog();

    int docId;
    aAliasEditor *al;
    aFormDesigner *fd;
    aListViewItem *item;

    virtual int getDocId();

public slots:
    virtual void init();
    virtual void closeEditor();
    virtual void setData( aListViewItem * o );
    virtual void EditForm();
    virtual void EditModule();
    virtual void updateMD();

protected slots:
    virtual void languageChange();

private slots:
    virtual void formPreview();

};

#endif // DEDITDIALOG_H
