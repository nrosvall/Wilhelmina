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

#define WIN32_LEAN_AND_MEAN

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
#include <wincrypt.h>
#include <qtimer.h>
#include <qclipboard.h>
#include <qdesktopservices.h>
#include <qurl.h>
#include "PreferencesDialog.h"
#include <qfiledialog.h>
#include "SSHsync.h"
#include <QMultiMap>
#include "DuplicateDialog.h"
#include "AboutDialog.h"
#include <WtsApi32.h>
#include "DuplicateEntry.h"

Wilhelmina::Wilhelmina(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.lineEditSearch->setPlaceholderText("Type to search");
   
    this->restoreGeometry(Settings.value("geometry").toByteArray());

    m_DataPath = Settings.value("DatafileLocation", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Wilhelmina/").toString();

    if (!Settings.contains("Profiles")) {
        QList<QString> profiles;
        profiles.append(m_DataPath);
        Settings.setValue("Profiles", QVariant::fromValue(profiles));
    }

    m_statusLabel = new QLabel();
    this->setWindowTitle("Wilhelmina - " + m_DataPath);
    ui.statusBar->addPermanentWidget(m_statusLabel);

    ui.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    populateProfileMenu();

    connect(ui.listWidget, &QListWidget::itemDoubleClicked, this, &Wilhelmina::listItemDoubleClicked);
    connect(ui.listWidget, &QListWidget::itemClicked, this, &Wilhelmina::listItemClicked);
    connect(ui.listWidget, &QListWidget::itemSelectionChanged, this, &Wilhelmina::listItemSelectionChanged);
    connect(ui.listWidget, &QListWidget::customContextMenuRequested, this, &Wilhelmina::showContextMenu);

    ui.actionDelete->setEnabled(false);
    ui.actionCopyPassword->setEnabled(false);
    ui.actionCopyUsername->setEnabled(false);
    ui.actionOpen_in_Browser->setEnabled(false);
    ui.actionEdit->setEnabled(false);
    ui.actionClone->setEnabled(false);

    WTSRegisterSessionNotification((HWND)winId(), NOTIFY_FOR_THIS_SESSION);

    m_cryptoState.setState(true);
}

Wilhelmina::~Wilhelmina()
{
   delete m_statusLabel;
   WTSUnRegisterSessionNotification((HWND)winId());
}

void Wilhelmina::addProfileAction(QString title) {
    
    bool hasAction = false;

    foreach(QAction * action, ui.menuProfiles->actions()) {
        if (action->text() == title) {
            hasAction = true;
            break;
        }
    }

    if (!hasAction) {
        QAction* act = new QAction(title);
        ui.menuProfiles->addAction(act);
        connect(act, &QAction::triggered, this, [=]() { changeProfile(title); });
    }

}

void Wilhelmina::populateProfileMenu() {
    ui.menuProfiles->clear();
    QList<QString> toBeRemoved;
    QList<QString> profiles = Settings.value("Profiles").value<QList<QString>>();
    for (auto &item : profiles) {
        if (QDir(item).exists()) {
            addProfileAction(item);
        }
        else {
            //Situation where the last known data location was deleted from the file system.
            //Just create the missing path again and add it as a profile
            if (item == m_DataPath) {
                QDir().mkpath(item);
                profiles.append(item);
                addProfileAction(item);
            }
            else {
                //If the profile is missing from the filesystem and it's not the last one in use, delete it completely.
                toBeRemoved.append(item);
            }
        }
    }

    for (auto& item : toBeRemoved)
        profiles.removeAll(item);

    Settings.setValue("Profiles", QVariant::fromValue(profiles));
}

void Wilhelmina::changeProfile(QString profilePath) {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    applyNewProfile(profilePath);
    QApplication::restoreOverrideCursor();
}

void Wilhelmina::setIdleFilter(IdleFilter* filter) {
    filter->setCryptoStateInstance(&m_cryptoState);
    m_IdleFilter = filter;
}

void Wilhelmina::ProtectMasterPassphrase() {

    if (_MasterPassword) {
        if (!CryptProtectMemory(_MasterPassword, _dwMasterPassphraseLength, CRYPTPROTECTMEMORY_SAME_PROCESS)) {
            QMessageBox::critical(this, "Wilhelmina",
                "Protecting master passphrase failed.",
                QMessageBox::Ok);
        }
    }
}

void Wilhelmina::UnProtectMasterPassphrase() {
 
    if (_MasterPassword) {
        if (!CryptUnprotectMemory(_MasterPassword, _dwMasterPassphraseLength, CRYPTPROTECTMEMORY_SAME_PROCESS)) {
            QMessageBox::critical(this, "Wilhelmina",
                "Unprotecting master passphrase failed.",
                QMessageBox::Ok);
        }
    }
}

void Wilhelmina::SetMasterPassphrase(QString p) {

    if (_MasterPassword != NULL) {
        SecureZeroMemory(_MasterPassword, sizeof(_MasterPassword));
        GlobalFree(_MasterPassword);
    }

    DWORD dwPlainSize = (wcslen(p.toStdWString().c_str()) + 1) * sizeof(wchar_t);
    DWORD dwMod = 0;

    if (dwMod = dwPlainSize % CRYPTPROTECTMEMORY_BLOCK_SIZE)
        _dwMasterPassphraseLength = dwPlainSize + (CRYPTPROTECTMEMORY_BLOCK_SIZE - dwMod);
    else
        _dwMasterPassphraseLength = dwPlainSize;

    _MasterPassword = (LPWSTR)GlobalAlloc(LPTR, _dwMasterPassphraseLength);

    wmemcpy_s(_MasterPassword, _dwMasterPassphraseLength, p.toStdWString().c_str(), wcslen(p.toStdWString().c_str()) + 1);
    ProtectMasterPassphrase();
}

wchar_t* Wilhelmina::GetMasterpassphrase() {
    UnProtectMasterPassphrase();
    return _MasterPassword;
}

void Wilhelmina::showEvent(QShowEvent* ev)
{
    QMainWindow::showEvent(ev);
    QTimer::singleShot(0, this, SLOT(PostActivate()));
}

void Wilhelmina::exitWilhelmina() {
    this->close();
}

void Wilhelmina::encryptOnWindowStateEvent(wchar_t *p) {

    if (!m_cryptoState.getState()) { 

        if (p == nullptr) {
            p = GetMasterpassphrase();
        }

        if (m_Entries.Encrypt(this, ui.statusBar, &Settings, p, m_DataPath, true)) {
            ui.listWidget->clear();
            m_cryptoState.setState(true);
        }
        else {
            QMessageBox::critical(this, "Wilhelmina",
                "Encryption failed.\nDo you have permission to write into the data location:\n" + m_DataPath + " ?",
                QMessageBox::Ok);
        }
        
        ProtectMasterPassphrase();

        ui.statusBar->showMessage("Ready");
    }
}

void Wilhelmina::closeEvent(QCloseEvent* ev) {

    QApplication::setOverrideCursor(Qt::WaitCursor);
    wchar_t* p = GetMasterpassphrase();
    if (p) {
        encryptOnWindowStateEvent(p);
    }
    else {
        QMessageBox::critical(this, "Wilhelmina",
            "Master passphrase is null. Cannot encrypt. Fatal failure.",
            QMessageBox::Ok);
    }
    
    Settings.setValue("geometry", this->saveGeometry());
    Settings.setValue("DatafileLocation", m_DataPath);
    QApplication::restoreOverrideCursor();

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

bool Wilhelmina::eventFilter(QObject* target, QEvent* ev) {

    if (ev->type() == QEvent::KeyPress && target == ui.lineEditSearch) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(ev);

        if (keyEvent->key() == Qt::Key_Escape) {
            if (ui.lineEditSearch->text().length() > 0)
                ui.lineEditSearch->clear();
        }
    }

    return QObject::eventFilter(target, ev);
}

void Wilhelmina::syncFromRemote(const QString& fullDataPath) {
    if (Settings.value("SSHenabled").toBool()) {
        SSHsync sync(&Settings, this);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        ui.statusBar->showMessage("Please wait...Syncing...");
        if (!sync.fromRemote(fullDataPath)) {
            QApplication::restoreOverrideCursor(); //Stupid, but we want the wait cursor to go away even if there's an error.
            QMessageBox::critical(this, "Wilhelmina", sync.lastErrorMessage(), QMessageBox::Ok);
        }
        ui.statusBar->showMessage("Sync Ready");
        QApplication::restoreOverrideCursor();
    }
}

void Wilhelmina::PostActivate()
{
    if (!QDir(m_DataPath).exists()) {
        if (!QDir().mkpath(m_DataPath)) {
            QMessageBox::critical(this, "Wilhelmina", "Unable to create path " + m_DataPath + ".\n Abort.",
                QMessageBox::Ok);
            QApplication::quit();
        }
    }

    QString fullDataPath = m_DataPath + m_Entries.encryptedBlobFile();
    syncFromRemote(fullDataPath);

    //We have our data path, do we have any encrypted data?
    if (QFile::exists(fullDataPath)) {
        MasterPasswordDialog dlg(m_DataPath, &Settings, false, true, this);
        dlg.SetCanReject(false);
        if (dlg.exec() == QDialog::Accepted) {
            SetMasterPassphrase(dlg.GetPassphrase());

            if (m_DataPath != dlg.GetProfilePath()) {
                m_DataPath = dlg.GetProfilePath();
                Settings.setValue("DatafileLocation", m_DataPath);
                this->setWindowTitle("Wilhelmina - " + m_DataPath);
                fullDataPath = m_DataPath + m_Entries.encryptedBlobFile();
                syncFromRemote(fullDataPath);
            }

            if (m_Entries.Decrypt(GetMasterpassphrase(), m_DataPath)) {
                m_cryptoState.setState(false);
                populateViewFromEntries();
                ProtectMasterPassphrase();
            }
            else {
                ProtectMasterPassphrase();
                PostActivate();
            }
        }
    }
    else {
        //We don't have any data even if the directory path exists so we need to ask for the user to set up the master passphrase.
        m_cryptoState.setState(false);
        MasterPasswordDialog dlg(m_DataPath, &Settings, true, false, this);
        if (dlg.exec() == QDialog::Accepted)
            SetMasterPassphrase(dlg.GetPassphrase());
        else
            QApplication::quit();
    }
    
    ui.statusBar->showMessage("Ready");
}

void Wilhelmina::listItemClicked(QListWidgetItem* item) {
    if (item->isSelected()) {
        ui.actionDelete->setEnabled(true);
        ui.actionCopyPassword->setEnabled(true);
        ui.actionCopyUsername->setEnabled(true);
        ui.actionOpen_in_Browser->setEnabled(true);
        ui.actionEdit->setEnabled(true);
        ui.actionClone->setEnabled(true);
    }
}

void Wilhelmina::listItemDoubleClicked(QListWidgetItem *item) {
    editEntry();
}

void Wilhelmina::editEntry() {

    CustomListWidgetItem* ci = static_cast<CustomListWidgetItem*>(ui.listWidget->selectedItems()[0]);
    QString id = ci->getID();
    QJsonObject obj = m_Entries.GetJObject(id);

    AddNewEntry dlg("Edit Entry", true, &obj, &Settings, this);

    if (dlg.exec() == QDialog::Accepted) {
        m_Entries.deleteEntry(ci->getID());
        ui.listWidget->removeItemWidget(ci);
        delete ci;
        AddNewEntryToMemory(dlg.GetTitle(), dlg.GetUsername(), dlg.GetPassword(), dlg.GetUrl(), dlg.Pinned(), dlg.GetNotes());
    }
}

void Wilhelmina::cloneEntry() {
    CustomListWidgetItem* ci = static_cast<CustomListWidgetItem*>(ui.listWidget->selectedItems()[0]);
    QString id = ci->getID();
    QJsonObject obj = m_Entries.GetJObject(id);
    AddNewEntryToMemory(obj.value("title").toString(), obj.value("user").toString(), 
                        obj.value("password").toString(), obj.value("url").toString(),
                        obj.value("pinned").toBool(),
                        obj.value("notes").toString());
}

void Wilhelmina::encryptAndLock() {
        this->showMinimized();
}

void Wilhelmina::populateViewFromEntries() {

    QApplication::setOverrideCursor(Qt::WaitCursor);

    ui.listWidget->clear();

    for (auto oneEntry : m_Entries.entryArray()) {
        QJsonObject entry = oneEntry.toObject();
        AddEntryToView(entry.value("title").toString(), entry.value("ID").toString(), entry.value("pinned").toBool());
    }

    QApplication::restoreOverrideCursor();
}

void Wilhelmina::AddEntryToView(QString title, QString ID, bool pinned) {
    CustomListWidgetItem* item = new CustomListWidgetItem();
    item->setPinned(pinned);
    item->setText(title);
    item->setID(ID);

    if (item->pinned())
        ui.listWidget->insertItem(0, item);
    else
        ui.listWidget->addItem(item);

    if (!ui.actionEncrypt->isEnabled())
        ui.actionEncrypt->setEnabled(true);

    m_statusLabel->setText(QString::number(ui.listWidget->count()) + " entries ");
}

void Wilhelmina::AddNewEntryToMemory(QString title, QString user, QString password, QString url, bool pinned, QString notes) {

    QString ID = QUuid::createUuid().toString();
    m_Entries.AddEntry(title, user, password, url, notes, pinned, ID);
    
    AddEntryToView(title, ID, pinned);
    encryptCurrentData();
}

void Wilhelmina::encryptCurrentData() {

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (!m_cryptoState.getState()) {
        if (!m_Entries.Encrypt(this, ui.statusBar, &Settings, GetMasterpassphrase(), m_DataPath, false)) {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, "Wilhelmina",
                "Encryption failed.\nDo you have permission to write into the data location:\n" + m_DataPath + " ?",
                QMessageBox::Ok);
        }

        ProtectMasterPassphrase();
    }
    else {
        m_cryptoState.setState(true);
    }

    QApplication::restoreOverrideCursor();
}

