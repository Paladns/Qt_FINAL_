#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>


/**
 * @brief 数据库管理单例类
 * 负责SQLite数据库连接、表结构初始化及数据访问
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    // 单例模式获取实例
    static DatabaseManager& instance();
    
    // 初始化数据库连接和表结构
    bool initialize();
    
    // 获取数据库连接
    QSqlDatabase database() const { return m_database; }
    
    // 用户相关操作
    bool authenticateUser(const QString& username, const QString& password, QString& role);
    bool registerUser(const QString& username, const QString& password, const QString& role);
    
    // 活动相关操作
    int createActivity(const QString& title, const QString& description, 
                      const QString& organizer, const QString& startTime, 
                      const QString& endTime, int maxParticipants, const QString& category);
    bool updateActivityStatus(int activityId, const QString& status);
    QSqlQuery getActivities(const QString& role = "", int userId = -1);
    QSqlQuery getActivityById(int activityId);
    
    // 报名相关操作
    bool enrollActivity(int userId, int activityId, bool& hasConflict, QString& conflictInfo);
    bool cancelEnrollment(int userId, int activityId);
    bool checkTimeConflict(int userId, const QString& startTime, const QString& endTime, int excludeActivityId = -1);
    QSqlQuery getEnrollments(int activityId = -1, int userId = -1);
    
    // 候补队列操作
    bool addToWaitlist(int userId, int activityId);
    bool processWaitlist(int activityId);
    
    // 管理员审批操作
    bool approveActivity(int activityId, int adminId);
    bool rejectActivity(int activityId, int adminId, const QString& reason);

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    
    // 禁用拷贝构造和赋值
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    // 创建表结构
    bool createTables();
    bool createUsersTable();
    bool createActivitiesTable();
    bool createEnrollmentsTable();
    bool createWaitlistTable();
    
    // 初始化测试数据
    void initTestData();
    
    QSqlDatabase m_database;
    bool m_initialized;
};

#endif // DATABASEMANAGER_H
