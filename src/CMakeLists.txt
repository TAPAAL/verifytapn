
add_subdirectory(Core)
add_subdirectory(ReachabilityChecker)

add_executable(verifytapn-${ARCH_TYPE} main.cpp)
target_link_libraries(verifytapn-${ARCH_TYPE} PRIVATE
	Core
	ReachabilityChecker
        -ldbm -lbase -ludebug -lhash
        ${EXTERNAL_INSTALL_LOCATION}/lib/unfoldtacpn/libunfoldtacpn.a
)

add_dependencies(verifytapn-${ARCH_TYPE} unfoldtacpn-ext)
