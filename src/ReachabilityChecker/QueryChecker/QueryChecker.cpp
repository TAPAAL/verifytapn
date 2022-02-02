#include "QueryChecker.hpp"
#include "../../Core/SymbolicMarking/SymbolicMarking.hpp"
#include "../../Core/Query/ExpressionSatisfiedVisitor.hpp"
#include "boost/any.hpp"
#include "../../Core/Query/AST.hpp"

namespace VerifyTAPN
{
	QueryChecker::QueryChecker(const AST::Query* query) : query(query)
	{
	}

	QueryChecker::~QueryChecker()
	{
	}

	bool QueryChecker::IsExpressionSatisfied(const SymbolicMarking& marking) const
	{
		AST::ExpressionSatisfiedVisitor visitor(marking);
		boost::any any;
		query->Accept(visitor, any);

		return boost::any_cast<bool>(any);
	}
}
