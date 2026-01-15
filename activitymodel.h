#ifndef ACTIVITYMODEL_H
#define ACTIVITYMODEL_H

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QVariant>

//【阶段3：2024-05-22】实现活动数据模型类，用于TableView数据展示
//【阶段4：2024-05-23】扩展模型功能，添加数据格式化方法

/**
 * @brief 活动数据模型类
 * 继承自QSqlQueryModel，用于在TableView中展示活动列表
 */
class ActivityModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit ActivityModel(QObject *parent = nullptr);
    
    // 重写headerData方法，自定义列标题
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    // 刷新数据
    void refresh(const QString& role = "", int userId = -1);
    
    // 获取活动ID（根据行号）
    int getActivityId(int row) const;
};

#endif // ACTIVITYMODEL_H
