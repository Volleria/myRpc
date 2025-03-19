#pragma once
#include <string>
#include "LockQueue.h"
#include <time.h>
#include <iostream>
enum
{
    INFO,
    ERROR,
};


class Logger
{
public:
    static Logger& GetInstance();
    void SetLogLevel(int level);   // 设置日志级别
    void Log(std::string msg);     // 打印日志

private:
    int _log_level;
    LockQueue<std::string> _log_queue;

    Logger();
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;

};

// 定义宏
#define LOG_INFO(logmsgformat,...)\
    do \
    { \
        Logger::GetInstance().SetLogLevel(INFO); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        Logger::GetInstance().Log(buf); \
    } while(0);

#define LOG_ERR(logmsgformat,...)\
    do \
    { \
        Logger::GetInstance().SetLogLevel(ERROR); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        Logger::GetInstance().Log(buf); \
    } while(0);