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

#include <qstring.h>

class DuplicateEntry
{
public:
	DuplicateEntry(QString title, QString ID, QString secondTitle, QString secondID);
	QString title() const;
	QString ID() const;
	QString secondTitle() const;
	QString secondID() const;
private:
	QString m_title;
	QString m_id;
	QString m_secondTitle;
	QString m_secondID;
};

