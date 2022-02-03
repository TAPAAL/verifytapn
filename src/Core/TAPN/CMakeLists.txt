add_library(TAPN
	InhibitorArc.cpp
	Pairing.cpp
	TimedInputArc.cpp
	TimedTransition.cpp
	TimeInvariant.cpp
	OutputArc.cpp
   	TimedArcPetriNet.cpp
      	TimedPlace.cpp
    	TimeInterval.cpp
   	TransportArc.cpp
        TAPNModelBuilder.cpp
)


add_dependencies(TAPN udbm-ext boost-ext)