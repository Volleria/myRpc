
set(SRC_LIST callUserService.cc ../user.pb.cc)
add_executable(consumer ${SRC_LIST})

target_link_libraries(consumer MyRpc protobuf)