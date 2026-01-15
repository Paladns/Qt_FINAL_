//【阶段3：2024-05-22】创建活动模型类，连接数据库查询结果
//【阶段4：2024-05-23】优化数据显示格式

#include "activitymodel.h"
#include "databasemanager.h"

ActivityModel::ActivityModel(QObject *parent)
    : QSqlQueryModel(parent)
{
}

QVariant ActivityModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return "ID";
        case 1: return "活动名称";
        case 2: return "发起人";
        case 3: return "开始时间";
        case 4: return "结束时间";
        case 5: return "人数";
        case 6: return "状态";
        case 7: return "分类";
        default: return QVariant();
        }
    }
    return QSqlQueryModel::headerData(section, orientation, role);
}

// void ActivityModel::refresh(const QString& role, int userId)
// {
//     QSqlQuery query = DatabaseManager::instance().getActivities(role, userId);
//     setQuery(std::move(query)); // 使用move避免已弃用的copy构造函数
// }

// int ActivityModel::getActivityId(int row) const
// {
//     if (row < 0 || row >= rowCount()) {
//         return -1;
//     }
    
//     QModelIndex index = this->index(row, 0);
//     return data(index).toInt();
// }
