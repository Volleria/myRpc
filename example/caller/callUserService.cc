#include <iostream>
#include "MyRpcApplication.h"
#include "user.pb.h"


int main(int argc, char **argv)
{

    MyRpcApplication::Init(argc, argv);

    // rpc 请求
    fixbug::UserService_Stub stub(new MyRpcChannel());

    fixbug::LoginRequest request;
    request.set_username("test_name01");
    request.set_password("123456");
    // rpc响应
    fixbug::LoginResponse response;

    MyRpcController controller;

    // 发起rpc调用
    stub.Login(&controller, &request, &response, nullptr);
    // rpc调用完成,读取响应结果
    if(controller.Failed())
    {
        std::cout << "rpc Login failed: " << controller.ErrorText() << std::endl;
    }
    else
    {
        if(response.result_code().error_code() == 0)
        {
            std::cout << "rpc Login response: " << response.success() << std::endl;
        }
        else
        {
            std::cout << "rpc Login response error: " << response.result_code().error_msg() << std::endl;
        }
    }




    fixbug::RegisterRequest request2;
    request2.set_id(1);
    request2.set_username("test_name01");
    request2.set_password("123456");
    // rpc响应
    fixbug::RegisterResponse response2;
    // 发起rpc调用
    stub.Register(NULL, &request2, &response2, nullptr);
    // rpc调用完成,读取响应结果
    if(response2.result_code().error_code() == 0)
    {
        std::cout << "rpc Register response: " << response2.success() << std::endl;
    }
    else
    {
        std::cout << "rpc Register response error: " << response2.result_code().error_msg() << std::endl;
    }




    return 0;
}
