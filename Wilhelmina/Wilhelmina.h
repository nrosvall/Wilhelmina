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

private:
    void showEventHelper();
    QString m_MasterPassword;
    Ui::WilhelminaClass ui;
    QString m_DataPath;
    Entries m_Entries;
    void AddNewEntryToMemory(QString title, QString user, QString password, QString url, QString notes);

public slots:
    void addNewEntry();
    void listItemDoubleClicked(QListWidgetItem*);
};
