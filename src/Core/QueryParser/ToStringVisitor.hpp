#ifndef TOSTRINGVISITOR_HPP_
#define TOSTRINGVISITOR_HPP_

#include "Visitor.hpp"
#include "AST.hpp"
#include "../TAPN/TAPN.hpp"

namespace VerifyTAPN
{
	namespace AST
	{

		class ToStringVisitor : public Visitor
		{
		public:
			ToStringVisitor(const std::shared_ptr<TAPN::TimedArcPetriNet>& tapn) : tapn(tapn) { };
			virtual ~ToStringVisitor() {}
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

			void Print(const Query& query) { boost::any any; query.Accept(*this, any); };
		private:
			const std::shared_ptr<TAPN::TimedArcPetriNet> tapn;
		};
	}
}
#endif /* TOSTRINGVISITOR_HPP_ */
