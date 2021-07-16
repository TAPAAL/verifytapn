add_library(Search SearchStrategy.cpp)
target_link_libraries(Search Trace ReachabilityChecker)