void Wilhelmina::addNewEntry() {
    AddNewEntry dlg("Add New Entry", false, nullptr, &Settings, this);

    if (dlg.exec() == QDialog::Accepted) {
        AddNewEntryToMemory(dlg.GetTitle(), dlg.GetUsername(), dlg.GetPassword(), dlg.GetUrl(), dlg.Pinned(), dlg.GetNotes());
    }
}

void Wilhelmina::deleteSelectedItem() {

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Question);
   
    CustomListWidgetItem* item = static_cast<CustomListWidgetItem*>(ui.listWidget->selectedItems()[0]);

    msgBox.setText("This will delete \"" + item->text() + "\".");
    msgBox.setInformativeText("Do you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {
        m_Entries.deleteEntry(item->getID());
        ui.listWidget->takeItem(ui.listWidget->currentRow());
        delete item;

        encryptCurrentData();
        m_statusLabel->setText(QString::number(ui.listWidget->count()) + " entries ");
    }
}

void Wilhelmina::listItemSelectionChanged() {
    if (ui.listWidget->selectedItems().count() == 0) {
        ui.actionDelete->setEnabled(false);
        ui.actionCopyPassword->setEnabled(false);
        ui.actionCopyUsername->setEnabled(false);
        ui.actionOpen_in_Browser->setEnabled(false);
        ui.actionEdit->setEnabled(false);
        ui.actionClone->setEnabled(false);
    }
}

