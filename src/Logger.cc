#include "Logger.h"

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
{
    // 启动专门的写日志线程
    std::thread writeLog([&](){
        for(;;)
        {
            // 获取当天日期,然后取日志信息，写入相应的日志文件当中
            // 1. 获取当天日期
            time_t t = time(0);
            tm *now = localtime(&t);
            char file_name[64] = {0};
            snprintf(file_name, sizeof(file_name), "%d_%02d_%02d_log.txt", (now->tm_year + 1900), (now->tm_mon + 1), now->tm_mday);

            FILE *fp = fopen(file_name, "a+");
            if(fp == nullptr)
            {
                std::cerr << "open/create file: " << file_name << " failed" << std::endl;
                exit(EXIR_FAILURE);
            }
            
            std::string msg = _log_queue.Pop();

            char time_buf[64] = {0};
            snprintf(time_buf, sizeof(time_buf), "%02d:%02d:%02d:=>[%s]",  now->tm_hour, 
                                                    now->tm_min, now->tm_sec,_log_level == INFO ? "INFO" : "ERROR");
            msg = "[" + std::string(time_buf) + "] " + msg + "\n";
            fputs(msg.c_str(), fp);
            fclose(fp);
        }
    });

    writeLog.detach(); // 设置分离线程
}

void Logger::SetLogLevel(int level)
{
    _log_level = level;
}


// 把日志信息写入lockqueue缓冲区当中
void Logger::Log(std::string msg)
{
    _log_queue.Push(msg);
}

