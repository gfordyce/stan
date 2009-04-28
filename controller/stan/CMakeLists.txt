link_libraries(model view utils boost_serialization-vc80-mt-1_38 boost_thread-vc80-mt-1_38)
add_executable(stan WIN32 wx_app wx_canvas wx_frame)
target_link_libraries(stan ${wxWidgets_LIBRARIES})
