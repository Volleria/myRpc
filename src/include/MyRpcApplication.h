#pragma once
#include "MyRpcConfig.h"
#include "RpcProvider.h"
#include "MyRpcChannel.h"
#include "MyRpcController.h"

// MyRpc框架初始化
class MyRpcApplication
{
public:
    // 框架的初始化操作
    static void Init(int argc, char **argv);
    static MyRpcApplication& GetInstance();
    static MyRpcConfig& GetConfig(); 

private:
    static MyRpcConfig config;

    MyRpcApplication(){};
    MyRpcApplication(const MyRpcApplication &) = delete;
    MyRpcApplication(MyRpcApplication &&) = delete;

};