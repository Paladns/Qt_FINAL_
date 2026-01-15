#include "databasemanager.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>
#include <QFileInfo>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
{
    // 初始化数据库连接
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    
    // 设置数据库文件路径为当前目录
    QString dbPath;
    // 优先使用应用程序所在目录，如果无法获取则使用当前工作目录
    if (QCoreApplication::instance()) {
        dbPath = QCoreApplication::applicationDirPath() + "/campus_activity.db";
    } else {
        dbPath = QDir::currentPath() + "/campus_activity.db";
    }
    
    // 确保目录存在
    QFileInfo dbFileInfo(dbPath);
    QDir dbDir = dbFileInfo.absoluteDir();
    if (!dbDir.exists()) {
        dbDir.mkpath(".");
    }
    
    m_database.setDatabaseName(dbPath);
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

