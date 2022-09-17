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

#include "MasterPasswordDialog.h"
#include <qdialogbuttonbox.h>
#include <qprocess.h>
#include <qdir.h>

MasterPasswordDialog::MasterPasswordDialog(QString &dataPath, QSettings* settings, bool setNewPassphrase, bool disableCancel, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	
	//Hide the dialog icon
	setWindowFlag(Qt::CustomizeWindowHint, true);
	setWindowFlag(Qt::WindowTitleHint, true);
	setWindowFlag(Qt::WindowSystemMenuHint, false);

	this->setFixedSize(this->size());

	m_setNewPassphrase = setNewPassphrase;

	connect(ui.lineEditMasterPassphrase, &QLineEdit::textChanged, this, &MasterPasswordDialog::PassphraseFieldChanged);

	if (m_setNewPassphrase) {
		connect(ui.lineEditMasterPassphraseAgain, &QLineEdit::textChanged, this, &MasterPasswordDialog::PassphraseFieldChanged);
		ui.comboBoxProfiles->setVisible(false);
	}

	if (!m_setNewPassphrase) {
		ui.lineEditMasterPassphraseAgain->setVisible(false);
		ui.label_MasterPassphraseAgain->setVisible(false);
		ui.comboBoxProfiles->insertItem(0, dataPath);

		QList<QString> profiles = settings->value("Profiles").value<QList<QString>>();
		//Zero is our last known path, skip that as it's already inserted
		int count = 1;
		for (auto& item : profiles) {
			if (QDir(item).exists()) {
				if (ui.comboBoxProfiles->findText(item, Qt::MatchExactly) == -1) {
					ui.comboBoxProfiles->insertItem(count, item);
					count++;
				}
			}
		}
	}

	if (disableCancel)
		ui.cancelButton->setEnabled(false);

	this->SetCanReject(true);

	ui.pushButtonExit->setEnabled(true);

	PassphraseFieldChanged();
}

MasterPasswordDialog::~MasterPasswordDialog()
{}

QString MasterPasswordDialog::GetProfilePath() {
	return ui.comboBoxProfiles->currentText();
}

QString MasterPasswordDialog::GetPassphrase() {
	return ui.lineEditMasterPassphrase->text();
}

void MasterPasswordDialog::PassphraseFieldChanged() {

	if (m_setNewPassphrase) {
		if (ui.lineEditMasterPassphrase->text().length() == 0 || ui.lineEditMasterPassphrase->text() != ui.lineEditMasterPassphraseAgain->text())
			ui.okButton->setEnabled(false);
		else
			ui.okButton->setEnabled(true);
	}
	else {
		if (ui.lineEditMasterPassphrase->text().length() == 0)
			ui.okButton->setEnabled(false);
		else
			ui.okButton->setEnabled(true);
	}
}

void MasterPasswordDialog::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape && !m_canReject)
		return;
	
	QDialog::keyPressEvent(e);
}

void MasterPasswordDialog::SetCanReject(bool status) {
	m_canReject = status;
}

void MasterPasswordDialog::accept() {
	QDialog::accept();
}

void MasterPasswordDialog::reject() {
	if (m_canReject)
		QDialog::reject();
}

void MasterPasswordDialog::exitWilhelmina() {
	m_canReject = true;
	QApplication::quit();
}

void MasterPasswordDialog::showVirtualKeyboard() {
	QProcess osk;
	osk.setProgram("\"C:\\Windows\\System32\\osk.exe\"");
	osk.startDetached();
}

