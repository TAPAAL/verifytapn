#ifndef UPWARDCLOSEDVISITOR_HPP_
#define UPWARDCLOSEDVISITOR_HPP_

#include "Visitor.hpp"

namespace VerifyTAPN {
namespace AST {
	class UpwardClosedVisitor: public VerifyTAPN::AST::Visitor {
	public:
		UpwardClosedVisitor() : propShouldBeUpward(true) { };
		virtual ~UpwardClosedVisitor() {};
		virtual void Visit(const NotExpression& expr, boost::any& context);
		virtual void Visit(const OrExpression& expr, boost::any& context);
		virtual void Visit(const AndExpression& expr, boost::any& context);
		virtual void Visit(const AtomicProposition& expr, boost::any& context);
		virtual void Visit(const BoolExpression& expr, boost::any& context);
		virtual void Visit(const Query& query, boost::any& context);
                virtual void Visit(const NumberExpression& expr, boost::any& context);
                virtual void Visit(const IdentifierExpression& expr, boost::any& context);
                virtual void Visit(const MultiplyExpression& expr, boost::any& context);
                virtual void Visit(const MinusExpression& expr, boost::any& context);
                virtual void Visit(const SubtractExpression& expr, boost::any& context);
                virtual void Visit(const PlusExpression& expr, boost::any& context);
		bool IsUpwardClosed(const Query& query);

	private:
		bool propShouldBeUpward;
	};
}
}

#endif /* UPWARDCLOSEDVISITOR_HPP_ */
