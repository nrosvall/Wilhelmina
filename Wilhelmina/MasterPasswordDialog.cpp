#include "MasterPasswordDialog.h"
#include <qdialogbuttonbox.h>

MasterPasswordDialog::MasterPasswordDialog(bool setNewPassphrase, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	//TODO: add exit button that closes the program

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

	this->SetCanReject(true);

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

