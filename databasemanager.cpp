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

bool DatabaseManager::initialize()
{
    if (m_initialized) {
        return true;
    }

    // 重新确认数据库路径（确保使用正确的目录）
    QString dbPath;
    if (QCoreApplication::instance()) {
        dbPath = QCoreApplication::applicationDirPath() + "/campus_activity.db";
    } else {
        dbPath = QDir::currentPath() + "/campus_activity.db";
    }

    QFileInfo dbFileInfo(dbPath);
    QDir dbDir = dbFileInfo.absoluteDir();
    if (!dbDir.exists()) {
        dbDir.mkpath(".");
    }

    m_database.setDatabaseName(dbPath);
    qDebug() << "Database path:" << dbPath;

    // 打开数据库连接
    if (!m_database.open()) {
        qDebug() << "Failed to open database:" << m_database.lastError().text();
        return false;
    }

    // 创建表结构
    if (!createTables()) {
        qDebug() << "Failed to create tables:" << m_database.lastError().text();
        return false;
    }

    // 初始化测试数据
    initTestData();

    m_initialized = true;
    qDebug() << "Database initialized successfully at:" << dbPath;
    return true;
}

bool DatabaseManager::createTables()
{
    return createUsersTable()
        && createActivitiesTable()
        && createEnrollmentsTable()
        && createWaitlistTable();
}

bool DatabaseManager::createUsersTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            role TEXT NOT NULL CHECK(role IN ('admin', 'organizer', 'student')),
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
        )
    )";

    if (!query.exec(sql)) {
        qDebug() << "Failed to create users table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::createActivitiesTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS activities (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            description TEXT,
            organizer_id INTEGER NOT NULL,
            start_time TEXT NOT NULL,
            end_time TEXT NOT NULL,
            max_participants INTEGER NOT NULL DEFAULT 0,
            current_participants INTEGER NOT NULL DEFAULT 0,
            status TEXT NOT NULL DEFAULT 'pending' CHECK(status IN ('pending', 'approved', 'rejected', 'cancelled', 'completed')),
            category TEXT,
            admin_id INTEGER,
            approved_at TEXT,
            rejected_reason TEXT,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (organizer_id) REFERENCES users(id),
            FOREIGN KEY (admin_id) REFERENCES users(id)
        )
    )";

    if (!query.exec(sql)) {
        qDebug() << "Failed to create activities table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::createEnrollmentsTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS enrollments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            activity_id INTEGER NOT NULL,
            enrolled_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            status TEXT NOT NULL DEFAULT 'enrolled' CHECK(status IN ('enrolled', 'cancelled')),
            FOREIGN KEY (user_id) REFERENCES users(id),
            FOREIGN KEY (activity_id) REFERENCES activities(id),
            UNIQUE(user_id, activity_id, status)
        )
    )";

    if (!query.exec(sql)) {
        qDebug() << "Failed to create enrollments table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::createWaitlistTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS waitlist (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            activity_id INTEGER NOT NULL,
            added_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (user_id) REFERENCES users(id),
            FOREIGN KEY (activity_id) REFERENCES activities(id),
            UNIQUE(user_id, activity_id)
        )
    )";

    if (!query.exec(sql)) {
        qDebug() << "Failed to create waitlist table:" << query.lastError().text();
        return false;
    }

    return true;
}

void DatabaseManager::initTestData()
{
    QSqlQuery query(m_database);

    // 检查是否已有数据
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next() && query.value(0).toInt() > 0) {
        return; // 已有数据，不重复插入
    }

    // 插入测试用户
    query.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");

    // 管理员
    query.addBindValue("admin");
    query.addBindValue("admin123");
    query.addBindValue("admin");
    query.exec();

    // 发起人
    query.addBindValue("organizer1");
    query.addBindValue("org123");
    query.addBindValue("organizer");
    query.exec();

    // 学生
    query.addBindValue("student1");
    query.addBindValue("stu123");
    query.addBindValue("student");
    query.exec();

    query.addBindValue("student2");
    query.addBindValue("stu123");
    query.addBindValue("student");
    query.exec();
}

bool DatabaseManager::authenticateUser(const QString& username, const QString& password, QString& role)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT role FROM users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (!query.exec()) {
        qDebug() << "Authentication query failed:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        role = query.value(0).toString();
        return true;
    }

    return false;
}

bool DatabaseManager::registerUser(const QString& username, const QString& password, const QString& role)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(password);
    query.addBindValue(role);

    if (!query.exec()) {
        qDebug() << "Registration failed:" << query.lastError().text();
        return false;
    }

    return true;
}