void Wilhelmina::copyUsername() {
    QClipboard* cBoard = QApplication::clipboard();
    CustomListWidgetItem* item = static_cast<CustomListWidgetItem*>(ui.listWidget->selectedItems()[0]);
   
    cBoard->setText(m_Entries.GetJObject(item->getID()).value("user").toString());
    ui.statusBar->showMessage("Username copied", 2000);
}

void Wilhelmina::copyPassword() {
    QClipboard* cBoard = QApplication::clipboard();
    CustomListWidgetItem* item = static_cast<CustomListWidgetItem*>(ui.listWidget->selectedItems()[0]);
    
    cBoard->setText(m_Entries.GetJObject(item->getID()).value("password").toString());
    ui.statusBar->showMessage("Password copied", 2000);
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
        QMessageBox::warning(this, "Wilhelmina", "The entry has no URL set. Edit the entry to add one.", QMessageBox::Ok);
    }
}

void Wilhelmina::applyNewProfile(QString profilePath) {
    
    if (!m_cryptoState.getState()) {
        wchar_t* p = GetMasterpassphrase();
        if (!m_Entries.Encrypt(this, ui.statusBar, &Settings, p, m_DataPath, true)) {
            QMessageBox::critical(this, "Wilhelmina",
                "Encryption failed.\nDo you have permission to write into the data location:\n" + m_DataPath + " ?",
                QMessageBox::Ok);
            ProtectMasterPassphrase();
            return;
        }
        else {
            ProtectMasterPassphrase();
            m_cryptoState.setState(true);
            ui.listWidget->clear();
        }
    }

    if (!QDir(profilePath).exists()) {
        if (!QDir().mkpath(profilePath))
            QMessageBox::critical(this, "Wilhelmina", "Unable to create path " + profilePath + ".\n Abort.",
                QMessageBox::Ok);

        syncFromRemote(profilePath + m_Entries.encryptedBlobFile());
    }

    m_DataPath = profilePath;
    Settings.setValue("DatafileLocation", m_DataPath);
    this->setWindowTitle("Wilhelmina - " + m_DataPath);

    if (QFile::exists(m_DataPath + m_Entries.encryptedBlobFile())) {
        if (m_Entries.Decrypt(GetMasterpassphrase(), m_DataPath)) {
            m_cryptoState.setState(false);
            populateViewFromEntries();
            ProtectMasterPassphrase();
        }
        else {
            //It's possible to have wait cursor set when we land here, restore default cursor
            //to make user experience nicer.
            ProtectMasterPassphrase();
            QApplication::restoreOverrideCursor();
            PostActivate();
        }
    }
    else {
        m_cryptoState.setState(false);
    }
}

