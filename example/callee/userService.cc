#include <iostream> 
#include <string>
#include "user.pb.h"
#include "MyRpcApplication.h"
#include "RpcProvider.h"

// UserService 本地服务，提供两个进程内的本地方法 Login GetFriendLists
class UserService : public UserServiceRpc // 使用在rpc服务端，rpc服务提供者
{
    public:
    bool Login(std::string name, std::string password)
    {
        std::cout << "UserService::Login" << std::endl;
        std::cout << "name: " << name << " password: " << password << std::endl;
        return true;
    }

    // 重写基类UserServiceRpc的虚函数
    // 1. controller: 用于控制rpc的调用信息
    // 2. request: rpc调用的请求参数
    // 3. response: rpc调用的返回结果
    // 4. done: rpc调用完成后的回调
    // caller  ->  Login  ->  muduo ->  callee
    // callee ->  Login  ->  提交给下面重写的Login方法 
    void Login(::google::protobuf::RpcController* controller,
               const ::fixbug::LoginRequest* request,
               ::fixbug::LoginResponse* response,
               ::google::protobuf::Closure* done) override
    {
        // 框架给业务上报了请求参数LoginRequest，业务获取相应数据做本地业务
        std::string name = request->username();
        std::string password = request->password();
        // 本地业务
        bool loginResult = Login(name, password); 

        // 业务处理完毕，将结果填充到response中
        fixbug::ResultCode* result = response->mutable_result();
        result->set_errcode(0);
        result->set_errmsg("this err message is set by callee");
        response->set_success(loginResult);

        // 业务处理完毕，调用done回调通知框架
        done->Run();
    }
};

int main()
{
    // 框架初始化操作
    MyRpcApplication::Init(argc, argv);

    // 创建一个rpc服务提供者
    RpcProvider provider;
    // 发布服务，把UserService对象发布到rpc节点上
    provider.NotifyService(new UserService());

    // 启动一个线程，开始提供rpc调用服务
    provider.Run(); 
     
    return 0;
}