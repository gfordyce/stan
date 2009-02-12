link_libraries(model view controller utils boost_serialization-gcc42-mt boost_thread-gcc42-mt `wx-config --libs`)
add_executable(first first.cpp)
add_executable(figtest figtest.cpp)
add_executable(loadfig loadfig.cpp)
