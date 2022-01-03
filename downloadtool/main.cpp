#include "widget.h"
#include "loginform.h"
#include "download.h"
#include "ch341.h"
#include <QApplication>
#include <string>
#include <QDebug>

// 必须安装CH341驱动，要识别为外部接口，不能识别为COM口
// libcrypto-1_1.dll要放在可执行文件目录

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<std::shared_ptr<Ch341>>("std::shared_ptr<Ch341>");
    LoginForm loginForm;
    loginForm.show();
//    Download down;
//    down.show();
    return a.exec();
}
