#include "ToStringVisitor.hpp"
#include "AST.hpp"

namespace VerifyTAPN
{
	namespace AST
	{
		void ToStringVisitor::Visit(const NotExpression& expr, boost::any& context)
		{
			std::cout << "!";
			expr.Child().Accept(*this, context);
		}

		void ToStringVisitor::Visit(const OrExpression& expr, boost::any& context)
		{
			expr.Left().Accept(*this, context);
			std::cout << " or ";
			expr.Right().Accept(*this, context);
		}

		void ToStringVisitor::Visit(const AndExpression& expr, boost::any& context)
		{
			expr.Left().Accept(*this, context);
			std::cout << " and ";
			expr.Right().Accept(*this, context);
		}
                
                // needs implementation
		void ToStringVisitor::Visit(const AtomicProposition& expr, boost::any& context)
		{
//			std::cout << tapn->GetPlace(expr.Place()).GetName() << " " << expr.Operator() << " " << expr.N();
		}
                
                void ToStringVisitor::Visit(const NumberExpression& expr, boost::any& context){};
                void ToStringVisitor::Visit(const IdentifierExpression& expr, boost::any& context){};
                void ToStringVisitor::Visit(const MultiplyExpression& expr, boost::any& context){};
                void ToStringVisitor::Visit(const MinusExpression& expr, boost::any& context){};
                void ToStringVisitor::Visit(const SubtractExpression& expr, boost::any& context){};
                void ToStringVisitor::Visit(const PlusExpression& expr, boost::any& context){};

		void ToStringVisitor::Visit(const BoolExpression& expr, boost::any& context)
		{
			std::cout << (expr.GetValue() ? "true" : "false");
		}

		void ToStringVisitor::Visit(const Query& query, boost::any& context)
		{
			if(query.GetQuantifier() == EF)
				std::cout << "EF ";
			else
				std::cout << "AG ";

			query.Child().Accept(*this, context);
		}
	}
}
