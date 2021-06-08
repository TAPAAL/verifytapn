add_library(SymbolicMarking 
	AbstractMarking.cpp  
	DiscretePart.cpp                  
	StoredMarking.cpp
	DBMMarking.cpp       
	DiscretePartInclusionMarking.cpp  
	UppaalDBMMarkingFactory.cpp
)


target_link_libraries(SymbolicMarking
	${EXTERNAL_INSTALL_LOCATION}/lib/libudbm.a
)
