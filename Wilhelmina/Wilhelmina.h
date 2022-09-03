#pragma once

#include <QtWidgets/QMainWindow>
#include "Entries.h"
#include "CustomListWidgetItem.h"
#include "ui_Wilhelmina.h"

class Wilhelmina : public QMainWindow
{
    Q_OBJECT

public:
    Wilhelmina(QWidget *parent = nullptr);
    ~Wilhelmina();

protected:
    void showEvent(QShowEvent* ev);
    void changeEvent(QEvent* ev);
    void closeEvent(QCloseEvent* ev);

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
};
