set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(QueryParser)
add_subdirectory(SymbolicMarking)
add_subdirectory(TAPN)
add_subdirectory(TAPNParser)

add_library(Core 
	ArgsParser.cpp 
	IndirectionTable.cpp 
	VerificationOptions.cpp
)

target_link_libraries(Core 
	QueryParser 
	SymbolicMarking 
	TAPN 
	TAPNParser
)




