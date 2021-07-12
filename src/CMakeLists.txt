
add_subdirectory(Core)
add_subdirectory(ReachabilityChecker)

add_executable(verifytapn-${ARCH_TYPE} main.cpp)
target_link_libraries(verifytapn-${ARCH_TYPE} 
	Core 
	ReachabilityChecker
        ${EXTERNAL_INSTALL_LOCATION}/uppaal/lib/libudbm.a
	)
