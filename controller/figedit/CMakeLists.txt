link_libraries(model view utils boost_serialization-vc80-mt-gd-1_38 boost_thread-vc80-mt-gd-1_38)
add_executable(figedit WIN32 wx_app wx_canvas wx_frame colorStaticText)
target_link_libraries(figedit ${wxWidgets_LIBRARIES})
