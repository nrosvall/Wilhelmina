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
#include "ui_DuplicatesDialog.h"
#include "DuplicateEntry.h"
#include "Entries.h"
#include <qsettings.h>

class DuplicateDialog : public QDialog
{
	Q_OBJECT

public:
	DuplicateDialog(QWidget* parent,  Entries* entries, QSettings* settings);
	~DuplicateDialog();
	bool Edited();
public slots:
	void cellDoubleClicked(QTableWidgetItem* item);
private:
	Ui::DialogDuplicates ui;
	void findDuplicates();
	Entries* m_entries;
	QSettings* m_settings;
	bool m_edited;
	void freeTableWidgetMemory();

};