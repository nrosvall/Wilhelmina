#pragma once

#include <QDialog>
#include <QKeyEvent>
#include "ui_MasterPasswordDialog.h"

class MasterPasswordDialog : public QDialog
{
	Q_OBJECT

public:
	MasterPasswordDialog(bool setNewPassphrase, bool disableCancel = false, QWidget* parent = nullptr);
	~MasterPasswordDialog();
	void SetCanReject(bool status);
	QString GetPassphrase();
	
private:
	Ui::DialogMasterPassword ui;
	bool m_setNewPassphrase;
	bool m_canReject;
	
public slots:
	void PassphraseFieldChanged();
	void exitWilhelmina();

protected:
	void keyPressEvent(QKeyEvent* e) override;
	void accept() override;
	void reject() override;
};