void Wilhelmina::showPreferences() {

    PreferencesDialog dlg(&Settings, this);
    if (dlg.exec() == QDialog::Accepted) {
        
        //Check if we changed the datapath and encrypt all existing data to the old path.
        if (m_DataPath != dlg.dataFileLocation()) {
            applyNewProfile(dlg.dataFileLocation());
        }

        //Update interval timer, check the interval value changed and restart timer with the new interval
        if (m_IdleFilter->Interval() != dlg.intervalInMilliseconds()) {
            m_IdleFilter->setInterval(dlg.intervalInMilliseconds());
        }

        if (Settings.value("SSHenabled").toBool()) {
            //We do not want to sync from remote with newly created profile.
            if (!dlg.profilesAdded()) {

                if (QDir().mkpath(m_DataPath)) {
                    syncFromRemote(m_DataPath + m_Entries.encryptedBlobFile());
                    if (m_Entries.Decrypt(GetMasterpassphrase(), m_DataPath)) {
                        populateViewFromEntries();
                    }
                    else {
                        QMessageBox msgBox(this);
                        msgBox.setIcon(QMessageBox::Information);
                        msgBox.setText("Found remote profile, but unable to decrypt it.");
                        msgBox.setInformativeText("Try changing the master password for this profile to match the remote one and try enabling SSH again.");
                        msgBox.setStandardButtons(QMessageBox::Ok);
                        msgBox.setDefaultButton(QMessageBox::Ok);
                        msgBox.exec();
                        Settings.setValue("SSHenabled", false);
                    }

                    ProtectMasterPassphrase();
                }
                else {
                    QMessageBox::critical(this, "Wilhelmina", "Unable to create path " + m_DataPath + ".",
                        QMessageBox::Ok);
                }
            }
        }

        if (dlg.profilesAdded() || dlg.localProfileAdded_exist_remote())
            populateProfileMenu();
    }
}

