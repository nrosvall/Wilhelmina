#include "Wilhelmina.h"
#include "AddNewEntry.h"
#include <qjsondocument.h>
#include <qlistwidget.h>
#include <QListWidgetItem>
#include <qstandardpaths.h>
#include <qdir.h>
#include <quuid.h>
#include <qmessagebox.h>
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
    connect(ui.listWidget, &QListWidget::itemClicked, this, &Wilhelmina::listItemClicked);
    connect(ui.listWidget, &QListWidget::itemSelectionChanged, this, &Wilhelmina::listItemSelectionChanged);

    ui.actionDelete->setEnabled(false);
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
    QTimer::singleShot(0, this, SLOT(PostStart()));
}

void Wilhelmina::PostStart()
{
    if (!QDir(m_DataPath).exists()) {
        MasterPasswordDialog dlg(true, this);
        if (dlg.exec() == QDialog::Accepted) {
            m_MasterPassword = dlg.GetPassphrase();
            if (!QDir().mkpath(m_DataPath)) {
                QMessageBox box(this);
                box.setText("Unable to create path " + m_DataPath + ". Abort.");
                box.exec();
                QApplication::quit();
            }
        }
        else {
            QApplication::quit();
        }
    }
    else {
        //We have our data path, do we have any encrypted data?
        if (QFile::exists(m_DataPath + m_Entries.encryptedBlobFile())) {
            if (m_MasterPassword.isEmpty()) {
                MasterPasswordDialog dlg(false, this);
                dlg.SetCanReject(false);
                if (dlg.exec() == QDialog::Accepted) {
                    m_MasterPassword = dlg.GetPassphrase();

                    if (m_Entries.Decrypt(m_MasterPassword, m_DataPath)) {
                        populateViewFromEntries();
                    }
                }
            }
        }
        else {
            //TODO: Implement, what the program should do if there are no data file? Nothing?
        }
    }
}

void Wilhelmina::listItemClicked(QListWidgetItem* item) {
    if(item->isSelected())
        ui.actionDelete->setEnabled(true);
}

void Wilhelmina::listItemDoubleClicked(QListWidgetItem *item) {
    
    CustomListWidgetItem* ci =  static_cast<CustomListWidgetItem *>(item);

    QString id = ci->getID();
    
    QJsonObject obj = m_Entries.GetJObject(id);

    qDebug() << obj.value("title").toString();

    int i = 0;
}

void Wilhelmina::encryptAndLock() {
    if (m_Entries.Encrypt(m_MasterPassword, m_DataPath)) {
        m_MasterPassword.clear();
        ui.listWidget->clear();
        this->showMinimized();
    }
}

void Wilhelmina::populateViewFromEntries() {
    for (auto oneEntry : m_Entries.entryArray()) {
        QJsonObject entry = oneEntry.toObject();
        AddEntryToView(entry.value("title").toString(), entry.value("ID").toString());
    }
}

void Wilhelmina::AddEntryToView(QString title, QString ID) {
    CustomListWidgetItem* item = new CustomListWidgetItem();
    item->setText(title);
    item->setID(ID);

    ui.listWidget->addItem(item);
}

void Wilhelmina::AddNewEntryToMemory(QString title, QString user, QString password, QString url, QString notes) {

    QString ID = QUuid::createUuid().toString();
    m_Entries.AddEntry(title, user, password, url, notes, ID);
    
    AddEntryToView(title, ID);
}

void Wilhelmina::addNewEntry() {
    AddNewEntry dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        AddNewEntryToMemory(dlg.GetTitle(), dlg.GetUsername(), dlg.GetPassword(), dlg.GetUrl(), dlg.GetNotes());
    }
}

void Wilhelmina::deleteSelectedItem() {
    CustomListWidgetItem* item = static_cast<CustomListWidgetItem*>(ui.listWidget->selectedItems()[0]);
    m_Entries.deleteItem(item->getID());
    ui.listWidget->takeItem(ui.listWidget->currentRow());
}

void Wilhelmina::listItemSelectionChanged() {
    if (ui.listWidget->selectedItems().count() == 0)
        ui.actionDelete->setEnabled(false);
}
