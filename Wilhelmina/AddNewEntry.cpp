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

#include "AddNewEntry.h"
#include <qdialogbuttonbox.h>
#include "PasswordGenerator.h"

AddNewEntry::AddNewEntry(QString title, bool isEdit,
	QJsonObject* obj, QSettings* settings,
	QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_Settings = settings;

	//Hide the dialog icon
	setWindowFlag(Qt::CustomizeWindowHint, true);
	setWindowFlag(Qt::WindowTitleHint, true);
	setWindowFlag(Qt::WindowSystemMenuHint, false);

	this->setFixedSize(this->size());
	this->setWindowTitle(title);

	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	if (isEdit) {

		ui.buttonBox->button(QDialogButtonBox::Save)->setText("Edit");

		ui.lineEdit_Title->setText(obj->value("title").toString());
		ui.lineEdit_Username->setText(obj->value("user").toString());
		ui.lineEdit_Password->setText(obj->value("password").toString());
		ui.lineEdit_Url->setText(obj->value("url").toString());
		ui.plainTextEditNotes->setPlainText(obj->value("notes").toString());
		ui.checkBoxPinned->setChecked(obj->value("pinned").toBool());
	}

	CheckFieldStatuses();
}

AddNewEntry::~AddNewEntry()
{}

void AddNewEntry::GenerateNewPassword() {
	QApplication::setOverrideCursor(Qt::WaitCursor);
	PasswordGenerator generator(this);
	ui.lineEdit_Password->setText(generator.generatePassword(m_Settings->value("PasswordLength", 26).toInt()));
	QApplication::restoreOverrideCursor();
}

QString AddNewEntry::GetTitle() {
	return this->ui.lineEdit_Title->text();
}

QString AddNewEntry::GetUsername() {
	return this->ui.lineEdit_Username->text();
}

QString AddNewEntry::GetPassword() {
	return this->ui.lineEdit_Password->text();
}

QString AddNewEntry::GetUrl() {
	return this->ui.lineEdit_Url->text();
}

QString AddNewEntry::GetNotes() {
	return this->ui.plainTextEditNotes->toPlainText();
}

bool AddNewEntry::Pinned() {
	return this->ui.checkBoxPinned->isChecked();
}

void AddNewEntry::HandleCustomPlaceholderTextColor() {

	if (m_Settings->value("DarkThemeEnabled").toBool()) {

		QString qssOrig = "QLineEdit[text = \"\"]{ color:#E0E1E3; }";
		QString qssNew = "QLineEdit[text = \"\"]{ color:gray; }";
		
		if (ui.lineEdit_Title->text().length() == 0)
			ui.lineEdit_Title->setStyleSheet(qssNew);
		else
			ui.lineEdit_Title->setStyleSheet(qssOrig);

		if (ui.lineEdit_Username->text().length() == 0)
			ui.lineEdit_Username->setStyleSheet(qssNew);
		else
			ui.lineEdit_Username->setStyleSheet(qssOrig);

		if (ui.lineEdit_Password->text().length() == 0)
			ui.lineEdit_Password->setStyleSheet(qssNew);
		else
			ui.lineEdit_Password->setStyleSheet(qssOrig);

		if (ui.lineEdit_Url->text().length() == 0)
			ui.lineEdit_Url->setStyleSheet(qssNew);
		else
			ui.lineEdit_Url->setStyleSheet(qssOrig);

	}
}

void AddNewEntry::CheckFieldStatuses() {
	if (ui.lineEdit_Title->text().length() == 0 || ui.lineEdit_Username->text().length() == 0 || ui.lineEdit_Password->text().length() == 0)
		ui.buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
	else
		ui.buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);

	HandleCustomPlaceholderTextColor();
}

void AddNewEntry::UserFieldChanged() {
	CheckFieldStatuses();
}

void AddNewEntry::TitleFieldChanged() {
	CheckFieldStatuses();
}

void AddNewEntry::PasswordFieldChanged() {
	CheckFieldStatuses();
}

void AddNewEntry::UrlFieldChanged() {
	CheckFieldStatuses();
}

void AddNewEntry::ShowPassword() {
	if (ui.checkBoxShowPassword->isChecked()) {
		ui.lineEdit_Password->setEchoMode(QLineEdit::Normal);
	}
	else {
		ui.lineEdit_Password->setEchoMode(QLineEdit::Password);
	}
}