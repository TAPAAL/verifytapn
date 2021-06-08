set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(Core)
add_subdirectory(ReachabilityChecker)
#add_subdirectory(HashFunctions)

add_executable(verifytapn-${ARCH_TYPE} main.cpp)
add_dependencies(verifytapn-${ARCH_TYPE} udbm-ext)
target_link_libraries(verifytapn-${ARCH_TYPE} 
	Core 
	ReachabilityChecker
	QueryParser
	${EXTERNAL_INSTALL_LOCATION}/lib/libdbm.a
	${EXTERNAL_INSTALL_LOCATION}/lib/libhash.a
	${EXTERNAL_INSTALL_LOCATION}/lib/libbase.a
	)
