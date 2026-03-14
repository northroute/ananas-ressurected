#include "deditrole.h"


/*
 *  Constructs a dEditRole as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
dEditRole::dEditRole(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);

    if (modal)
        setModal(true);

    if (name)
        setObjectName(name);
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditRole::~dEditRole()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditRole::languageChange()
{
    retranslateUi(this);
}


void
dEditRole::setData( const QString &s )
{
	lineEdit1->setText(s);
}


QString 
dEditRole::getData()
{
	return lineEdit1->text();
}
