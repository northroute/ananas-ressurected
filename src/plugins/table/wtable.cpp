/****************************************************************************
** $Id: wtable.cpp,v 1.1 2008/11/05 21:16:30 leader Exp $
**
** Code file of the table plugin of Ananas
** Designer and Engine applications
**
** Created : 20031201
**
** Copyright (C) 2003-2004 Leader InfoTech.  All rights reserved.
** Copyright (C) 2006 Grigory Panov <gr1313 at mail.ru>, Yoshkar-Ola.
**
** This file is part of the Ananas Plugins of the Ananas
** automation accounting system.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.leaderit.ru/page=ananas or email sales@leaderit.ru
** See http://www.leaderit.ru/gpl/ for GPL licensing information.
**
** Contact org@leaderit.ru if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "acfg.h"
#include "aobject.h"
#include "awidget.h"
#include "wtable.h"



wTable::wTable(QWidget *parent)
    : QTableWidget(parent)
{
}

wTable::~wTable()
{
}


void
wTable::setDocument(int row, aObject *object)
{
}

void wTable::setText(int row, int col, const QString &text)
{
    QTableWidgetItem *item = this->item(row, col);
    if (!item) {
        item = new QTableWidgetItem(text);
        setItem(row, col, item);
    } else {
        item->setText(text);
    }
}

QString wTable::text(int col, int row)
{
    QTableWidgetItem *item = this->item(row, col);
    return item ? item->text() : QString();
}

void wTable::setHeaderText(int col, const QString &text)
{
    QTableWidgetItem *header = horizontalHeaderItem(col);
    if (!header) {
        header = new QTableWidgetItem(text);
        setHorizontalHeaderItem(col, header);
    } else {
        header->setText(text);
    }
}

QString wTable::textHeader(int col)
{
    QTableWidgetItem *header = horizontalHeaderItem(col);
    return header ? header->text() : QString();
}

void wTable::sortColumn(int col, bool ascending, bool wholeRows)
{
    Q_UNUSED(wholeRows);
    printf("do sort column\n");
    sortItems(col, ascending ? Qt::AscendingOrder : Qt::DescendingOrder);
}

void wTable::setColumnWidth(int col, int width)
{
    QTableWidget::setColumnWidth(col, width);
}

int wTable::columnWidth(int col)
{
    return QTableWidget::columnWidth(col);
}

void wTable::setNumCols(int r)
{
    setColumnCount(r);
}

int wTable::numCols() const
{
    return columnCount();
}

void wTable::hideColumn(int col)
{
    QTableWidget::hideColumn(col);
}

void wTable::showColumn(int col)
{
    QTableWidget::showColumn(col);
}

/*
bool
wTable::isColumnHidden ( int col ) const
{
//	int i = -2;
//	if(col>=0) i=col;
	return QTable::isColumnHidden ( col );
}
*/

void wTable::adjustColumn(int col)
{
    resizeColumnToContents(col);
}

void wTable::setColumnStretchable(int col, bool stretch)
{
    QHeaderView *h = horizontalHeader();
    if (stretch)
        h->setResizeMode(col, QHeaderView::Stretch);
    else
        h->setResizeMode(col, QHeaderView::Interactive);
}

bool wTable::isColumnStretchable(int col) const
{
    return horizontalHeader()->resizeMode(col) == QHeaderView::Stretch;
}

void wTable::swapColumns(int col1, int col2, bool swapHeader)
{
    for (int row = 0; row < rowCount(); ++row) {
        QTableWidgetItem *item1 = takeItem(row, col1);
        QTableWidgetItem *item2 = takeItem(row, col2);

        setItem(row, col1, item2);
        setItem(row, col2, item1);
    }

    if (swapHeader) {
        QTableWidgetItem *h1 = takeHorizontalHeaderItem(col1);
        QTableWidgetItem *h2 = takeHorizontalHeaderItem(col2);

        setHorizontalHeaderItem(col1, h2);
        setHorizontalHeaderItem(col2, h1);
    }
}

void wTable::swapCells(int row1, int col1, int row2, int col2)
{
    QTableWidgetItem *item1 = takeItem(row1, col1);
    QTableWidgetItem *item2 = takeItem(row2, col2);

    setItem(row1, col1, item2);
    setItem(row2, col2, item1);
}

void wTable::setCurrentCell(int row, int col)
{
    QTableWidget::setCurrentCell(row, col);
}

void wTable::setColumnReadOnly(int col, bool ro)
{
    for (int row = 0; row < rowCount(); ++row) {
        QTableWidgetItem *item = this->item(row, col);
        if (!item) {
            item = new QTableWidgetItem();
            setItem(row, col, item);
        }

        Qt::ItemFlags flags = item->flags();
        if (ro)
            item->setFlags(flags & ~Qt::ItemIsEditable);
        else
            item->setFlags(flags | Qt::ItemIsEditable);
    }
}

void wTable::insertColumns(int col, int count)
{
    for (int i = 0; i < count; ++i)
        insertColumn(col);
}

void wTable::removeColumn(int col)
{
    QTableWidget::removeColumn(col);
}

void wTable::editCell(int row, int col, bool replace)
{
    Q_UNUSED(replace);
    editItem(item(row, col));
}

void wTable::columnClicked(int col)
{
    emit cellClicked(0, col);
}

void wTable::swapRows(int row1, int row2, bool swapHeader)
{
    Q_UNUSED(swapHeader);

    for (int col = 0; col < columnCount(); ++col) {
        QTableWidgetItem *item1 = takeItem(row1, col);
        QTableWidgetItem *item2 = takeItem(row2, col);

        setItem(row1, col, item2);
        setItem(row2, col, item1);
    }
}
