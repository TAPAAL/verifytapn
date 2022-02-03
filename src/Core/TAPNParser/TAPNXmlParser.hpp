#ifndef VERIFYTAPN_TAPNXMLPARSER_HPP_
#define VERIFYTAPN_TAPNXMLPARSER_HPP_

#include "../TAPN/TAPN.hpp"

#include <rapidxml.hpp>

namespace VerifyTAPN {
	using namespace VerifyTAPN::TAPN;


	class TAPNXmlParser
	{
	private:
		struct ArcCollections{
			TimedInputArc::Vector inputArcs;
			OutputArc::Vector outputArcs;
			TransportArc::Vector transportArcs;
			InhibitorArc::Vector inhibitorArcs;

		public:
			ArcCollections(const TimedInputArc::Vector& inputArcs, const OutputArc::Vector& outputArcs, const TransportArc::Vector& transportArcs, const InhibitorArc::Vector& inhibitorArcs) : inputArcs(inputArcs), outputArcs(outputArcs), transportArcs(transportArcs), inhibitorArcs(inhibitorArcs) {};
		};
	public: // construction
		explicit TAPNXmlParser() {};

		virtual ~TAPNXmlParser() { /* empty */ };

	public:
		std::shared_ptr<TimedArcPetriNet> Parse(const std::string & filename) const;
		std::vector<int> ParseMarking(const std::string & filename, const TimedArcPetriNet& tapn) const;
	private:
		std::shared_ptr<TimedArcPetriNet> ParseTAPN(const rapidxml::xml_node<> & root) const;

		TimedPlace::Vector ParsePlaces(const rapidxml::xml_node<>& root) const;
		std::shared_ptr<TimedPlace> ParsePlace(const rapidxml::xml_node<>& placeNode) const;

		TimedTransition::Vector ParseTransitions(const rapidxml::xml_node<>& root) const;
		std::shared_ptr<TimedTransition> ParseTransition(const rapidxml::xml_node<>& transitionNode) const;

		ArcCollections ParseArcs(const rapidxml::xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const;
		TransportArc::Vector ParseTransportArcs(const rapidxml::xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const;
		InhibitorArc::Vector ParseInhibitorArcs(const rapidxml::xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const;
		TimedInputArc::Vector ParseInputArcs(const rapidxml::xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const;
		OutputArc::Vector ParseOutputArcs(const rapidxml::xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const;
		std::shared_ptr<TimedInputArc> ParseInputArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const;
		std::shared_ptr<InhibitorArc> ParseInhibitorArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const;
		std::shared_ptr<TransportArc> ParseTransportArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const;
		std::shared_ptr<OutputArc> ParseOutputArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const;
		std::vector<int> ParseInitialMarking(const rapidxml::xml_node<>& root, const TimedArcPetriNet& tapn) const;
	};
}

#endif /* VERIFYTAPN_TAPNXMLPARSER_HPP_ */
