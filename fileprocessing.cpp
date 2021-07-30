#include "fileprocessing.h"
#include <QDebug>
FileProcessing::FileProcessing(QObject *parent) : QObject(parent)
{

}

FileProcessing::~FileProcessing()
{
    if(file_global.isOpen())
        file_global.close();
}

void FileProcessing::getFileName(QString fileName)
{
    file_global.setFileName(fileName);
    if (!file_global.open(QIODevice::Append |QIODevice::WriteOnly))
    {
        qDebug() << "Ошибка при открытии файлав потоке";
        return;
    }
}
