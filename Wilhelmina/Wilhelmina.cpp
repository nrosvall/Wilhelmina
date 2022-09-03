/*
* Wilhelmina
*
* Copyright (C) 2022  Niko Rosvall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
#include <qclipboard.h>
#include <qdesktopservices.h>
#include <qurl.h>
#include "PreferencesDialog.h"

Wilhelmina::Wilhelmina(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.lineEdit->setPlaceholderText("Type to search");
   
    m_DataPath = Settings.value("DatafileLocation", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Wilhelmina/").toString();
    
    connect(ui.listWidget, &QListWidget::itemDoubleClicked, this, &Wilhelmina::listItemDoubleClicked);
    connect(ui.listWidget, &QListWidget::itemClicked, this, &Wilhelmina::listItemClicked);
    connect(ui.listWidget, &QListWidget::itemSelectionChanged, this, &Wilhelmina::listItemSelectionChanged);

    ui.actionDelete->setEnabled(false);
    ui.actionCopyPassword->setEnabled(false);
    ui.actionCopyUsername->setEnabled(false);
    ui.actionOpen_in_Browser->setEnabled(false);

    m_IsEncrypted = false;
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
    QTimer::singleShot(0, this, SLOT(PostActivate()));
}

void Wilhelmina::encryptOnWindowStateEvent() {
    if (!m_IsEncrypted) {
        if (m_Entries.Encrypt(m_MasterPassword, m_DataPath)) {
            m_MasterPassword.clear();
            ui.listWidget->clear();
            m_IsEncrypted = true;
        }
        else {
            QMessageBox::critical(this, "Wilhelmina", 
                "Encryption failed.\nDo you have permission to write into the data location:\n" + m_DataPath + " ?",
                                  QMessageBox::Ok);
        }
    }
}

void Wilhelmina::closeEvent(QCloseEvent* ev) {

    //Want to encrypt? What if user kills the process? Should we check do we have a master passphrase?
    //It should be enough to check the master password, if empty, skip encryption
    if(!m_MasterPassword.isEmpty())
        encryptOnWindowStateEvent();
    
    QMainWindow::closeEvent(ev);
}

void Wilhelmina::changeEvent(QEvent* ev) {

    if (ev->type() == QEvent::WindowStateChange) {
        switch (windowState())
        {
        case Qt::WindowMinimized:
            encryptOnWindowStateEvent();
            break;
        default:
            break;
        }
    }

    ev->accept();
}

void Wilhelmina::PostActivate()
{
    if (!QDir(m_DataPath).exists()) {
        MasterPasswordDialog dlg(true, this);
        if (dlg.exec() == QDialog::Accepted) {
            m_MasterPassword = dlg.GetPassphrase();
            if (!QDir().mkpath(m_DataPath)) {
                QMessageBox::critical(this, "Wilhelmina", "Unable to create path " + m_DataPath + ".\n Abort.",
                                      QMessageBox::Ok);
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
            
            MasterPasswordDialog dlg(false, true, this);
            dlg.SetCanReject(false);
            if (dlg.exec() == QDialog::Accepted) {
                m_MasterPassword = dlg.GetPassphrase();

                if (m_Entries.Decrypt(m_MasterPassword, m_DataPath)) {
                    m_IsEncrypted = false;
                    populateViewFromEntries();
                }
                else {
                    m_MasterPassword.clear();
                    PostActivate();
                }
            }
        }
        else {
            //We don't have any data even if the directory path exists so we need to ask for the user to set up the master passphrase.
            m_IsEncrypted = false;
            MasterPasswordDialog dlg(true, this);
            if (dlg.exec() == QDialog::Accepted)
                m_MasterPassword = dlg.GetPassphrase();
            else
                QApplication::quit();
        }
    }
}

void Wilhelmina::listItemClicked(QListWidgetItem* item) {
    if (item->isSelected()) {
        ui.actionDelete->setEnabled(true);
        ui.actionCopyPassword->setEnabled(true);
        ui.actionCopyUsername->setEnabled(true);
        ui.actionOpen_in_Browser->setEnabled(true);
    }
}

void Wilhelmina::listItemDoubleClicked(QListWidgetItem *item) {
    
    CustomListWidgetItem* ci =  static_cast<CustomListWidgetItem *>(item);
    QString id = ci->getID();
    QJsonObject obj = m_Entries.GetJObject(id);

    AddNewEntry dlg("View Entry", true, &obj, &Settings, this);

    if (dlg.exec() == QDialog::Accepted) {
        m_Entries.deleteItem(ci->getID());
        ui.listWidget->removeItemWidget(item);
        delete item;
        AddNewEntryToMemory(dlg.GetTitle(), dlg.GetUsername(), dlg.GetPassword(), dlg.GetUrl(), dlg.GetNotes());
    }
}

void Wilhelmina::encryptAndLock() {
        this->showMinimized();
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

    if (!ui.actionEncrypt->isEnabled())
        ui.actionEncrypt->setEnabled(true);
}

void Wilhelmina::AddNewEntryToMemory(QString title, QString user, QString password, QString url, QString notes) {

    QString ID = QUuid::createUuid().toString();
    m_Entries.AddEntry(title, user, password, url, notes, ID);
    
    AddEntryToView(title, ID);
}

void Wilhelmina::addNewEntry() {
    AddNewEntry dlg("Add New Entry", false, nullptr, &Settings, this);

    if (dlg.exec() == QDialog::Accepted) {
        AddNewEntryToMemory(dlg.GetTitle(), dlg.GetUsername(), dlg.GetPassword(), dlg.GetUrl(), dlg.GetNotes());
    }
}

void Wilhelmina::deleteSelectedItem() {
    CustomListWidgetItem* item = static_cast<CustomListWidgetItem*>(ui.listWidget->selectedItems()[0]);
    m_Entries.deleteItem(item->getID());
    ui.listWidget->takeItem(ui.listWidget->currentRow());
    delete item;
}

void Wilhelmina::listItemSelectionChanged() {
    if (ui.listWidget->selectedItems().count() == 0) {
        ui.actionDelete->setEnabled(false);
        ui.actionCopyPassword->setEnabled(false);
        ui.actionCopyUsername->setEnabled(false);
        ui.actionOpen_in_Browser->setEnabled(false);
    }
}

void Wilhelmina::copyUsername() {
    QClipboard* cBoard = QApplication::clipboard();
    CustomListWidgetItem* item = static_cast<CustomListWidgetItem*>(ui.listWidget->selectedItems()[0]);
   
    cBoard->setText(m_Entries.GetJObject(item->getID()).value("user").toString());
}

void Wilhelmina::copyPassword() {
    QClipboard* cBoard = QApplication::clipboard();
    CustomListWidgetItem* item = static_cast<CustomListWidgetItem*>(ui.listWidget->selectedItems()[0]);
    
    cBoard->setText(m_Entries.GetJObject(item->getID()).value("password").toString());
}

void Wilhelmina::openInBrowser() {
    CustomListWidgetItem* item = static_cast<CustomListWidgetItem*>(ui.listWidget->selectedItems()[0]);
    QJsonObject obj = m_Entries.GetJObject(item->getID());
    QString url = obj.value("url").toString();
    if (!url.isEmpty()) {
        if (!url.startsWith("http"))
            url = "https://" + url;
        QDesktopServices::openUrl(QUrl(url));
    }
    else {
        QMessageBox::warning(this, "Wilhelmina", "The entry has no URL set.\nEdit the entry to add one.", QMessageBox::Ok);
    }
}

void Wilhelmina::showPreferences() {
    PreferencesDialog dlg(&Settings, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_DataPath = dlg.dataFileLocation();
        
        if (QFile::exists(m_DataPath + m_Entries.encryptedBlobFile())) {
            if (m_Entries.Decrypt(m_MasterPassword, m_DataPath)) {
                m_IsEncrypted = false;

                //TODO: We should here check if current listview entries are not in entryarray and add them there?
                //Or should we just start saving (encrypting) after every add/remove operation?

                populateViewFromEntries();
            }
            else {
                m_MasterPassword.clear();
                PostActivate();
            }
        }

        //TODO: update interval timer
    }
}
