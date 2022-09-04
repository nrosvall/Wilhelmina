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

#include "PreferencesDialog.h"
#include <qstandardpaths.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

PreferencesDialog::PreferencesDialog(QSettings* settings, QWidget* parent) : QDialog(parent) {
	ui.setupUi(this);

	m_Settings = settings;

	//Hide the dialog icon
	setWindowFlag(Qt::CustomizeWindowHint, true);
	setWindowFlag(Qt::WindowTitleHint, true);
	setWindowFlag(Qt::WindowSystemMenuHint, false);

	ui.lineEditDataLocation->setText(m_Settings->value("DatafileLocation", 
		QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Wilhelmina/").toString());

	ui.spinBox_LockInterval->setValue(m_Settings->value("LockInterval", 5).toInt());
	ui.spinBoxPasswordLength->setValue(m_Settings->value("PasswordLength", 26).toInt());

	this->setFixedSize(this->size());
}

void PreferencesDialog::accept() {

	QString dataPath = ui.lineEditDataLocation->text();
	if (!QDir(dataPath).exists()) {
		if (!QDir().mkpath(dataPath)) {
			QMessageBox::critical(this, "Wilhelmina", "Unable to create path " + dataPath + ".\n Abort.",
				QMessageBox::Ok);
		}
	}

	m_Settings->setValue("DatafileLocation", dataPath);
	m_Settings->setValue("LockInterval", ui.spinBox_LockInterval->value());
	m_Settings->setValue("PasswordLength", ui.spinBoxPasswordLength->value());

	QDialog::accept();
}

void PreferencesDialog::selectDataPath() {

	QString path = QFileDialog::getExistingDirectory(this, "Choose Folder");

	if (path.isEmpty())
		return;

	ui.lineEditDataLocation->setText(path + "/");
}

QString PreferencesDialog::dataFileLocation() {
	return ui.lineEditDataLocation->text();
}

int PreferencesDialog::intervalInMilliseconds() {
	return ui.spinBox_LockInterval->value() * 60000;
}