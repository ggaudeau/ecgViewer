#include "tool.h"

#include <QString>
#include <QDir>
#include <QRegularExpression>

bool Tool::listECGFilesInDirectory(const std::string& dir, std::list<std::string>& paths)
{
    QString const FILE_PATTERN = "ecg-*.txt";
    QDir const dataDirectory( QString::fromStdString(dir) );

    if (dataDirectory.exists()) {
        QStringList const nameFilters(FILE_PATTERN);
        QDir::Filters const propFilters = QDir::Files | QDir::NoSymLinks | QDir::Readable | QDir::CaseSensitive;
        QFileInfoList const fileInfoLst = dataDirectory.entryInfoList(nameFilters, propFilters);
        QMap<int, QString> filePaths;

        foreach (const QFileInfo& fileInfo, fileInfoLst) {
            QString baseName = fileInfo.baseName();
            QRegularExpression re("[0-9]+");
            QRegularExpressionMatch res = re.match(baseName);
            if (res.hasMatch()) {
                QString captured = res.captured(0);

                bool ok;
                int index = captured.toInt(&ok);
                if (ok) {
                    if (!filePaths.contains(index)) {
                        filePaths[index] = fileInfo.absoluteFilePath();
                    } else {
                        qDebug() << "ERROR: index [" << index << " is already in map, please check file name [" << baseName << "] in data directory";
                        break;
                    }
                } else {
                    qDebug() << "ERROR: cant convert captured string [" << captured << "] to int in [" << baseName << "]";
                    break;
                }
            }
        }

        QList<int> keys = filePaths.keys();
        bool bConsecutiveNumbers = true;

        for (int i = 0 ; i < keys.size() ; ++i) {
            if (i != keys.at(i)) {
                bConsecutiveNumbers = false;
                break;
            }
        }

        if (bConsecutiveNumbers) {
            QList<QString> values = filePaths.values();

            foreach (const QString& v, values) {
                paths.push_back( v.toStdString() );
            }
            return true;
        } else {
            qDebug() << "ERROR: numbers are not consecutives or incomplete";
        }

    } else {
        qDebug() << "ERROR: directory [" << dataDirectory << "] does not exist";
    }
    return false;
}
