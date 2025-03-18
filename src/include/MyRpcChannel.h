#pragma once
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <string>
#include "MyRpcHeader.pb.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

class MyRpcChannel : public google::protobuf::RpcChannel
{
public:
    // 所有通过stub代理对象调用的rpc方法都由该函数接收处理
    // 统一做rpc方法调用和网络发送
    /*
        rpc请求的数组组装、数据序列化
        发送rpc请求到rpc服务节点 wait
        接受rpc服务节点的响应数据
        解析rpc响应数据，反序列化
    */
    void CallMethod(const google::protobuf::MethodDescriptor *method,
                    google::protobuf::RpcController *controller,
                    const google::protobuf::Message *request,
                    google::protobuf::Message *response,
                    google::protobuf::Closure *done) override;

}