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

#pragma once

#include <QtWidgets/QMainWindow>
#include "Entries.h"
#include "CustomListWidgetItem.h"
#include "ui_Wilhelmina.h"
#include <qsettings.h>
#include "IdleFilter.h"
#include <qlabel.h>

class Wilhelmina : public QMainWindow
{
    Q_OBJECT

public:
    Wilhelmina(QWidget *parent = nullptr);
    ~Wilhelmina();
    QSettings Settings;
    void setIdleFilter(IdleFilter* filter);

protected:
    void showEvent(QShowEvent* ev);
    void changeEvent(QEvent* ev);
    void closeEvent(QCloseEvent* ev);
    bool eventFilter(QObject* target, QEvent* ev);

private:
    void encryptOnWindowStateEvent();
    QString m_MasterPassword;
    Ui::WilhelminaClass ui;
    QString m_DataPath;
    Entries m_Entries;
    void AddNewEntryToMemory(QString title, QString user, QString password, QString url, QString notes);
    void ProtectMasterPassphrase();
    void UnProtectMasterPassphrase();
    void populateViewFromEntries();
    void AddEntryToView(QString title, QString ID);
    bool m_IsEncrypted;
    IdleFilter* m_IdleFilter;
    QLabel* m_statusLabel;
    void encryptCurrentData();

public slots:
    void addNewEntry();
    void listItemDoubleClicked(QListWidgetItem*);
    void listItemClicked(QListWidgetItem*);
    void PostActivate();
    void encryptAndLock();
    void deleteSelectedItem();
    void listItemSelectionChanged();
    void copyUsername();
    void copyPassword();
    void openInBrowser();
    void showPreferences();
    void changeMasterPassphrase();
    void editEntry();
    void cloneEntry();
    void showContextMenu(const QPoint&);
    void exitWilhelmina();
    void searchChanged();
    void exportEntries();
    void importEntries();
    void findDuplicates();
};
