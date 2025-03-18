#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>

// 框架读取配置文件类
// rpcserver IP+Port  zookeeper IP+Port
class MyRpcConfig
{
private:
    std::unordered_map<std::string, std::string> configMap;
    void Trim(std::string &srcBuf);

public:
    MyRpcConfig();
    ~MyRpcConfig();

    // 加载配置文件
    void LoadConfigFile(const char *configFile);

    // 读取配置文件
    std::string Load(const std::string &key);

};


