#ifndef CSVEXPORTTASK_H
#define CSVEXPORTTASK_H

#include <QString>
#include <QList>
#include <QStringList>

//【阶段11：2024-05-30】实现CSV导出任务类
//【阶段14：2024-06-02】简化导出功能，直接在主线程执行，避免线程安全问题

/**
 * @brief CSV导出任务类
 * 简化版本：在主线程中执行所有操作，避免SQLite线程安全问题
 */
class CSVExportTask
{
public:
    explicit CSVExportTask(const QString& fileName);
    
    // 在主线程中查询数据
    bool prepareData();
    
    // 写入文件
    void writeToFile();

private:
    QString m_fileName;
    QList<QStringList> m_dataRows;  // 存储查询到的数据
};

#endif // CSVEXPORTTASK_H
