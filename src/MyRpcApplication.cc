#include "MyRpcApplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

MyRpcConfig MyRpcApplication::config;

void ShowArgsHelp()
{
    std::cout<<"format: command -i config_file_path"<<std::endl;
}

void MyRpcApplication::Init(int argc, char **argv)
{
    if(argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        
        default:
            break;
        }
    }

    // 开始加载配置文件 rpcserver_ip =    rpcserver_port =    zookeeper_ip =   zookeeper_port = 
    config.LoadConfigFile(config_file.c_str()); // string -> char*

    std::cout<<"rpcserver_ip: "<<config.Load("rpcserver_ip")<<std::endl;
    std::cout<<"rpcserver_port: "<<config.Load("rpcserver_port")<<std::endl;
    std::cout<<"zookeeper_ip: "<<config.Load("zookeeper_ip")<<std::endl;
    std::cout<<"zookeeper_port: "<<config.Load("zookeeper_port")<<std::endl;

    
}

MyRpcApplication &MyRpcApplication::GetInstance()
{
    static MyRpcApplication app;
    return app;
}

MyRpcConfig &MyRpcApplication::GetConfig()
{
    return config;
}
