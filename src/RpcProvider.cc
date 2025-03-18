#include "RpcProvider.h"



RpcProvider::RpcProvider()
{
}

RpcProvider::~RpcProvider()
{
}
// service对象中有服务的描述对象，可以获取服务的描述对象
void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    // service对象中有服务的描述对象，可以获取服务的描述对象
    const google::protobuf::ServiceDescriptor *ser_desc = service->GetDescriptor();
    // 获取服务的名字
    std::string service_name = ser_desc->full_name(); 
    // 获取服务的方法的数量
    int methodcnt = ser_desc->method_count();

    for(int i = 0;i < methodcnt ; i++)
    {
        // 获取服务的方法的描述对象
        const google::protobuf::MethodDescriptor *method_desc = ser_desc->method(i);
        // 获取服务的方法名字
        std::string method_name = method_desc->name();
        service_info._methodMap.insert({method_name, method_desc});
    }
    service_info.service = service;
    _serviceMap.insert({service_name, service_info});
}

void RpcProvider::Run()
{
    std::string ip = MyRpcApplication::GetInstance().GetConfig().Load("rpcserver_ip");
    uint16_t port = std::stoi(MyRpcApplication::GetInstance().GetConfig().Load("rpcserver_port"));

    muduo::net::InetAddress address(ip, port);
    // 创建一个TcpServer对象
    muduo::net::TcpServer server(&_eventLoop, address, "RpcProvider");

    // 绑定 连接回调 和 消息读写回调 方法， 好处：分离了网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置muduo库的线程数量
    server.setThreadNum(4);
    std::cout << "RpcProvider::Run server ip: " << ip << " port: " << port << std::endl;


    // 开启muduo库的事件循环
    server.start();
    _eventLoop.loop();

}

// 新的socket连接回调
// RPC是短链接请求，每次请求都是一个新的链接，链接建立后，客户端会发送一个RPC请求，服务端响应后链接就断开
// 所以这里的OnConnection方法中，只需要获取链接建立成功的信息即可
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if(!conn->connected())
    {
        conn->shutdown();
    }
    else
    {
        std::cout << "RpcProvider::OnConnection new connection: " << conn->peerAddress().toIpPort() << std::endl;
    }
    
}
/*
在框架内部，RPC服务提供者RpcProvider类中有一个成员函数OnMessage，用于处理RPC请求消息。
在OnMessage函数中，我们需要解析RPC请求消息，提取出方法名和参数，然后根据方法名找到对应的服务对象和方法对象，最后调用服务对象的方法。

RPC方法发布成类， 携带的数据包含 service_name  menthod_name  args
RpcProvider 和 RpcConsumer 都是RPC框架的类，RpcProvider是RPC服务提供者，RpcConsumer是RPC服务消费者。
RpcProvider中有一个成员函数NotifyService，用于注册RPC服务对象，RpcConsumer中有一个成员函数CallMethod，用于调用RPC服务对象的方法。
RpcProvider和RpcConsumer都有一个成员函数OnMessage，用于处理RPC请求消息。
*/

// header_size + header_str + args_str
// 消息读写回调，如果远程有一个RPC服务的调用请求，那么OnMessage方法就会响应
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp stamp)
{
    // 网络上接收的RPC消息是一个完整的RPC请求   方法名+参数
    std::string recive_buf = buffer->retrieveAllAsString(); 
    std::cout << "OnMessage: new message: " << recive_buf << " timestamp: " << stamp.toFormattedString() << std::endl;

    // 从字符流种解析出消息头
    uint32_t header_size = 0;

    // 从0开始，拷贝4个字节到header_size中
    recive_buf.copy((char *)&header_size, 4, 0); 

    // 从4开始，拷贝header_size个字节到header_str中
    std::string rpc_header_str = recive_buf.substr(4, header_size);
    MyRpc::RpcHeader rpc_header;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    if(rpc_header.ParseFromString(rpc_header_str))
    {
        // 数据头反序列化成功
        service_name = rpc_header.service_name();
        method_name = rpc_header.method_name();
        args_size = rpc_header.args_size();
    }
    else
    {
        std::cerr << "rpc_header.ParseFromString failed" << std::endl;
        return;
    }

    std::string args_str = recive_buf.substr(4 + header_size, args_size);

    // 打印调试信息
    std::cout << "header_size: " << header_size << " rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << " method_name: " << method_name << " args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;

    // 获取service对象 和 method对象
    auto it = _serviceMap.find(service_name);
    if(it == _serviceMap.end())
    {
        std::cerr << "Can't find service: " << service_name << std::endl;
        return;
    }

    
    auto method_it = it->second._methodMap.find(method_name);
    if(method_it == it->second._methodMap.end())
    {
        std::cerr << "Can't find method: " << method_name << std::endl;
        return;
    }
    
    auto service = it->second.service;
    auto method = method_it->second;

    // 生成RPC方法调用的请求request和响应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str))
    {
        std::cerr << "request.ParseFromString failed" << std::endl;
        return;
    }

    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 给下面的method调用传递一个空的Closure对象
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, 
                                                                    const muduo::net::TcpConnectionPtr&, 
                                                                    google::protobuf::Message*>
                                                                    (this, 
                                                                    &RpcProvider::SendRpcResponse, 
                                                                    conn, response);

    // 调用RPC方法
    service->CallMethod(method, nullptr, request, response, done);

}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message* response)
{
    std::string resopnse_str;
    if(response->SerializeToString(&resopnse_str))
    {
        // 序列化成功后，将rpc方法调用的响应发送给rpc方法调用方
        conn->send(resopnse_str);
    }
    else
    {
        std::cerr << "response.SerializeToString failed" << std::endl;
    }
    conn->shutdown();
}
