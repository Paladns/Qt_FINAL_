//【阶段11：2024-05-30】实现CSV导出功能，使用QFile和QTextStream写入文件
//【阶段14：2024-06-02】简化导出功能，直接在主线程执行，避免线程安全问题

#include "csvexporttask.h"
#include "databasemanager.h"
#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
#include <QDebug>
#include <QStringConverter>

CSVExportTask::CSVExportTask(const QString& fileName)
    : m_fileName(fileName)
{
}
