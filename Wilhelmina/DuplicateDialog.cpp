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

DuplicateDialog::DuplicateDialog(QWidget* parent, QMultiMap<QString, QString>& dups) : QDialog(parent)
{
	ui.setupUi(this);

	//Hide the dialog icon
	setWindowFlag(Qt::CustomizeWindowHint, true);
	setWindowFlag(Qt::WindowTitleHint, true);
	setWindowFlag(Qt::WindowSystemMenuHint, false);

	this->setFixedSize(this->size());

	ui.labelInfo->setText("Found " + QString::number(dups.count()) + " duplicates");

	QMultiMap<QString, QString>::iterator i;
	QString viewText;

	for (i = dups.begin(); i != dups.end(); i++)
		viewText.append(i.key() + " has the same password as " + i.value() + "\n");
	
	ui.plainTextEditDuplicates->setPlainText(viewText.trimmed());
}