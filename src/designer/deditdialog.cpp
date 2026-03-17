#include "deditdialog.h"

#include "formdesigner.h"
#include "mainform.h"
#include "alog.h"
#include "acfg.h"

//#include "qananastable.h"
//#include <unistd.h>

//extern QApplication *application;

/*
 *  Constructs a dEditDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dEditDialog::dEditDialog(QWidget* parent, Qt::WindowFlags fl)
    : QMainWindow(parent, fl)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dEditDialog::~dEditDialog()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dEditDialog::languageChange()
{
    retranslateUi(this);
}

void dEditDialog::init()
{
	delete statusBar();
	//eModule->setInterpreter(new QSInterpreter());
//	fd = new aFormDesigner();
}


void dEditDialog::closeEditor()
{
    updateMD();
    ( (MainForm*)this->topLevelWidget() )->wl->remove( this );
    ((MainForm*)topLevelWidget())->removeTab(windowTitle());
}


void dEditDialog::setData(aListViewItem *o)
{
    item = NULL;
    QString parentClass;

    if (o) {
        item = o;
        aCfg *md = o->md;
        aCfgItem obj = o->obj;

        aAliasEditor *a = new aAliasEditor(md, obj, tAliases);
        al = a;
        al->setData();

        eName->setText(md->attr(obj, mda_name));
        eDescription->setPlainText(md->sText(obj, md_description));
        eModule->setPlainText(md->sText(obj, md_sourcecode));
        eFormFile->setText(QString("inputform_") + QString::number(item->id) + QString(".ui"));

        setWindowTitle(tr("Form:") + eName->text());

        parentClass = md->objClass(md->parent(md->parent(obj)));

        if (parentClass == md_catalogue) {
            cbFormMode->clear();
            cbFormMode->addItem(QObject::tr("Elements list dialog"));
            cbFormMode->addItem(QObject::tr("Element dialog"));
            cbFormMode->addItem(QObject::tr("Group dialog"));
        } else {
            cbSelect->setHidden(true);
        }

        int i = md->sText(obj, md_defaultmod).toInt();
        cbNew->setChecked((i >> md_form_new) % 2);
        cbView->setChecked((i >> md_form_view) % 2);
        cbEdit->setChecked((i >> md_form_edit) % 2);
        cbSelect->setChecked((i >> md_form_select) % 2);

        i = md->attr(obj, mda_type).toInt();
        cbFormMode->setCurrentIndex(i);

        i = md->attr(obj, mda_readonly).toInt();
        cbReadOnly->setChecked(i);

        QString ui = md->sText(obj, md_formdesign);
        Q_UNUSED(ui);
    }
}

//#include "mainform.h"
//#include <formwindow.h>
//extern MainForm *mainform;


void dEditDialog::EditForm()
{
    aCfg *md = item->md;
    aCfgItem obj = item->obj, o;
    QString ui, tpl_name = "inputform.ui.tpl", tpldir;
    QFile f(eFormFile->text());

    aLog::print(aLog::Debug, tr("dEditDialog edit form"));

#ifdef Q_OS_WIN32
    tpldir = qApp->applicationDirPath() + "/templates/";
#else
    tpldir = "/usr/share/ananas/templates/";
#endif
    tpldir = ":/designer/templates/";

    if (!f.exists())
    {
        aLog::print(aLog::Debug, tr("dEditDialog form file does not exists"));
        ui = md->sText(obj, md_formdesign);

        if (!ui.isEmpty())
        {
            aLog::print(aLog::Debug, tr("dEditDialog create form file from metadata"));
            QFile f1(eFormFile->text());
            if (f1.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                QTextStream ts(&f1);
                ts.setCodec("UTF-8");
                ts << ui;
                f1.close();
            }
        }
        else
        {
            aLog::print(aLog::Debug, tr("dEditDialog create new form file from template"));

            o = md->parent(md->parent(obj));
            if (md->objClass(o) == md_document)  tpl_name = "inputformdoc.tpl";
            if (md->objClass(o) == md_catalogue) tpl_name = "inputformcat.tpl";
            if (md->objClass(o) == md_journal)   tpl_name = "inputformjourn.tpl";
            if (md->objClass(o) == md_report)    tpl_name = "inputformrep.tpl";

            QFile fi(tpldir + tpl_name);
            if (fi.open(QIODevice::ReadOnly))
            {
                if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
                {
                    QTextStream tso(&f);
                    tso.setCodec("UTF-8");

                    QTextStream tsi(&fi);
                    tsi.setCodec("UTF-8");
                    QString form = tsi.readAll();

                    form.replace(QString("$$$id$$$"), QString("%1").arg(md->id(o)));
                    form.replace(QString("$$$name$$$"), QString(""));
                    form.replace(QString("$$$caption$$$"), md->attr(o, mda_name));

                    tso << form;

                    fi.close();
                    f.close();
                }
                else
                {
                    aLog::print(aLog::Error, tr("dEditDialog open file %1 for write").arg(f.fileName()));
                }
            }
            else
            {
                aLog::print(aLog::Error,
                            tr("dEditDialog open file %1 for read from templates directory %2")
                                .arg(tpl_name).arg(tpldir));
            }
        }
    }

    if (!QFile(eFormFile->text()).exists())
    {
        aLog::print(aLog::Error, tr("dEditDialog file %1 not exists").arg(eFormFile->text()));
    }

    formdesigner->fileOpen(eFormFile->text());
    aLog::print(aLog::Debug, tr("dEditDialog show formdesigner"));
    formdesigner->show();
}


//
//	if (!proc.start()) {
//		printf("Error running qt designer\n");
//	}
//CHECK_POINT
/*
    if ((rc=fork())==0) {
	arg = strdup( (const char *) eFormFile->text() );
	execlp("designer","designer","-client",arg,0);
	printf("Error running qt designer: errno=%d\terror=%s\n",
		      errno,strerror(errno));
    } else if (rc==-1) {
	printf("Error while fork: errno=%d\terror=%s\n",
		      errno,strerror(errno));
    }
*/


