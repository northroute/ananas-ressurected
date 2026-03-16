#ifndef CATALOGFORM_H
#define CATALOGFORM_H

#include "ui_catalogform.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QList>
#include <QPixmap>
#include <QLabel>
#include <QKeyEvent>
#include <QVariant>
#include <QImage>

class CatalogForm : public QMainWindow, public Ui::CatalogForm
{
    Q_OBJECT

public:
    CatalogForm(QWidget* parent = 0, Qt::WindowFlags fl = 0);
    ~CatalogForm();

    aListView* ListView;
    QString fieldName;
    aLineEdit *LineEdit;
    aListBox* ListHint;
    QFrame* StatusFrame;
    aCatalogue* cat;
    bool FormToSelect;

    virtual qulonglong getId();
    virtual void init();
    virtual void destroy();
    virtual long findFirst( const QString & s );
    virtual qulonglong getIdg( qulonglong ide );
    virtual QPixmap getGroupPixmap();
    virtual void edit( QTreeWidgetItem * item, bool );
    virtual QPixmap getElementPixmap();
    virtual QPixmap getMarkDeletedPixmap();

public slots:
    virtual void setData( aCatalogue * catalog, QMap<qulonglong, QTreeWidgetItem *> mg, const QStringList & Fname, const QStringList & FnameGroup, qulonglong idElForm, qulonglong idGrForm, const bool toSelect );
    virtual void setId( qulonglong idx );
    virtual void find( const QString & s );
    virtual void pressArrow();
    virtual void isPressArrow();
    virtual void setText( const QString & s );
    virtual void go();
    virtual void goToItem( QTreeWidgetItem * item );
    virtual void loadElements( qulonglong idGroup );
    virtual void onLoadElements( QTreeWidgetItem * item );
    virtual void new_item( QTreeWidgetItem * parentItem );
    virtual void new_group( QTreeWidgetItem * parentItem );
    virtual void del_item( QTreeWidgetItem * item );
    virtual void mark_deleted( QTreeWidgetItem * item );
    virtual void undo_mark_deleted( QTreeWidgetItem * item );
    virtual void edit( QTreeWidgetItem * item, int );
    virtual void select( QTreeWidgetItem * item );
    virtual void Refresh( qulonglong id );

signals:
    void selected(qulonglong);

protected:
    QGridLayout* GridLayout;
    QVBoxLayout* layout1;
    qulonglong id,idElementForm,idGroupForm;
    QStringList fieldList, fieldListGroup;

    virtual long getGroupId( QTreeWidgetItem * item );
    virtual qulonglong getElementId( QTreeWidgetItem * item );

protected slots:
    virtual void languageChange();

    virtual void keyPressEvent( QKeyEvent * e );


private:
    QMap<qulonglong, QTreeWidgetItem*> map_gr;
    QMap<qulonglong, QTreeWidgetItem*> map_el;
    QMap<qulonglong,bool> map_deleted;

private slots:
    virtual void doOk();

};

#endif // CATALOGFORM_H
