#include "MyRpcConfig.h"



MyRpcConfig::MyRpcConfig()
{
}

MyRpcConfig::~MyRpcConfig()
{
}

void MyRpcConfig::LoadConfigFile(const char *configFile) 
{
    // 打开配置文件
    FILE *pf = fopen(configFile, "r");
    if (pf == nullptr) {
        throw std::runtime_error("无法打开配置文件: " + std::string(configFile));
    }

    // 逐行读取文件
    char buf[512];
    while (fgets(buf, sizeof(buf), pf) != nullptr) {
        std::string line(buf);
        Trim(line); // 去除首尾空白字符

        // 忽略空行和注释行
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // 查找等号位置
        size_t idx = line.find('=');
        if (idx == std::string::npos) {
            std::cerr << "警告: 配置文件格式错误，缺少等号: " << line << std::endl;
            continue;
        }

        // 提取键和值
        std::string key = line.substr(0, idx);
        std::string value = line.substr(idx + 1);

        // 去除键和值的首尾空白字符
        Trim(key);
        Trim(value);

        // 存储键值对
        configMap[key] = value;
    }

    fclose(pf);
}

std::string MyRpcConfig::Load(const std::string &key)
{
    if(MyRpcConfig::configMap.find(key) != MyRpcConfig::configMap.end())
    {
        return MyRpcConfig::configMap[key];
    }
    else
    {
        return std::string();
    }

}
void MyRpcConfig::Trim(std::string& str) {
    str.erase(0, str.find_first_not_of(" \t")); // 去除行首空白字符
    str.erase(str.find_last_not_of(" \t") + 1); // 去除行尾空白字符
}