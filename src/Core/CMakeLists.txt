
add_subdirectory(Query)
add_subdirectory(SymbolicMarking)
add_subdirectory(TAPN)

add_library(Core
	IndirectionTable.cpp
	VerificationOptions.cpp
)

target_link_libraries(Core
	Query
	SymbolicMarking
	TAPN
        Boost::program_options
)

add_dependencies(Core boost-ext)


