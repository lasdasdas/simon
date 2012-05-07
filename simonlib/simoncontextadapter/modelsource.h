/*
 *   Copyright (C) 2012 Peter Grasch <grasch@simon-listens.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef MODELSOURCE_H
#define MODELSOURCE_H

#include <QString>
#include <QStringList>
#include <QDateTime>

class ModelSource
{
public:
  ModelSource(const QDateTime& date, int baseModelType, const QString& baseModelPath, const QStringList& scenarioPaths, const QString& promptsPath);
  
  int baseModelType() { return m_baseModelType; }
  QString baseModelPath() { return m_baseModelPath; }
  QStringList scenarioPaths() { return m_scenarioPaths; }
  QString promptsPath() { return m_promptsPath; }
  QDateTime date() { return m_date; }
  
private:
  QDateTime m_date;
  int m_baseModelType;
  QString m_baseModelPath;
  QStringList m_scenarioPaths;
  QString m_promptsPath;
};

#endif // MODELSOURCE_H