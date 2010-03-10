# Prepare environment for using wxWidgets
include(${wxWidgets_USE_FILE})

# Prepare environment
add_definitions(-DWXDLLIMPEXP_FRAMEWORK=WXIMPORT)
include_directories(${ISTT_SOURCE_DIR}/Framework)

# Find header and source files
file(GLOB HeaderFiles *.hpp)
file(GLOB SourceFiles *.cpp)

# Have the source file group only contain C++ files
source_group("Source Files" /*.cpp)

# Add resources
#add_resources()

# Add target
#add_executable(Application WIN32 ${HeaderFiles} ${SourceFiles} Application.rc)
add_executable(stan WIN32 ${HeaderFiles} ${SourceFiles})

# Set the target's output name
set_target_properties(stan PROPERTIES OUTPUT_NAME "STAN Animator")

# Link required libraries
target_link_libraries(stan ${wxWidgets_LIBRARIES})

# link_libraries(model view utils boost_serialization-vc80-mt-1_38 boost_thread-vc80-mt-1_38)

#add_executable(stan WIN32 wx_app wx_canvas wx_frame)
#target_link_libraries(stan ${wxWidgets_LIBRARIES})
