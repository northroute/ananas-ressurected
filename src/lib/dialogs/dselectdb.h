#ifndef DSELECTDB_H
#define DSELECTDB_H

#include "ui_dselectdb.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QDir>          // вместо <qdir.h>
#include <QString>       // вместо <qstring.h>
#include <QFileDialog>   // вместо <q3filedialog.h>
#include <QLineEdit>     // вместо <qlineedit.h>
#include <QDomDocument>  // вместо <qdom.h>

class ANANAS_EXPORT dSelectDB : public QDialog, public Ui::dSelectDB
{
    Q_OBJECT

public:
    dSelectDB(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~dSelectDB();

    QString rcfile;

public slots:
    void newItem();
    void editItem();
    void deleteItem();
    void onCancel();
    void ItemRenamed(QTreeWidgetItem *item, int col);
    void onHelp();
    void createMenu();
    void onDblClick(QTreeWidgetItem *item, int col);
    void itemSelect(QTreeWidgetItem *item, int col);
    void onOK();

protected:
    QSettings settings;

    virtual void clearSettings();

protected slots:
    virtual void languageChange();

private:
    int withgroups;
    QMenuBar *menuBar;
    bool changes;
    int ro_mode;
    bool local;

private slots:
    void init();
    void readSettings( QStringList entryGroup );
    void newGroup();
    void saveRC();
    void importItem();
    void exportItem();

};

#endif // DSELECTDB_H