int DatabaseManager::createActivity(const QString& title, const QString& description,
                                   const QString& organizer, const QString& startTime,
                                   const QString& endTime, int maxParticipants, const QString& category)
{
    // 获取发起人ID
    QSqlQuery query(m_database);
    query.prepare("SELECT id FROM users WHERE username = ? AND role = 'organizer'");
    query.addBindValue(organizer);

    if (!query.exec() || !query.next()) {
        return -1;
    }

    int organizerId = query.value(0).toInt();

    // 插入活动
    query.prepare(R"(
        INSERT INTO activities (title, description, organizer_id, start_time, end_time, max_participants, category, status)
        VALUES (?, ?, ?, ?, ?, ?, ?, 'pending')
    )");
    query.addBindValue(title);
    query.addBindValue(description);
    query.addBindValue(organizerId);
    query.addBindValue(startTime);
    query.addBindValue(endTime);
    query.addBindValue(maxParticipants);
    query.addBindValue(category);

    if (!query.exec()) {
        qDebug() << "Failed to create activity:" << query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toInt();
}

bool DatabaseManager::updateActivityStatus(int activityId, const QString& status)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE activities SET status = ? WHERE id = ?");
    query.addBindValue(status);
    query.addBindValue(activityId);

    return query.exec();
}

QSqlQuery DatabaseManager::getActivities(const QString& role, int userId)
{
    QSqlQuery query(m_database);
    QString sql = R"(
        SELECT a.id, a.title, a.description, u.username as organizer_name,
               a.start_time, a.end_time, a.max_participants, a.current_participants,
               a.status, a.category, a.created_at
        FROM activities a
        JOIN users u ON a.organizer_id = u.id
        WHERE 1=1
    )";

    if (role == "organizer" && userId > 0) {
        sql += " AND a.organizer_id = " + QString::number(userId);
    } else if (role == "student" && userId > 0) {
        sql += " AND a.status = 'approved'";
    }

    sql += " ORDER BY a.created_at DESC";

    query.exec(sql);
    return query;
}

QSqlQuery DatabaseManager::getActivityById(int activityId)
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT a.id, a.title, a.description, u.username as organizer_name,
               a.start_time, a.end_time, a.max_participants, a.current_participants,
               a.status, a.category, a.created_at
        FROM activities a
        JOIN users u ON a.organizer_id = u.id
        WHERE a.id = ?
    )");
    query.addBindValue(activityId);
    query.exec();
    return query;
}

bool DatabaseManager::checkTimeConflict(int userId, const QString& startTime, const QString& endTime, int excludeActivityId)
{
    QSqlQuery query(m_database);
    QString sql = R"(
        SELECT a.id, a.title, a.start_time, a.end_time
        FROM activities a
        JOIN enrollments e ON a.id = e.activity_id
        WHERE e.user_id = ? AND e.status = 'enrolled'
          AND a.status = 'approved'
          AND ((a.start_time < ? AND a.end_time > ?)
           OR (a.start_time < ? AND a.end_time > ?)
           OR (a.start_time >= ? AND a.end_time <= ?))
    )";

    if (excludeActivityId > 0) {
        sql += " AND a.id != " + QString::number(excludeActivityId);
    }

    query.prepare(sql);
    query.addBindValue(userId);
    query.addBindValue(endTime);
    query.addBindValue(startTime);
    query.addBindValue(endTime);
    query.addBindValue(startTime);
    query.addBindValue(startTime);
    query.addBindValue(endTime);

    if (!query.exec()) {
        return false;
    }

    return query.next(); // 有冲突返回true
}

bool DatabaseManager::enrollActivity(int userId, int activityId, bool& hasConflict, QString& conflictInfo)
{
    // 检查活动状态
    QSqlQuery checkQuery(m_database);
    checkQuery.prepare("SELECT status, max_participants, current_participants, start_time, end_time FROM activities WHERE id = ?");
    checkQuery.addBindValue(activityId);

    if (!checkQuery.exec() || !checkQuery.next()) {
        return false;
    }

    QString status = checkQuery.value(0).toString();
    int maxParticipants = checkQuery.value(1).toInt();
    int currentParticipants = checkQuery.value(2).toInt();
    QString startTime = checkQuery.value(3).toString();
    QString endTime = checkQuery.value(4).toString();

    if (status != "approved") {
        return false;
    }

    // 检查时间冲突
    hasConflict = checkTimeConflict(userId, startTime, endTime, activityId);
    if (hasConflict) {
        conflictInfo = "您已报名了时间冲突的其他活动";
        return false;
    }

    // 检查是否已报名
    checkQuery.prepare("SELECT id FROM enrollments WHERE user_id = ? AND activity_id = ? AND status = 'enrolled'");
    checkQuery.addBindValue(userId);
    checkQuery.addBindValue(activityId);
    if (checkQuery.exec() && checkQuery.next()) {
        hasConflict = false;
        conflictInfo = "您已经报名了该活动";
        return false;
    }

    // 检查是否已满
    if (currentParticipants >= maxParticipants) {
        // 加入候补队列
        addToWaitlist(userId, activityId);
        hasConflict = false;
        conflictInfo = "活动已满，已加入候补队列";
        return false;
    }

    // 插入报名记录
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO enrollments (user_id, activity_id, status) VALUES (?, ?, 'enrolled')");
    query.addBindValue(userId);
    query.addBindValue(activityId);

    if (!query.exec()) {
        return false;
    }

    // 更新活动参与者数量
    query.prepare("UPDATE activities SET current_participants = current_participants + 1 WHERE id = ?");
    query.addBindValue(activityId);
    query.exec();

    hasConflict = false;
    conflictInfo = "报名成功";
    return true;
}

