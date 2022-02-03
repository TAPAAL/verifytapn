#include "TAPNXmlParser.hpp"

#include <string>
#include <algorithm>
#include <memory>

#include "boost/bind.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include "util.hpp"
#include "../SymbolicMarking/MarkingFactory.hpp"
#include "../SymbolicMarking/DiscretePart.hpp"

namespace VerifyTAPN {
	using namespace rapidxml;

	std::shared_ptr<TimedArcPetriNet> TAPNXmlParser::Parse(const std::string & filename) const
	{
		const std::string contents = VerifyTAPN::ReadFile(filename);
		std::vector<char> charArray(contents.begin(), contents.end());
		charArray.push_back('\0');

		xml_document<> xmldoc;
		xmldoc.parse<0>(&charArray[0]); // we need a char* to the string, and contents.c_str() returns const char*

		xml_node<>* pnml = xmldoc.first_node("pnml");
		if(pnml == 0) throw std::string("invalid file.");

		xml_node<>* root = pnml->first_node("net");
		if(root == 0) throw std::string("invalid file.");

		return ParseTAPN(*root);
	}

	std::vector<int> TAPNXmlParser::ParseMarking(const std::string & filename, const TimedArcPetriNet& tapn) const
	{
		const std::string contents = VerifyTAPN::ReadFile(filename); // not sure if this is a good idea, because it copies to string? Maybe the compiler is smart enough not to make a copy
		std::vector<char> charArray(contents.begin(), contents.end());
		charArray.push_back('\0');

		xml_document<> xmldoc;
		xmldoc.parse<0>(&charArray[0]); // we need a char* to the string, and contents.c_str() returns const char*

		xml_node<>* pnml = xmldoc.first_node("pnml"); // we don't need error handling here, since we are parsing the same file as above
		xml_node<>* root = pnml->first_node("net");
		return ParseInitialMarking(*root, tapn);
	}

	std::shared_ptr<TimedArcPetriNet> TAPNXmlParser::ParseTAPN(const xml_node<>& root) const
	{
		TimedPlace::Vector places = ParsePlaces(root);
		TimedTransition::Vector transitions = ParseTransitions(root);

		TAPNXmlParser::ArcCollections arcs = ParseArcs(root, places, transitions);

		std::shared_ptr<TimedArcPetriNet> tapn = std::make_shared<TimedArcPetriNet>(places, transitions, arcs.inputArcs, arcs.outputArcs, arcs.transportArcs, arcs.inhibitorArcs);

		return tapn;
	}

	TimedPlace::Vector TAPNXmlParser::ParsePlaces(const xml_node<>& root) const
	{
		TimedPlace::Vector places;

		xml_node<>* placeNode = root.first_node("place");
		while(placeNode != NULL){
			std::shared_ptr<TimedPlace> place = ParsePlace(*placeNode);
			places.push_back(place);
			placeNode = placeNode->next_sibling("place");
		}

		return places;
	}

	std::shared_ptr<TimedPlace> TAPNXmlParser::ParsePlace(const xml_node<>& placeNode) const
	{
		std::string id(placeNode.first_attribute("id")->value());
		std::string name(placeNode.first_attribute("name")->value());

		std::string invariantNode = placeNode.first_attribute("invariant")->value();
		TimeInvariant timeInvariant = TimeInvariant::CreateFor(invariantNode);
		return std::make_shared<TimedPlace>(name, id, timeInvariant);
	}

	TimedTransition::Vector TAPNXmlParser::ParseTransitions(const xml_node<>& root) const
	{
		TimedTransition::Vector transitions;

		xml_node<>* transitionNode = root.first_node("transition");
		while(transitionNode != NULL){
			std::shared_ptr<TimedTransition> transition = ParseTransition(*transitionNode);
			transitions.push_back(transition);
			transitionNode = transitionNode->next_sibling("transition");
		}

		return transitions;
	}

	std::shared_ptr<TimedTransition> TAPNXmlParser::ParseTransition(const xml_node<>& transitionNode) const
	{
		std::string id(transitionNode.first_attribute("id")->value());
		std::string name(transitionNode.first_attribute("name")->value());
		return std::make_shared<TimedTransition>(name, id);
	}

