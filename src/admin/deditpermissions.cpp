#include "deditpermissions.h"

//#include "deditpermissions.ui.h"
/*
 *  Constructs a dEditPermissions as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
dEditPermissions::dEditPermissions(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
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
dEditPermissions::~dEditPermissions()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditPermissions::languageChange()
{
    retranslateUi(this);
}


void dEditPermissions::setData(int pm)
{
    int r = 1;

    for (int i = 1; i <= 7; i++)
    {
        QCheckBox *c = findChild<QCheckBox*>(QString("checkBox%1").arg(i));
        if (c)
            c->setChecked((pm & r) != 0);

        r <<= 1;
    }
}


int dEditPermissions::getData()
{
    int r = 1, res = 0;

    for (int i = 1; i <= 7; i++)
    {
        QCheckBox *c = findChild<QCheckBox*>(QString("checkBox%1").arg(i));
        if (c && c->isChecked())
            res |= r;

        r <<= 1;
    }

    return res;
}


void dEditPermissions::setAll(bool v)
{
    for (int i = 1; i <= 7; i++)
    {
        QCheckBox *c = findChild<QCheckBox*>(QString("checkBox%1").arg(i));
        if (c) c->setChecked(v);
    }
}
