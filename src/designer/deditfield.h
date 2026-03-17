#ifndef DEDITFIELD_H
#define DEDITFIELD_H


#include "ui_deditfield.h"

#include <QVariant>
#include <QImage>
#include <QPixmap>
#include <QStatusBar>

class dEditField : public QMainWindow, public Ui::dEditField
{
    Q_OBJECT

public:
    dEditField(QWidget* parent, Qt::WindowFlags fl);
    ~dEditField();

    QStringList otypes;
    QStringList artypes;
    aListViewItem *item;
    aAliasEditor *al;

public slots:
    virtual void setData( aListViewItem * o );
    virtual void updateMD();
    virtual void typeSelect( int idx );
    virtual void nameChanged();
    virtual void AARegSelect( int i );

protected slots:
    virtual void languageChange();

private:
    QString capt;

    void closeEditor();
    void init();

};

#endif // DEDITFIELD_H