	TAPNXmlParser::ArcCollections TAPNXmlParser::ParseArcs(const xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{

		TimedInputArc::Vector inputArcs = ParseInputArcs(root, places, transitions);
		TransportArc::Vector transportArcs = ParseTransportArcs(root, places, transitions);
		InhibitorArc::Vector inhibitorArcs = ParseInhibitorArcs(root,places,transitions);
		OutputArc::Vector outputArcs = ParseOutputArcs(root, places, transitions);

		return ArcCollections(inputArcs, outputArcs, transportArcs, inhibitorArcs);
	}

	TransportArc::Vector TAPNXmlParser::ParseTransportArcs(const rapidxml::xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		TransportArc::Vector transportArcs;
		xml_node<>* arcNode = root.first_node("transportArc");
		while(arcNode != NULL){
			transportArcs.push_back(ParseTransportArc(*arcNode, places, transitions));
			arcNode = arcNode->next_sibling("transportArc");
		}
		return transportArcs;
	}

	InhibitorArc::Vector TAPNXmlParser::ParseInhibitorArcs(const rapidxml::xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		InhibitorArc::Vector inhibitorArcs;
		xml_node<>* arcNode = root.first_node("inhibitorArc");
		while(arcNode != NULL){
			inhibitorArcs.push_back(ParseInhibitorArc(*arcNode, places, transitions));
			arcNode = arcNode->next_sibling("inhibitorArc");
		}
		return inhibitorArcs;
	}

	TimedInputArc::Vector TAPNXmlParser::ParseInputArcs(const xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		TimedInputArc::Vector inputArcs;
		xml_node<>* arcNode = root.first_node("inputArc");
		while(arcNode != NULL){
			inputArcs.push_back(ParseInputArc(*arcNode, places, transitions));
			arcNode = arcNode->next_sibling("inputArc");
		}

		return inputArcs;
	}

	OutputArc::Vector TAPNXmlParser::ParseOutputArcs(const xml_node<>& root, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		OutputArc::Vector outputArcs;
		xml_node<>* arcNode = root.first_node("outputArc");
		while(arcNode != NULL){
			outputArcs.push_back(ParseOutputArc(*arcNode, places, transitions));
			arcNode = arcNode->next_sibling("outputArc");
		}

		return outputArcs;
	}

	std::shared_ptr<TimedInputArc> TAPNXmlParser::ParseInputArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		std::string source = arcNode.first_attribute("source")->value();
		std::string target = arcNode.first_attribute("target")->value();
		std::string interval = arcNode.first_attribute("inscription")->value();

		TimedPlace::Vector::const_iterator place = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::GetId), _1) == source);
		TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::GetId), _1) == target);

		return std::make_shared<TimedInputArc>(*place, *transition, TimeInterval::CreateFor(interval));
	}

	std::shared_ptr<TransportArc> TAPNXmlParser::ParseTransportArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		std::string sourceName = arcNode.first_attribute("source")->value();
		std::string transitionName = arcNode.first_attribute("transition")->value();
		std::string targetName = arcNode.first_attribute("target")->value();
		std::string interval = arcNode.first_attribute("inscription")->value();

		TimedPlace::Vector::const_iterator source = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::GetId), _1) == sourceName);
		TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::GetId), _1) == transitionName);
		TimedPlace::Vector::const_iterator target = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::GetId), _1) == targetName);
		return std::make_shared<TransportArc>(*source, *transition, *target, TimeInterval::CreateFor(interval));
	}

	std::shared_ptr<InhibitorArc> TAPNXmlParser::ParseInhibitorArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		std::string source = arcNode.first_attribute("source")->value();
		std::string target = arcNode.first_attribute("target")->value();

		TimedPlace::Vector::const_iterator place = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::GetId), _1) == source);
		TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::GetId), _1) == target);

		return std::make_shared<InhibitorArc>(*place, *transition);
	}

	std::shared_ptr<OutputArc> TAPNXmlParser::ParseOutputArc(const rapidxml::xml_node<>& arcNode, const TimedPlace::Vector& places, const TimedTransition::Vector& transitions) const
	{
		std::string source = arcNode.first_attribute("source")->value();
		std::string target = arcNode.first_attribute("target")->value();

		TimedTransition::Vector::const_iterator transition = find_if(transitions.begin(), transitions.end(), boost::bind(boost::mem_fn(&TimedTransition::GetId), _1) == source);
		TimedPlace::Vector::const_iterator place = find_if(places.begin(), places.end(), boost::bind(boost::mem_fn(&TimedPlace::GetId), _1) == target);

		return std::make_shared<OutputArc>(*transition, *place);

	}

	std::vector<int> TAPNXmlParser::ParseInitialMarking(const rapidxml::xml_node<>& root, const TimedArcPetriNet& tapn) const
	{
		std::vector<int> markedPlaces;
		xml_node<>* placeNode = root.first_node("place");
		int totalInitTokens = 0;
		while(placeNode != NULL)
		{
			std::string initialMarkingValue = placeNode->first_attribute("initialMarking")->value();
			std::string placeName(placeNode->first_attribute("name")->value());

			boost::algorithm::trim(initialMarkingValue);

			int nTokens = boost::lexical_cast<int>(initialMarkingValue);
			totalInitTokens += nTokens;

			if(nTokens > 0)
			{
				for(int i = 0; i < nTokens; i++) {
					markedPlaces.push_back(tapn.GetPlaceIndex(placeName));
				}
			}

			placeNode = placeNode->next_sibling("place");
		}

		return markedPlaces;
	}


}
