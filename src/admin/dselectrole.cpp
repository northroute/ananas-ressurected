#include "dselectrole.h"

/*
 *  Constructs a dSelectRole as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
dSelectRole::dSelectRole(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
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
dSelectRole::~dSelectRole()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dSelectRole::languageChange()
{
    retranslateUi(this);
}

void dSelectRole::setData(aUser *user)
{
    usr = user;

    listBox1->clear();
    listId.clear();

    QList<aRole*> list = user->getRoles(false);

    for (QList<aRole*>::iterator it = list.begin(); it != list.end(); ++it)
    {
        listBox1->addItem((*it)->sysValue("name").toString());
        listId.append((*it)->sysValue("id").toString());
        delete (*it);
    }
}


void dSelectRole::onSelect()
{
    int row = listBox1->currentRow();
    if (row != -1)
    {
        printf("add role\n");

        roleId = listId[row].toULongLong();
        usr->addRole(roleId);

        delete listBox1->takeItem(row);
        accept();
    }
    else
    {
        printf("not selected\n");
    }
}


quint64 dSelectRole::getData()
{
    return roleId;
}
