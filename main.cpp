#include <QApplication>
#include "logindialog.h"
#include "adminwidget.h"
#include "organizerwidget.h"
#include "studentwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("CampusActivityManager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("CampusActivity");

    // 创建登录对话框
    LoginDialog *loginDialog = new LoginDialog();

    // 连接登录成功信号，根据角色打开对应主界面
    QObject::connect(loginDialog, &LoginDialog::loginSuccess,
                     [loginDialog](const QString& username, const QString& role, int userId) {
                         QWidget *mainWidget = nullptr;

                         if (role == "admin") {
                             AdminWidget *adminWidget = new AdminWidget(username, userId);
                             // 连接退出登录信号
                             QObject::connect(adminWidget, &AdminWidget::logoutRequested, [loginDialog, adminWidget]() {
                                 adminWidget->deleteLater();
                                 loginDialog->show();
                             });
                             mainWidget = adminWidget;
                         } else if (role == "organizer") {
                             OrganizerWidget *organizerWidget = new OrganizerWidget(username, userId);
                             // 连接退出登录信号
                             QObject::connect(organizerWidget, &OrganizerWidget::logoutRequested, [loginDialog, organizerWidget]() {
                                 organizerWidget->deleteLater();
                                 loginDialog->show();
                             });
                             mainWidget = organizerWidget;
                         } else if (role == "student") {
                             StudentWidget *studentWidget = new StudentWidget(username, userId);
                             // 连接退出登录信号
                             QObject::connect(studentWidget, &StudentWidget::logoutRequested, [loginDialog, studentWidget]() {
                                 studentWidget->deleteLater();
                                 loginDialog->show();
                             });
                             mainWidget = studentWidget;
                         }

                         if (mainWidget) {
                             loginDialog->hide();  // 隐藏登录窗口
                             mainWidget->show();
                         }
                     });

    loginDialog->show();

    return app.exec();
}