bool DatabaseManager::cancelEnrollment(int userId, int activityId)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE enrollments SET status = 'cancelled' WHERE user_id = ? AND activity_id = ? AND status = 'enrolled'");
    query.addBindValue(userId);
    query.addBindValue(activityId);

    if (!query.exec() || query.numRowsAffected() == 0) {
        return false;
    }

    // 更新活动参与者数量
    query.prepare("UPDATE activities SET current_participants = current_participants - 1 WHERE id = ?");
    query.addBindValue(activityId);
    query.exec();

    // 处理候补队列
    processWaitlist(activityId);

    return true;
}

QSqlQuery DatabaseManager::getEnrollments(int activityId, int userId)
{
    QSqlQuery query(m_database);
    QString sql = R"(
        SELECT e.id, e.user_id, u.username, e.enrolled_at, e.status, a.title as activity_title
        FROM enrollments e
        JOIN users u ON e.user_id = u.id
        JOIN activities a ON e.activity_id = a.id
        WHERE e.status = 'enrolled'
    )";

    if (activityId > 0) {
        sql += " AND e.activity_id = " + QString::number(activityId);
    }
    if (userId > 0) {
        sql += " AND e.user_id = " + QString::number(userId);
    }

    sql += " ORDER BY e.enrolled_at ASC";

    query.exec(sql);
    return query;
}

bool DatabaseManager::addToWaitlist(int userId, int activityId)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT OR IGNORE INTO waitlist (user_id, activity_id) VALUES (?, ?)");
    query.addBindValue(userId);
    query.addBindValue(activityId);

    return query.exec();
}

bool DatabaseManager::processWaitlist(int activityId)
{
    // 检查活动是否还有空位
    QSqlQuery checkQuery(m_database);
    checkQuery.prepare("SELECT max_participants, current_participants FROM activities WHERE id = ?");
    checkQuery.addBindValue(activityId);

    if (!checkQuery.exec() || !checkQuery.next()) {
        return false;
    }

    int maxParticipants = checkQuery.value(0).toInt();
    int currentParticipants = checkQuery.value(1).toInt();

    if (currentParticipants >= maxParticipants) {
        return false; // 仍然满员
    }

    // 获取候补队列中的第一个用户
    QSqlQuery waitlistQuery(m_database);
    waitlistQuery.prepare(R"(
        SELECT user_id FROM waitlist
        WHERE activity_id = ?
        ORDER BY added_at ASC
        LIMIT 1
    )");
    waitlistQuery.addBindValue(activityId);

    if (!waitlistQuery.exec() || !waitlistQuery.next()) {
        return false; // 候补队列为空
    }

    int userId = waitlistQuery.value(0).toInt();

    // 检查时间冲突
    QSqlQuery activityQuery(m_database);
    activityQuery.prepare("SELECT start_time, end_time FROM activities WHERE id = ?");
    activityQuery.addBindValue(activityId);
    activityQuery.exec();
    activityQuery.next();
    QString startTime = activityQuery.value(0).toString();
    QString endTime = activityQuery.value(1).toString();

    if (checkTimeConflict(userId, startTime, endTime, activityId)) {
        // 有时间冲突，跳过这个用户
        QSqlQuery deleteWaitlist(m_database);
        deleteWaitlist.prepare("DELETE FROM waitlist WHERE user_id = ? AND activity_id = ?");
        deleteWaitlist.addBindValue(userId);
        deleteWaitlist.addBindValue(activityId);
        deleteWaitlist.exec();
        return false;
    }

    // 从候补队列移除
    QSqlQuery deleteWaitlist(m_database);
    deleteWaitlist.prepare("DELETE FROM waitlist WHERE user_id = ? AND activity_id = ?");
    deleteWaitlist.addBindValue(userId);
    deleteWaitlist.addBindValue(activityId);
    deleteWaitlist.exec();

    // 添加报名
    QSqlQuery enrollQuery(m_database);
    enrollQuery.prepare("INSERT INTO enrollments (user_id, activity_id, status) VALUES (?, ?, 'enrolled')");
    enrollQuery.addBindValue(userId);
    enrollQuery.addBindValue(activityId);
    enrollQuery.exec();

    // 更新活动参与者数量
    QSqlQuery updateQuery(m_database);
    updateQuery.prepare("UPDATE activities SET current_participants = current_participants + 1 WHERE id = ?");
    updateQuery.addBindValue(activityId);
    updateQuery.exec();

    return true;
}

bool DatabaseManager::approveActivity(int activityId, int adminId)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE activities SET status = 'approved', admin_id = ?, approved_at = ? WHERE id = ?");
    query.addBindValue(adminId);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(activityId);

    return query.exec();
}

bool DatabaseManager::rejectActivity(int activityId, int adminId, const QString& reason)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE activities SET status = 'rejected', admin_id = ?, rejected_reason = ? WHERE id = ?");
    query.addBindValue(adminId);
    query.addBindValue(reason);
    query.addBindValue(activityId);

    return query.exec();
}
