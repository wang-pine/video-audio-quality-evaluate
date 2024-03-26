# CMakeList.txt: CMakeProject1 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("MainProject")

project ("Test1")
project ("Test2")
project ("Test3")
project ("Test4")
project ("Test5")
project ("Test6")
project ("Test7")

# 添加库
include_directories(
	"D:/SDK/ffmpeg-6.0-full_build-shared/include"
	"D:/SDK/curl-8.6.0/include"
)

link_directories(
	"D:/SDK/ffmpeg-6.0-full_build-shared/lib"
	"D:/SDK/curl-8.6.0/build/Win64/VC14.30/DLL Release"
)

set(HEADERS     "ffmHead.h"
				"useCurl.h"
				"CMakeProject1.cpp" 
				"CMakeProject1.h" )
# 将源代码添加到此项目的可执行文件。

add_executable (MainProject "main.cpp" ${HEADERS})

add_executable (Test1 "./tests/test1.cpp" ${HEADERS})
add_executable (Test2 "./tests/test2.cpp" ${HEADERS})
add_executable (Test3 "./tests/test3.cpp" ${HEADERS})
add_executable (Test4 "./tests/test4.cpp" ${HEADERS})
add_executable (Test5 "./tests/test5.cpp" ${HEADERS})
add_executable (Test6 "./tests/test6.cpp" ${HEADERS})
add_executable (Test7 "./tests/test7.cpp" ${HEADERS})

SET(LIBS "avutil" "avcodec" "avfilter" "avdevice" "avformat" "postproc" "swresample" "libcurl")

target_link_libraries(MainProject ${LIBS})

target_link_libraries(Test1 ${libs})
target_link_libraries(Test2 ${libs})
target_link_libraries(Test3 ${libs})
target_link_libraries(Test4 ${libs})
target_link_libraries(Test5 ${libs})
target_link_libraries(Test6 ${libs})
target_link_libraries(Test7 ${libs})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MainProject PROPERTY CXX_STANDARD 20)

  set_property(TARGET Test1 PROPERTY CXX_STANDARD 20)
  set_property(TARGET Test2 PROPERTY CXX_STANDARD 20)
  set_property(TARGET Test3 PROPERTY CXX_STANDARD 20)
  set_property(TARGET Test4 PROPERTY CXX_STANDARD 20)
  set_property(TARGET Test5 PROPERTY CXX_STANDARD 20)
  set_property(TARGET Test6 PROPERTY CXX_STANDARD 20)
  set_property(TARGET Test7 PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。

