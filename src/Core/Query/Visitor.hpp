#ifndef VISITOR_HPP_
#define VISITOR_HPP_

#include "boost/any.hpp"

namespace VerifyTAPN
{
	namespace AST
	{
		class NotExpression;
		class OrExpression;
		class AndExpression;
		class AtomicProposition;
		class BoolExpression;
		class Query;
                class NumberExpression;
                class IdentifierExpression;
                class MultiplyExpression;
                class MinusExpression;
                class SubtractExpression;
                class PlusExpression;
                class ArithmeticExpression;

		class Visitor
		{
		public:
			virtual ~Visitor() { };
			virtual void Visit(const NotExpression& expr, boost::any& context) = 0;
			virtual void Visit(const OrExpression& expr, boost::any& context) = 0;
			virtual void Visit(const AndExpression& expr, boost::any& context) = 0;
			virtual void Visit(const AtomicProposition& expr, boost::any& context) = 0;
			virtual void Visit(const BoolExpression& expr, boost::any& context) = 0;
			virtual void Visit(const Query& query, boost::any& context) = 0;
                        virtual void Visit(const NumberExpression& expr, boost::any& context) = 0;
                        virtual void Visit(const IdentifierExpression& expr, boost::any& context) = 0;
                        virtual void Visit(const MultiplyExpression& expr, boost::any& context) = 0;
                        virtual void Visit(const MinusExpression& expr, boost::any& context) = 0;
                        virtual void Visit(const SubtractExpression& expr, boost::any& context) = 0;
                        virtual void Visit(const PlusExpression& expr, boost::any& context) = 0;
		};
	}
}


#endif /* VISITOR_HPP_ */