void dEditDialog::EditModule()
{
//	cfgobj_editcode(objectptr);
}


void dEditDialog::updateMD()
{
    formdesigner->fileSaveAll();

    aCfg *md = item->md;
    aCfgItem obj = item->obj;
    QString ui;
    QFile f(eFormFile->text());

    al->updateMD();

    item->setText(0, eName->text().trimmed());
    md->setAttr(obj, mda_name, eName->text().trimmed());
    md->setAttr(obj, mda_type, QString::number(cbFormMode->currentIndex()));
    md->setAttr(obj, mda_readonly, cbReadOnly->isChecked() ? "1" : "0");
    md->setSText(obj, md_description, eDescription->toPlainText());
    md->setSText(obj, md_sourcecode, eModule->toPlainText());

    int i = (1 << md_form_new) * cbNew->isChecked() +
            (1 << md_form_view) * cbView->isChecked() +
            (1 << md_form_edit) * cbEdit->isChecked() +
            (1 << md_form_select) * cbSelect->isChecked();

    md->setSText(obj, md_defaultmod, QString("%1").arg(i));

    if (f.open(QIODevice::ReadOnly)) {
        QTextStream ts(&f);
        ts.setCodec("UTF-8");
        ui = ts.readAll();
        md->setSText(obj, md_formdesign, ui);
        f.close();
        f.remove();
    }
}


void dEditDialog::formPreview()
{
    aCfg *md = item->md;
    aCfgItem obj = item->obj;
    QString ui;
    QFile f(eFormFile->text());

    if (!f.exists()) {
        ui = md->sText(obj, md_formdesign);
        if (!ui.isEmpty()) {
            if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                QTextStream ts(&f);
                ts.setCodec("UTF-8");
                ts << ui;
                f.close();
            }
        } else {
            return;
        }
    }

    if (!f.open(QIODevice::ReadOnly))
        return;

    QFormBuilder fb;
    QWidget *form = fb.load(&f);
    f.close();

    if (form)
        form->show();
    else
        printf("Error create form\n");
}


int dEditDialog::getDocId()
{
    return docId;
}
