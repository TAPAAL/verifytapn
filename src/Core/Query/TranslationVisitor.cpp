

#include "TranslationVisitor.h"
#include "../TAPN/TAPN.hpp"


namespace VerifyTAPN {
    namespace AST {

        void TranslationVisitor::check_first(bool is_quant) {
            if (!_first_element && is_quant) {
                std::cerr << "Quantifiers only allowed as topmost element" << std::endl;
                std::exit(-1);
            } else if (_first_element && !is_quant) {
                std::cerr << "Topmost element must be a quantifier" << std::endl;
                std::exit(-1);
            }
            _first_element = false;
        }

        AST::Expression* TranslationVisitor::get_e_result() {
            if (!_e_result) {
                std::cerr << "Could not translate query" << std::endl;
                std::exit(-1);
            }
            auto r = _e_result;
            _e_result = nullptr;
            return r;
        }

        AST::ArithmeticExpression* TranslationVisitor::get_a_result() {
            if (!_a_result) {
                std::cerr << "Could not translate query" << std::endl;
                std::exit(-1);
            }
            auto r = _a_result;
            _a_result = nullptr;
            return r;
        }

        TranslationVisitor::TranslationVisitor(const TAPN::TimedArcPetriNet& net)
        : _net(net) {
        }

        std::unique_ptr<Query> TranslationVisitor::translate(const unfoldtacpn::PQL::Condition& condition) {
            condition.visit(*this);
            return std::unique_ptr<Query>(_result.release());
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::NotCondition *element) {
            check_first();
            (*element)[0]->visit(*this);
            _e_result = new AST::NotExpression(get_e_result());
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::AndCondition *element) {
            check_first();
            AST::Expression* old = nullptr;
            for (auto& sub : *element) {
                sub->visit(*this);
                if (old != nullptr)
                    old = new AST::AndExpression(get_e_result(), old);
                else
                    old = get_e_result();

            }
            _e_result = old;
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::OrCondition *element) {
            check_first();
            AST::Expression* old = nullptr;
            for (auto& sub : *element) {
                sub->visit(*this);
                if (old != nullptr)
                    old = new AST::OrExpression(get_e_result(), old);
                else
                    old = get_e_result();

            }
            _e_result = old;
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::LessThanCondition *element) {
            check_first();
            (*element)[0]->visit(*this);
            auto lhs = get_a_result();
            (*element)[1]->visit(*this);
            auto rhs = get_a_result();
            _e_result = new AST::AtomicProposition(lhs, "<", rhs);
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::LessThanOrEqualCondition *element) {
            check_first();
            (*element)[0]->visit(*this);
            auto lhs = get_a_result();
            (*element)[1]->visit(*this);
            auto rhs = get_a_result();
            _e_result = new AST::AtomicProposition(lhs, "<=", rhs);
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::EqualCondition *element) {
            check_first();
            (*element)[0]->visit(*this);
            auto lhs = get_a_result();
            (*element)[1]->visit(*this);
            auto rhs = get_a_result();
            _e_result = new AST::AtomicProposition(lhs, "=", rhs);
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::NotEqualCondition *element) {
            check_first();
            (*element)[0]->visit(*this);
            auto lhs = get_a_result();
            (*element)[1]->visit(*this);
            auto rhs = get_a_result();
            _e_result = new AST::AtomicProposition(lhs, "!=", rhs);
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::DeadlockCondition *element) {
            std::cerr << "deadlock-propositions not supported" << std::endl;
            std::exit(-1);
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::ControlCondition *condition) {
            std::cerr << "control-propositions not supported" << std::endl;
            std::exit(-1);
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::EFCondition *condition) {
            check_first(true);
            (*condition)[0]->visit(*this);
            _result = std::make_unique<Query>(Quantifier::EF, get_e_result());
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::EGCondition *condition) {
            check_first(true);
            (*condition)[0]->visit(*this);
            _result = std::make_unique<Query>(Quantifier::EG, get_e_result());
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::AGCondition *condition) {
            check_first(true);
            (*condition)[0]->visit(*this);
            _result = std::make_unique<Query>(Quantifier::AG, get_e_result());
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::AFCondition *condition) {
            check_first(true);
            (*condition)[0]->visit(*this);
            _result = std::make_unique<Query>(Quantifier::AF, get_e_result());
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::BooleanCondition *element) {
            check_first();
            _e_result = new BoolExpression(element->value());
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::UnfoldedIdentifierExpr *element) {
            check_first();
            auto id = _net.GetPlaceIndex(element->name());
            if (id == TAPN::TimedPlace::BottomIndex()) {
                std::cerr << "Could not find " << element->name() << std::endl;
                std::exit(-1);
            }
            _a_result = new IdentifierExpression(id);
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::LiteralExpr *element) {
            check_first();
            _a_result = new NumberExpression(element->value());
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::PlusExpr *element) {
            ArithmeticExpression* old = nullptr;
            check_first();
            for (auto& e : (*element)) {
                e->visit(*this);
                if (old != nullptr)
                    old = new PlusExpression(old, get_a_result());
                else
                    old = get_a_result();
            }
            _a_result = old;
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::MultiplyExpr *element) {
            ArithmeticExpression* old = nullptr;
            check_first();
            for (auto& e : (*element)) {
                e->visit(*this);
                if (old != nullptr)
                    old = new MultiplyExpression(old, get_a_result());
                else
                    old = get_a_result();
            }
            _a_result = old;
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::MinusExpr *element) {
            check_first();
            (*element)[0]->visit(*this);
            _a_result = new AST::MinusExpression(get_a_result());
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::SubtractExpr *element) {
            ArithmeticExpression* old = nullptr;
            check_first();
            for (auto& e : (*element)) {
                e->visit(*this);
                if (old != nullptr)
                    old = new SubtractExpression(old, get_a_result());
                else
                    old = get_a_result();
            }
            _a_result = old;
        }

        void TranslationVisitor::_accept(const unfoldtacpn::PQL::IdentifierExpr *element) {
            check_first();
            if (element->compiled()) {
                element->compiled()->visit(*this);
            } else {
                auto id = _net.GetPlaceIndex(element->name());
                if (id == TAPN::TimedPlace::BottomIndex()) {
                    std::cerr << "Could not find " << element->name() << std::endl;
                    std::exit(-1);
                }
                _a_result = new IdentifierExpression(id);
            }
        }
    }
}