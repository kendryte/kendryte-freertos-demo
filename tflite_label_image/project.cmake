include_directories(${SDK_ROOT}/src/shared)
target_link_libraries(${PROJECT_NAME} ${SDK_ROOT}/src/shared/libtensorflow-lite.a atomic posix)