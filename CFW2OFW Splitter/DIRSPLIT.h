#pragma once
#include <Qtcore/QString>
#include <QtCore/qstringlist.h>
#include <Qtcore/QDirIterator>
#include <QtCore/qfile.h>
#include <QtCore/qdir.h>
#include <Qtcore/qDebug>

class DIRSPLIT
{
public:
	DIRSPLIT(QString path, qint64 size, QStringList templatefiles);
	~DIRSPLIT();
	bool split();
private:
	QString path;
	qint64 size;
	QStringList templatefiles;
};

