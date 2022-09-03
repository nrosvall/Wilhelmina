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

#include <QDialog>
#include "ui_AddNewEntry.h"
#include <qjsonobject.h>

class AddNewEntry : public QDialog
{
	Q_OBJECT

public:
	AddNewEntry(QString title, bool isEdit, 
				QJsonObject *obj, 
				QWidget *parent = nullptr);
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
	void ShowPassword();
};
