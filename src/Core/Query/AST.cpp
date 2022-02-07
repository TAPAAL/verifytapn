#include "AST.hpp"
#include "TranslationVisitor.h"

namespace VerifyTAPN
{
	namespace AST
	{
		NotExpression* NotExpression::clone() const
		{
			return new NotExpression(*this);
		}

		void NotExpression::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

		void BoolExpression::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

		BoolExpression* BoolExpression::clone() const
		{
			return new BoolExpression(*this);
		}

		void AtomicProposition::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

		AtomicProposition* AtomicProposition::clone() const
		{
			return new AtomicProposition(*this);
		}

		AndExpression* AndExpression::clone() const
		{
			return new AndExpression(*this);
		}

		void AndExpression::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

		OrExpression* OrExpression::clone() const
		{
			return new OrExpression(*this);
		}

		void OrExpression::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

		void PlusExpression::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

                PlusExpression* PlusExpression::clone() const
		{
			return new PlusExpression(*this);
		}

                void SubtractExpression::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

                SubtractExpression* SubtractExpression::clone() const
		{
			return new SubtractExpression(*this);
		}

                void MinusExpression::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

                MinusExpression* MinusExpression::clone() const
		{
			return new MinusExpression(*this);
		}

                void MultiplyExpression::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

                MultiplyExpression* MultiplyExpression::clone() const
		{
			return new MultiplyExpression(*this);
		}

                void NumberExpression::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

                NumberExpression* NumberExpression::clone() const
		{
			return new NumberExpression(*this);
		}

                void IdentifierExpression::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

                IdentifierExpression* IdentifierExpression::clone() const
		{
			return new IdentifierExpression(*this);
		}

		Query* Query::clone() const
		{
			return new Query(*this);
		}

		void Query::Accept(Visitor& Visitor, boost::any& context) const
		{
			Visitor.Visit(*this, context);
		}

        std::unique_ptr<Query> toAST(const unfoldtacpn::PQL::Condition_ptr& ptr, const TAPN::TimedArcPetriNet& tapn) {
            TranslationVisitor v(tapn);
            return v.translate(*ptr);
        }
	}
}
