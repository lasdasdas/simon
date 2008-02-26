//
// C++ Implementation: bompdict
//
// Description: 
//
//
// Author: Peter Grasch <bedahr@gmx.net>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bompdict.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QFileInfo>

/**
 * \brief Constructor
 * \author Peter Grasch
 * @param path Sets the path of the dictionary (member)
 * @param parent Parent of the object
 */
BOMPDict::BOMPDict(QString path, QObject* parent): Dict(parent)
{
	this->path = path;
}

/**
 * \brief Loads the file from the given path
 * \author Peter Grasch
 * 
 * \todo Document
 * WARNING: This function assumes the system-charset to be ISO-8859-1 and WILL destroy special characters if it isn't
 * 
 * @param path If the path is empty the path set by the constructor is used
 */
void BOMPDict::load(QString path)
{
	if (path.isEmpty()) path = this->path;
	
	emit progress(0);
	QFile *dict = new QFile(path);
	if (!dict->open(QIODevice::ReadOnly))
		return;
	QFileInfo info;
	info.setFile(path);
	int maxProg = info.size();
	int currentProg = 0;
	
	QTextStream *dictStream = new QTextStream(dict);
	dictStream->setCodec("ISO-8859-1");
	emit loaded();
	
	QString line, xsp;
	int wordend, termend;
	line = dictStream->readLine(1000);
	while (!line.isNull())
	{
		wordend = line.indexOf("\t");
		termend = line.indexOf("\t", wordend+1);
		words << line.left(wordend);
		terminals << line.mid(wordend, 
				termend-wordend).trimmed();
		xsp = line.mid(termend).trimmed();
		
 		xsp.remove(QRegExp("^'*?*"));
		xsp.remove("'");
		xsp.remove("|");
		
		pronunciations << xsp;
		
		currentProg += line.length();
		emit progress((int) (((((double)currentProg) / 
				((double)maxProg)))*1000));
		line = dictStream->readLine(1000);
	}
    delete dictStream;
	dict->close();
    dict->deleteLater();
}



BOMPDict::~BOMPDict()
{
}


