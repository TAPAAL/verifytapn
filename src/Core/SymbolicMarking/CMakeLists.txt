add_library(SymbolicMarking
	AbstractMarking.cpp
	DiscretePart.cpp
	StoredMarking.cpp
	DBMMarking.cpp
	DiscretePartInclusionMarking.cpp
	UppaalDBMMarkingFactory.cpp
)


add_dependencies(SymbolicMarking udbm-ext)

#target_link_libraries(SymbolicMarking
#	 ${EXTERNAL_INSTALL_LOCATION}/uppaal/lib/libudbm.a
#)
