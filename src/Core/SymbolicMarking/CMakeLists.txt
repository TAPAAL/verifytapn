add_library(SymbolicMarking 
	AbstractMarking.cpp  
	DiscretePart.cpp                  
	StoredMarking.cpp
	DBMMarking.cpp       
	DiscretePartInclusionMarking.cpp  
	UppaalDBMMarkingFactory.cpp
)


target_link_libraries(SymbolicMarking
	${EXTERNAL_INSTALL_LOCATION}/lib/libdbm.a
	${EXTERNAL_INSTALL_LOCATION}/lib/libhash.a
	${EXTERNAL_INSTALL_LOCATION}/lib/libbase.a
	${EXTERNAL_INSTALL_LOCATION}/lib/libdebug.a
)
