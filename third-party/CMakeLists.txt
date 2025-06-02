add_library(easylogging OBJECT easylogging++.cc)
target_compile_options(easylogging PRIVATE -w)
target_compile_definitions(easylogging PRIVATE AUTO_INITIALIZE_EASYLOGGINGPP)
