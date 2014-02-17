#include "BadPlaceVisitor.hpp"

namespace VerifyTAPN{
	namespace AST {

		void BadPlaceVisitor::Visit(const NotExpression& expr, boost::any& context)
		{
			expr.Child().Accept(*this, context);
		}

		void BadPlaceVisitor::Visit(const OrExpression& expr, boost::any& context)
		{
			expr.Left().Accept(*this, context);
			expr.Right().Accept(*this, context);
		}

		void BadPlaceVisitor::Visit(const AndExpression& expr, boost::any& context)
		{
			expr.Left().Accept(*this, context);
			expr.Right().Accept(*this, context);
		}

		void BadPlaceVisitor::Visit(const AtomicProposition& expr, boost::any& context)
		{
			if(expr.Operator() == "=" || expr.Operator() == "==" || expr.Operator() == "!=" || expr.Operator() == "<" || expr.Operator() == "<="){
                                // not entirely sure _WHAT_ a bad place is?
                                expr.GetLeft().Accept(*this, context);
                                expr.GetRight().Accept(*this, context);
			}
		}

		void BadPlaceVisitor::Visit(const BoolExpression& expr, boost::any& context)
		{
		}

		void BadPlaceVisitor::Visit(const Query& query, boost::any& context)
		{
			query.Child().Accept(*this, context);
		}

                void BadPlaceVisitor::Visit(const NumberExpression& expr, boost::any& context) {
                    // ignore
                };

                void BadPlaceVisitor::Visit(const IdentifierExpression& expr, boost::any& context) {
                    badPlaces.push_back(expr.GetPlace());
                };

                void BadPlaceVisitor::Visit(const MultiplyExpression& expr, boost::any& context) {
                    expr.GetLeft().Accept(*this, context);
                    expr.GetRight().Accept(*this, context);
                };

                void BadPlaceVisitor::Visit(const MinusExpression& expr, boost::any& context) {
                    expr.GetValue().Accept(*this, context);
                };

                void BadPlaceVisitor::Visit(const SubtractExpression& expr, boost::any& context) {
                    expr.GetLeft().Accept(*this, context);
                    expr.GetRight().Accept(*this, context);
                };

                void BadPlaceVisitor::Visit(const PlusExpression& expr, boost::any& context) {
                    expr.GetLeft().Accept(*this, context);
                    expr.GetRight().Accept(*this, context);
                };
       }
}
