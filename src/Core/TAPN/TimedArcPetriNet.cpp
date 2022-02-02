#include "TimedArcPetriNet.hpp"

#include "TimeInterval.hpp"
#include <limits>

namespace VerifyTAPN {
    namespace TAPN {

        void TimedArcPetriNet::Initialize(bool useUntimedPlaces) {
            for (unsigned int i = 0; i < places.size(); i++) {
                UpdateMaxConstant(places[i]->GetInvariant());
            }

            //			placeIndices.set_empty_key(""); // assume place always have a name
            //			IndexMap::value_type bottom(TimedPlace::Bottom().GetName(), TimedPlace::BottomIndex());
            //			placeIndices.insert(bottom);
            //			for(unsigned int i = 0; i < places.size(); i++){
            //				IndexMap::value_type pair(places[i]->GetName(), i);
            //				placeIndices.insert(pair);
            //			}

            for (auto* arc : inputArcs)
                UpdateMaxConstant(arc->Interval());

            for (auto* arc : transportArcs)
                UpdateMaxConstant(arc->Interval());

            // CAUTION: This if statement removes orphan transitions.
            //			This changes answers for e.g. DEADLOCK queries if
            //			support for such queries are added later.
            if (transitions.size() > 0) {
                bool hasShownMessage = false;
                TimedTransition::Vector::iterator iter = transitions.begin();
                while (iter != transitions.end()) {
                    if ((*iter)->GetPresetSize() == 0 && (*iter)->GetPostsetSize() == 0) {
                        iter = transitions.erase(iter);
                        if (!hasShownMessage) {
                            std::cout << "Orphaned transitions have been removed." << std::endl << std::endl;
                            hasShownMessage = true;
                        }
                    } else {
                        iter++;
                    }
                }
            }

            GeneratePairings();
            FindMaxConstants();

            if (useUntimedPlaces)
                MarkUntimedPlaces();
        }

        void TimedArcPetriNet::MarkUntimedPlaces() {
            for (TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); ++iter) {
                bool isUntimedPlace = (*iter)->GetInvariant() == TimeInvariant::LS_INF;

                for (TransportArc::Vector::const_iterator arcIter = transportArcs.begin(); arcIter != transportArcs.end(); ++arcIter) {
                    isUntimedPlace = isUntimedPlace && (*arcIter)->Source() != **iter;
                }

                if (isUntimedPlace) {
                    for (TimedInputArc::Vector::const_iterator arcIter = inputArcs.begin(); arcIter != inputArcs.end(); ++arcIter) {
                        if ((*arcIter)->InputPlace() == **iter) {
                            isUntimedPlace = isUntimedPlace && (*arcIter)->Interval().IsZeroInfinity();
                        }
                    }
                }

                if (isUntimedPlace) (*iter)->MarkPlaceAsUntimed();
            }
        }

        void TimedArcPetriNet::FindMaxConstants() {
            for (TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); ++iter) {
                int maxConstant = 0;
                if ((*iter)->GetInvariant() != TimeInvariant::LS_INF) {
                    maxConstant = (*iter)->GetInvariant().GetBound();
                }
                for (TimedInputArc::Vector::const_iterator arcIter = inputArcs.begin(); arcIter != inputArcs.end(); ++arcIter) {
                    if ((*arcIter)->InputPlace() == **iter) {
                        auto* ia = *arcIter;
                        const TAPN::TimeInterval& interval = ia->Interval();

                        const int lowerBound = interval.GetLowerBound();
                        const int upperBound = interval.GetUpperBound();

                        if (upperBound == std::numeric_limits<int>().max())
                            maxConstant = (maxConstant < lowerBound ? lowerBound : maxConstant);
                        else
                            maxConstant = (maxConstant < upperBound ? upperBound : maxConstant);
                    }
                }
                (*iter)->SetMaxConstant(maxConstant);
            }

