#ifndef FILEPROCESSING_H
#define FILEPROCESSING_H

#include <QObject>
#include <QFile>
class FileProcessing: QObject
{
    Q_OBJECT
public:
    FileProcessing(QObject *parent);
    ~FileProcessing();

public slots:
    void getFileName(QString fileName);
private:
    QFile file_global;
};

#endif // FILEPROCESSING_H
