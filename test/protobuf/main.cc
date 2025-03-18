#include "test.pb.h"
#include <iostream>
#include <string>

using namespace fixbug;

int main()
{
    LoginRequest req;
    req.set_username("test_name01");
    req.set_password("123456");

    std::string send_data;
    if(req.SerializeToString(&send_data);)
    {
        std::cout << "send_data: " << send_data << std::endl;
    }

    LoginRequest req2;
    if(req2.ParseFromString(send_data))
    {
        std::cout << "username: " << req2.username() << std::endl;
        std::cout << "password: " << req2.password() << std::endl;
    }
    
    return 0;
}