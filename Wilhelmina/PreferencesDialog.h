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
#include "ui_Preferences.h"
#include <QSettings>

class PreferencesDialog : public QDialog
{
	Q_OBJECT

public:
	PreferencesDialog(QSettings *settings, QWidget* parent);
	QString dataFileLocation();
	int intervalInMilliseconds();
	bool profilesAdded();

private:
	Ui::PreferencesDialog ui;
	QSettings* m_Settings;
	void checkFieldStatuses();
	QWidget* m_Parent;
	bool m_profilesAdded;

protected:
	void accept() override;

public slots:
	void sshUserFieldChanged();
	void sshServerFieldChanged();
	void selectDataPath();
	void sshGroupBoxToggled();
};

