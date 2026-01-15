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

bool CSVExportTask::prepareData()
{
    // 在主线程中查询数据（SQLite不是线程安全的，必须在主线程访问）
    QSqlDatabase db = DatabaseManager::instance().database();
    if (!db.isOpen()) {
        qDebug() << "Database is not open";
        return false;
    }

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT e.id, e.activity_id, e.user_id, u.username, e.enrolled_at, e.status,
               a.title as activity_title, organizer.username as organizer_name
        FROM enrollments e
        JOIN users u ON e.user_id = u.id
        JOIN activities a ON e.activity_id = a.id
        JOIN users organizer ON a.organizer_id = organizer.id
        WHERE e.status = 'enrolled'
        ORDER BY e.enrolled_at DESC
    )");

    if (!query.exec()) {
        qDebug() << "Query execution failed:" << query.lastError().text();
        return false;
    }

    // 将查询结果存储到内存中
    m_dataRows.clear();
    while (query.next()) {
        QStringList row;
        row << QString::number(query.value(1).toInt());  // activity_id
        row << query.value(6).toString();  // activity_title
        row << query.value(7).toString();  // organizer_name
        row << query.value(3).toString();  // username
        row << query.value(4).toString();  // enrolled_at
        row << query.value(5).toString();  // status
        m_dataRows.append(row);
    }

    if (m_dataRows.isEmpty()) {
        qDebug() << "No data to export";
        return false;
    }

    return true;
}

void CSVExportTask::writeToFile()
{
    // 写入文件（在主线程中执行）
    QFile file(m_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing:" << m_fileName;
        qDebug() << "Error:" << file.errorString();
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // 写入BOM，确保Excel正确识别UTF-8编码
    out << "\xEF\xBB\xBF";

    // 写入表头
    out << "活动ID,活动名称,发起人,报名学生,报名时间,状态\n";

    // 写入数据行
    for (const QStringList& row : m_dataRows) {
        // 转义CSV字段中的引号和逗号
        QString activityTitleEscaped = row[1];
        activityTitleEscaped.replace("\"", "\"\"");
        QString organizerNameEscaped = row[2];
        organizerNameEscaped.replace("\"", "\"\"");
        QString usernameEscaped = row[3];
        usernameEscaped.replace("\"", "\"\"");

        QString line = QString("%1,\"%2\",\"%3\",\"%4\",\"%5\",\"%6\"\n")
                       .arg(row[0])  // activity_id
                       .arg(activityTitleEscaped)
                       .arg(organizerNameEscaped)
                       .arg(usernameEscaped)
                       .arg(row[4])  // enrolled_at
                       .arg(row[5]); // status

        out << line;
    }

    file.close();
    qDebug() << "Exported" << m_dataRows.size() << "rows to" << m_fileName;
}
