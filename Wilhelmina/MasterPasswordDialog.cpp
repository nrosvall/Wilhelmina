#include "MasterPasswordDialog.h"
#include <qdialogbuttonbox.h>

MasterPasswordDialog::MasterPasswordDialog(bool setNewPassphrase, QWidget* parent)
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

	if(m_setNewPassphrase)
		connect(ui.lineEditMasterPassphraseAgain, &QLineEdit::textChanged, this, &MasterPasswordDialog::PassphraseFieldChanged);

	if (!m_setNewPassphrase) {
		ui.lineEditMasterPassphraseAgain->setVisible(false);
		ui.label_MasterPassphraseAgain->setVisible(false);
	}

	PassphraseFieldChanged();
}

MasterPasswordDialog::~MasterPasswordDialog()
{}

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