void Wilhelmina::changeMasterPassphrase() {
    MasterPasswordDialog dlg(m_DataPath, &Settings, true, false, this);
    dlg.DisableExitButton();
    if (dlg.exec() == QDialog::Accepted) {
        SetMasterPassphrase(dlg.GetPassphrase());
    }
}

void Wilhelmina::showContextMenu(const QPoint& point) {
    
    QMenu menu(this);
    
    menu.addAction(ui.actionEdit);
    menu.addAction(ui.actionClone);
    menu.addSeparator();
    menu.addAction(ui.actionCopyPassword);
    menu.addAction(ui.actionCopyUsername);
    menu.addSeparator();
    CustomListWidgetItem* item = static_cast<CustomListWidgetItem*>(ui.listWidget->selectedItems()[0]);
    QJsonObject obj = m_Entries.GetJObject(item->getID());

    if (!obj.value("url").toString().isEmpty()) {
        menu.addAction(ui.actionOpen_in_Browser);
    }

    menu.addSeparator();
    menu.addAction(ui.actionDelete);

    const QPoint global = ui.listWidget->mapToGlobal(point);
    
    menu.exec(global);
}

void Wilhelmina::searchChanged() {

    for (int i = 0; i < ui.listWidget->count(); i++) {
        QListWidgetItem* item = ui.listWidget->item(i);
        item->setHidden(false);
    }

    QString search = ui.lineEditSearch->text().toLower();

    if (search.length() > 0) {
        //loop all listwidget items and hide the ones which title does not match the search
        for (int i = 0; i < ui.listWidget->count(); i++) {
            QListWidgetItem* item = ui.listWidget->item(i);
            if (!item->text().toLower().contains(search))
                item->setHidden(true);
        }
    }
}

