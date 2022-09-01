#include "AddNewEntry.h"
#include <qdialogbuttonbox.h>

AddNewEntry::AddNewEntry(QString title, bool isEdit,
						QJsonObject *obj,
						QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	//Hide the dialog icon
	setWindowFlag(Qt::CustomizeWindowHint, true);
	setWindowFlag(Qt::WindowTitleHint, true);
	setWindowFlag(Qt::WindowSystemMenuHint, false);

	this->setFixedSize(this->size());
	this->setWindowTitle(title);

	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	if (isEdit) {
		ui.lineEdit_Title->setText(obj->value("title").toString());
		ui.lineEdit_Username->setText(obj->value("user").toString());
		ui.lineEdit_Password->setText(obj->value("password").toString());
		ui.lineEdit_Url->setText(obj->value("url").toString());
		ui.plainTextEditNotes->setPlainText(obj->value("notes").toString());
	}

	CheckFieldStatuses();
}

AddNewEntry::~AddNewEntry()
{}

void AddNewEntry::GenerateNewPassword() {
	//TODO: Actually implement this
	ui.lineEdit_Password->setText("fooobar");
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

void AddNewEntry::CheckFieldStatuses() {
	if (ui.lineEdit_Title->text().length() == 0 || ui.lineEdit_Username->text().length() == 0 || ui.lineEdit_Password->text().length() == 0)
		ui.buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
	else
		ui.buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
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

void AddNewEntry::ShowPassword() {
	if (ui.checkBoxShowPassword->isChecked())
		ui.lineEdit_Password->setEchoMode(QLineEdit::Normal);
	else
		ui.lineEdit_Password->setEchoMode(QLineEdit::Password);
}