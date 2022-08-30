#pragma once

#include <QDialog>
#include "ui_MasterPasswordDialog.h"

class MasterPasswordDialog : public QDialog
{
	Q_OBJECT

public:
	MasterPasswordDialog(bool setNewPassphrase, QWidget* parent = nullptr);
	~MasterPasswordDialog();
	
	QString GetPassphrase();
	
private:
	Ui::DialogMasterPassword ui;
	bool m_setNewPassphrase;
	
public slots:
	void PassphraseFieldChanged();
};