void Wilhelmina::exportEntries() {

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("All entries are exported as plain text.");
    msgBox.setInformativeText("Do you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {
        QString filename = QFileDialog::getSaveFileName(this,
            tr("Export Entries"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), "Json Files (*.json)");

        if (!filename.isEmpty()) {
            QFile file(filename);

            if (file.open(QIODevice::ReadWrite)) {
                QJsonDocument doc;
                doc.setArray(m_Entries.entryArray());
                QByteArray json = doc.toJson();
                file.write(json);
                file.close();
                ui.statusBar->showMessage("Entries exported.", 2000);
            }
        }
    }
}

void Wilhelmina::importEntries() {

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("This will overwrite existing entries.");
    msgBox.setInformativeText("Do you want to continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {

        QString filename = QFileDialog::getOpenFileName(this,
            tr("Import Entries"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), "Json Files (*.json)");

        if (!filename.isEmpty()) {
            QFile file(filename);

            if (file.open(QIODevice::ReadOnly)) {
                QByteArray json = file.readAll();
                file.close();

                ui.listWidget->clear();
                QJsonDocument doc = QJsonDocument::fromJson(json);
                m_Entries.setDocumentFromImportData(doc);
                populateViewFromEntries();
            }
        }
    }
}

CryptoState* Wilhelmina::cryptoState() {
    return &m_cryptoState;
}

void Wilhelmina::findDuplicates() {
    
    DuplicateDialog dlg(this, &m_Entries, &Settings);
    int ret = dlg.exec();
    //We don't really care what button user uses to exit the dialog, check if anything is edited instead.
    if (ret == QDialog::Accepted || ret == QDialog::Rejected) {
        if (dlg.Edited()) {
            ui.listWidget->clear();
            encryptCurrentData();
            populateViewFromEntries();
        }
    }
}

void Wilhelmina::showAbout() {
    AboutDialog dlg(this);
    dlg.exec();
}

void Wilhelmina::openHomepage() {
    QDesktopServices::openUrl(QUrl("https://www.byteptr.com/wilhelmina/"));
}
