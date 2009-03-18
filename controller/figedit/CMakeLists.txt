link_directories(/opt/boost/lib)
link_libraries(model view utils boost_serialization-gcc42-mt boost_thread-gcc42-mt `wx-config --libs`)
add_executable(figedit wx_app wx_canvas wx_frame)
