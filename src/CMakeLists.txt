aux_source_directory(. SRC_LIST)

set(SRC_LIST MyRpcApplication.cc MyRpcConfig.cc MyRpcHeader.cc MyRpcProvider.cc)
add_library(MyRpc ${SRC_LIST})
target_link_libraries(MyRpc muduo_net muduo_base pthread)