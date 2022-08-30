#include "Wilhelmina.h"
#include "AddNewEntry.h"
#include <qjsondocument.h>
#include <qlistwidget.h>
#include <QListWidgetItem>
#include <qstandardpaths.h>
#include <qdir.h>
#include <quuid.h>
#include "MasterPasswordDialog.h"
#include <windows.h>
#include <wincrypt.h>
#include <qtimer.h>

Wilhelmina::Wilhelmina(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.lineEdit->setPlaceholderText("Type to search");
    //ui.actionNew->setEnabled(false);

    m_DataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Wilhelmina/";
    
    connect(ui.listWidget, &QListWidget::itemDoubleClicked, this, &Wilhelmina::listItemDoubleClicked);
}

Wilhelmina::~Wilhelmina()
{
    
}

void Wilhelmina::ProtectMasterPassphrase() {
    //CryptProtectMemory(&m_MasterPassword, m_MasterPassword.length(), CRYPTPROTECTMEMORY_SAME_PROCESS);
}

void Wilhelmina::UnProtectMasterPassphrase() {
    //CryptUnprotectMemory(&m_MasterPassword, m_MasterPassword.length(), CRYPTPROTECTMEMORY_SAME_PROCESS);
}

void Wilhelmina::showEvent(QShowEvent* ev)
{
    QMainWindow::showEvent(ev);
    QTimer::singleShot(0, this, SLOT(firstRun()));
}

void Wilhelmina::firstRun()
{
    if (!QDir(m_DataPath).exists()) {
        MasterPasswordDialog dlg(true, this);
        if (dlg.exec() == QDialog::Accepted) {
            m_MasterPassword = dlg.GetPassphrase();
        }
        else {
            QApplication::quit();
        }
    }
}

void Wilhelmina::listItemDoubleClicked(QListWidgetItem *item) {
    
    CustomListWidgetItem* ci =  static_cast<CustomListWidgetItem *>(item);

    QString id = ci->getID();

    QJsonObject obj = m_Entries.GetJObject(id);

    qDebug() << obj.value("title").toString();

    int i = 0;
}

void Wilhelmina::AddNewEntryToMemory(QString title, QString user, QString password, QString url, QString notes) {

    QString ID = QUuid::createUuid().toString();
    m_Entries.AddEntry(title, user, password, url, notes, ID);
    
    CustomListWidgetItem *item = new CustomListWidgetItem();
    item->setText(title);
    item->setID(ID);

    ui.listWidget->addItem(item);
}

void Wilhelmina::addNewEntry() {
    AddNewEntry dlg;

    if (dlg.exec() == QDialog::Accepted) {
        AddNewEntryToMemory(dlg.GetTitle(), dlg.GetUsername(), dlg.GetPassword(), dlg.GetUrl(), dlg.GetNotes());
    }

    QJsonDocument doc;
    //m_rootEntry.insert("Entries", m_EntryArray);

    //doc.setObject(m_rootEntry);

    QByteArray bytes = doc.toJson();

    qDebug() << bytes;
}
