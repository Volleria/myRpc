#pragma once

#include "google/protobuf/service.h"
#include <memory>
#include <string>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include "MyRpcApplication.h"
#include <functional>
#include "google/protobuf/descriptor.h"
#include <unordered_map>
#include "RpcHeader.pb.h"
#include <iostream>

// 框架提供的专门服务发布RPC服务的网络对象类
class RpcProvider
{
private:
    muduo::net::EventLoop _eventLoop; // 事件循环

    // 服务对象
    struct ServiceInfo
    {
        google::protobuf::Service *service; // 服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> _methodMap; // 方法名字和方法描述对象的映射
    };
    
    // 存放注册成功的服务对象和其服务方法的列表
    std::unordered_map<std::string, ServiceInfo> _serviceMap;

    void OnConnection(const muduo::net::TcpConnectionPtr &conn);
    void OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp stamp);
    void SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, const std::string &response_str);

public:
    RpcProvider();
    ~RpcProvider();

    // 框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);

    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();
};

