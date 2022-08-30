#pragma once

#include <QDialog>
#include "ui_AddNewEntry.h"

class AddNewEntry : public QDialog
{
	Q_OBJECT

public:
	AddNewEntry(QWidget *parent = nullptr);
	~AddNewEntry();
	QString GetTitle();
	QString GetUsername();
	QString GetPassword();
	QString GetUrl();
	QString GetNotes();

private:
	Ui::AddNewEntryClass ui;
	void CheckFieldStatuses();

public slots:
	void GenerateNewPassword();
	void UserFieldChanged();
	void TitleFieldChanged();
	void PasswordFieldChanged();
};
