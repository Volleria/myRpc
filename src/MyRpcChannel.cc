#include "MyRpcChannel.h"
#include <memory>  // 包含智能指针的头文件

/*
    [header_size |service_name method_name] [args_size | args_str]
*/
void MyRpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method, 
                                google::protobuf::RpcController *controller, 
                                const google::protobuf::Message *request, 
                                google::protobuf::Message *response, 
                                google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *ser_desc = method->service();
    std::string service_name = ser_desc->name();
    std::string method_name = method->name();

    // 获取参数的序列化字符串长度 args_size
    std::string args_str;
    int args_size = 0;
    if(!request->SerializeToString(&args_str))
    {
        controller->SetFailed("request.SerializeToString failed");
        return;
    }
    args_size = args_str.size();

    // 定义Rpc的请求header
    MyRpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if(!rpc_header.SerializeToString(&rpc_header_str))
    {
        controller->SetFailed("rpc_header.SerializeToString failed");
        return;
    }
    header_size = rpc_header_str.size();

    // 组装rpc方法调用的请求字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0, (char *)&header_size, 4);
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    // 打印调试信息
    std::cout << "header_size: " << header_size << " rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << " method_name: " << method_name << " args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;

    // 使用智能指针管理 clientfd
    auto closeSocket = [](int *fd) {
        if (*fd != -1) {
            close(*fd);
        }
        delete fd;
    };

    std::unique_ptr<int, decltype(closeSocket)> clientfd(new int(socket(AF_INET, SOCK_STREAM, 0)), closeSocket);

    if (*clientfd == -1)
    {
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error: %s", strerror(errno));
        controller->SetFailed(errtxt);
        return;
    }

    std::string ip = MyRpcApplication::GetInstance().GetConfig().Load("rpcserver_ip");
    uint16_t port = std::stoi(MyRpcApplication::GetInstance().GetConfig().Load("rpcserver_port"));

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr(ip.c_str());

    if(connect(*clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error: %s", strerror(errno));
        controller->SetFailed(errtxt);
        return;

    }

    // 发送rpc请求
    if(send(*clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1)
    {
        char errtxt[512] = {0};
        sprintf(errtxt, "send error: %s", strerror(errno));
        controller->SetFailed(errtxt);
        return;
    }

    // 接收rpc响应
    char recv_buf[1024] = {0};
    int recv_size = recv(*clientfd, recv_buf, 1024, 0);  
    if(recv_size == -1)
    {
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error: %s", strerror(errno));
        controller->SetFailed(errtxt);
        return;
    }

    std::string response_str(recv_buf, recv_size);
    if(!response->ParseFromString(response_str))
    {
        controller->SetFailed("response.ParseFromString failed");
        return;
    }

    // if(!response->ParseFromArray(recv_buf, recv_size))
    // {
    //     std::cerr << "response.ParseFromArray failed" << std::endl;
    //     return;
    // }
}