            for (auto* ta : transportArcs) {
                const_cast<TimedPlace&> (ta->Source()).SetMaxConstant(this->maxConstant);
            }
        }

        void TimedArcPetriNet::UpdateMaxConstant(const TimeInterval& interval) {
            int lowerBound = interval.GetLowerBound();
            int upperBound = interval.GetUpperBound();
            if (lowerBound < std::numeric_limits<int>().max() && lowerBound > maxConstant) {
                maxConstant = lowerBound;
            }
            if (upperBound < std::numeric_limits<int>().max() && upperBound > maxConstant) {
                maxConstant = upperBound;
            }
        }

        void TimedArcPetriNet::UpdateMaxConstant(const TimeInvariant& invariant) {
            int bound = invariant.GetBound();
            if (bound < std::numeric_limits<int>().max() && bound > maxConstant) {
                maxConstant = bound;
            }
        }

        int TimedArcPetriNet::GetPlaceIndex(const std::string& placeName) const {
            int idx = TimedPlace::BottomIndex();
            for (unsigned int i = 0; i < places.size(); ++i) {
                if (places[i]->GetName() == placeName) {
                    idx = i;
                    break;
                }
            }

            return idx;
        }

        void TimedArcPetriNet::Print(std::ostream & out) const {
            out << "TAPN:" << std::endl << "  Places: ";
            for (TimedPlace::Vector::const_iterator iter = places.begin(); iter != places.end(); iter++) {
                out << *(*iter);
                out << std::endl;
            }

            out << std::endl << "  Transitions: ";
            for (TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); iter++) {
                out << *(*iter);
                out << std::endl;
            }

            out << std::endl << "  Input Arcs: ";
            for (TimedInputArc::Vector::const_iterator iter = inputArcs.begin(); iter != inputArcs.end(); iter++) {
                out << *(*iter);
                out << ", ";
            }

            if (transportArcs.size() > 0) {
                out << std::endl << "  Transport Arcs: ";
                for (TransportArc::Vector::const_iterator iter = transportArcs.begin(); iter != transportArcs.end(); iter++) {
                    out << *(*iter);
                    out << ", ";
                }
            }

            if (inhibitorArcs.size() > 0) {
                out << std::endl << "  Inhibitor Arcs: ";
                for (InhibitorArc::Vector::const_iterator iter = inhibitorArcs.begin(); iter != inhibitorArcs.end(); iter++) {
                    out << *(*iter);
                    out << ", ";
                }
            }


            out << std::endl << "  Output Arcs: ";
            for (OutputArc::Vector::const_iterator iter = outputArcs.begin(); iter != outputArcs.end(); iter++) {
                out << *(*iter);
                out << ", ";
            }

            out << std::endl;
        }

        void TimedArcPetriNet::GeneratePairings() {
            for (TimedTransition::Vector::const_iterator iter = transitions.begin(); iter != transitions.end(); ++iter) {
                const TimedTransition& t = *(*iter);
                Pairing p(*this, t);
                pairings.insert(std::pair<TimedTransition, Pairing>(t, p));
            }
        }

        void TimedArcPetriNet::toTAPNXML(std::ostream& out, const std::vector<int>& initial) const {
            out << R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<pnml xmlns="http://www.informatik.hu-berlin.de/top/pnml/ptNetb">
<net id="ComposedModel" type="P/T net">
)";
            for (auto* place : places) {
                auto& inv = place->GetInvariant();
                auto [x, y] = place->getPosition();
                out << "<place id=\"" << place->GetName() << "\" name=\"" << place->GetName()
                    << "\" invariant=\"";
                if (inv.IsBoundStrict()) {
                    if (inv.GetBound() == std::numeric_limits<int>::max())
                        out << "&lt; inf";
                    else
                        out << "&lt;" << inv.GetBound();
                } else {
                    out << "&lt;=" << inv.GetBound();
                }
                out << "\" initialMarking=\"" << initial[place->GetIndex()] << "\">\n";
                out << "\t<graphics><position x=\"" << x << "\" y=\"" << y << "\" /></graphics>\n";
                out << "</place>\n";
            }

            for (auto* transition : transitions) {
                out << "<transition id=\"" << transition->GetName() << "\" name=\"" << transition->GetName()
                    << "\" \">\n";
                auto [x, y] = transition->getPosition();
                out << "\t<graphics><position x=\"" << x << "\" y=\"" << y << "\" /></graphics>\n";
                out << "</transition>\n";
            }

            for (auto* ia : inputArcs) {
                out << "<inputArc inscription=\"" << ia->Interval() << "\" "
                    << "\" source=\"" << ia->InputPlace().GetName() << "\" target=\"" << ia->OutputTransition().GetName() << "\" />\n";
            }

            for (auto* oa : outputArcs) {
                out << "<outputArc "
                    << "\" target=\"" << oa->OutputPlace().GetName()
                    << "\" source=\"" << oa->InputTransition().GetName() << "\" />\n";
            }

            for (auto* ia : inhibitorArcs) {
                out << "<inhibitorArc inscription=\"[0,inf)\" "
                    << "\" source=\"" << ia->InputPlace().GetName()
                    << "\" target=\"" << ia->OutputTransition().GetName() << "\" />\n";
            }

            for (auto* ta : transportArcs) {
                out << "<transportArc inscription=\"" << ta->Interval() << "\" "
                    << "source=\"" << ta->Source().GetName() << "\" "
                    << "transition=\"" << ta->Transition().GetName() << "\" "
                    << "target=\"" << ta->Destination().GetName() << "\" />\n";
            }

            out << "</net></pnml>\n";
        }

    }
}


