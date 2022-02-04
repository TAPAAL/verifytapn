add_library(Trace
	ConcreteMarking.cpp
	EntrySolver.cpp
	TraceStore.cpp
)
add_dependencies(Trace rapidxml-ext udbm-ext sparsehash-ext)
