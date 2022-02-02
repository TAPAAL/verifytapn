
add_subdirectory(QueryParser)
add_subdirectory(SymbolicMarking)
add_subdirectory(TAPN)

add_library(Core
	ArgsParser.cpp
	IndirectionTable.cpp
	VerificationOptions.cpp
)

target_link_libraries(Core
	QueryParser
	SymbolicMarking
	TAPN
)




