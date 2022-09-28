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

#include "DuplicateDialog.h"
#include "CustomQTableWidgetItem.h"
#include "AddNewEntry.h"

DuplicateDialog::DuplicateDialog(QWidget* parent, Entries *entries, QSettings *settings) : QDialog(parent)
{
	ui.setupUi(this);

	m_entries = entries;
	m_settings = settings;
	m_edited = false;

	//Hide the dialog icon
	setWindowFlag(Qt::CustomizeWindowHint, true);
	setWindowFlag(Qt::WindowTitleHint, true);
	setWindowFlag(Qt::WindowSystemMenuHint, false);

	this->setFixedSize(this->size());
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	connect(ui.tableWidget, &QTableWidget::itemDoubleClicked, this, &DuplicateDialog::cellDoubleClicked);

	findDuplicates();
}

DuplicateDialog::~DuplicateDialog() {
	freeTableWidgetMemory();
}

void DuplicateDialog::freeTableWidgetMemory() {
	for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
		delete ui.tableWidget->item(i, 0);
		delete ui.tableWidget->item(i, 1);
	}
}

bool DuplicateDialog::Edited() {
	return m_edited;
}

void DuplicateDialog::cellDoubleClicked(QTableWidgetItem* item) {

	CustomQTableWidgetItem* ci = static_cast<CustomQTableWidgetItem*>(item);
	QString id = ci->ID();
	QJsonObject obj = m_entries->GetJObject(id);

	AddNewEntry dlg("Edit Entry", true, &obj, m_settings, this);

	if (dlg.exec() == QDialog::Accepted) {
		m_entries->deleteEntry(ci->ID());
		QString p = dlg.GetPassword();
		m_entries->AddEntry(dlg.GetTitle(), dlg.GetUsername(), p, dlg.GetUrl(),  dlg.GetNotes(), dlg.Pinned(), id);
		m_edited = true;

		freeTableWidgetMemory();
		findDuplicates();
	}
}

void DuplicateDialog::findDuplicates() {

	ui.tableWidget->clear();

	QApplication::setOverrideCursor(Qt::WaitCursor);
	QList<DuplicateEntry*> dups;

	QJsonObject entry;
	QJsonObject entryNext;

	for (int i = 0; i < m_entries->entryArray().count(); i++) {
		for (int j = i + 1; j < m_entries->entryArray().count(); j++) {
			entry = m_entries->entryArray()[i].toObject();
			entryNext = m_entries->entryArray()[j].toObject();

			if ((i != j) && (entry.value("password").toString() == entryNext.value("password").toString())) {
				if (entry.value("title").toString() != entryNext.value("title").toString()) {
					dups.append(new DuplicateEntry(entry.value("title").toString(), entry.value("ID").toString(),
						entryNext.value("title").toString(), entryNext.value("ID").toString()));
				}
			}
		}
	}

	ui.tableWidget->setColumnCount(2);
	ui.tableWidget->setRowCount(dups.count());
	
	if(dups.count() > 0)
		ui.labelInfo->setText("Same passwords reused " + QString::number(dups.count()) + " times");
	else
		ui.labelInfo->setText("No duplicates found.");
	
	int index = 0;

	for (auto entry : dups) {
		ui.tableWidget->setItem(index, 0, new CustomQTableWidgetItem(entry->title(), entry->ID()));
		ui.tableWidget->setItem(index, 1, new CustomQTableWidgetItem(entry->secondTitle(), entry->secondID()));
		index++;
	}

	QApplication::restoreOverrideCursor();
}