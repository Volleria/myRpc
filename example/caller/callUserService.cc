#include <iostream>
#include "MyRpcApplication.h"


int main(int argc, char **argv)
{
    // 1. 初始化应用程序上下文
    MyRpcApplication &app = MyRpcApplication::GetInstance();
    app.Init(argc, argv);

    // 2. 启动服务
    app.Run();

    return 0;
}