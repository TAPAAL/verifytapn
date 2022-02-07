add_subdirectory(PassedWaitingList)
add_subdirectory(QueryChecker)
add_subdirectory(Trace)
add_subdirectory(Search)

add_library(ReachabilityChecker
	SuccessorGenerator.cpp
)

target_link_libraries(ReachabilityChecker
	PassedWaitingList
	QueryChecker
	Trace
	Search
)

add_dependencies(ReachabilityChecker boost-ext)