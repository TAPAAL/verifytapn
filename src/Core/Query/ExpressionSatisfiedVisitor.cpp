#include "ExpressionSatisfiedVisitor.hpp"
#include "AST.hpp"
#include "../SymbolicMarking/SymbolicMarking.hpp"
#include <exception>

namespace VerifyTAPN
{
	namespace AST
	{

		void ExpressionSatisfiedVisitor::Visit(const NotExpression& expr, boost::any& context)
		{
			boost::any c;
			expr.Child().Accept(*this, c);
			context = !boost::any_cast<bool>(c);
		}

		void ExpressionSatisfiedVisitor::Visit(const OrExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

			context = boost::any_cast<bool>(left) || boost::any_cast<bool>(right);
		}

		void ExpressionSatisfiedVisitor::Visit(const AndExpression& expr, boost::any& context)
		{
			boost::any left, right;
			expr.Left().Accept(*this, left);
			expr.Right().Accept(*this, right);

			context = boost::any_cast<bool>(left) && boost::any_cast<bool>(right);
		}

		void ExpressionSatisfiedVisitor::Visit(const AtomicProposition& expr, boost::any& context)
		{
                    boost::any left, right;
                    expr.GetLeft().Accept(*this, left);
                    expr.GetRight().Accept(*this, right);
                    int iLeft = boost::any_cast<int>(left);
                    int iRight = boost::any_cast<int>(right);
                    context = Compare(iLeft, expr.Operator(), iRight);
		}

                void ExpressionSatisfiedVisitor::Visit(const NumberExpression& expr, boost::any& context){
                    context = expr.GetValue();
                };
                
                void ExpressionSatisfiedVisitor::Visit(const IdentifierExpression& expr, boost::any& context){
                    context = static_cast<int>(marking.NumberOfTokensInPlace(expr.GetPlace()));
                };
                
                void ExpressionSatisfiedVisitor::Visit(const MultiplyExpression& expr, boost::any& context){
                    boost::any left, right;
                    expr.GetLeft().Accept(*this, left);
                    expr.GetRight().Accept(*this, right);
                    context = (boost::any_cast<int>(left) * boost::any_cast<int>(right));
                };
                
                void ExpressionSatisfiedVisitor::Visit(const MinusExpression& expr, boost::any& context){
                    boost::any value;
                    expr.GetValue().Accept(*this, value);
                    context = -boost::any_cast<int>(value);
                };
                
                void ExpressionSatisfiedVisitor::Visit(const SubtractExpression& expr, boost::any& context){
                    boost::any left, right;
                    expr.GetLeft().Accept(*this, left);
                    expr.GetRight().Accept(*this, right);
                    context = (boost::any_cast<int>(left) - boost::any_cast<int>(right));
                };
                
                void ExpressionSatisfiedVisitor::Visit(const PlusExpression& expr, boost::any& context){
                    boost::any left, right;
                    expr.GetLeft().Accept(*this, left);
                    expr.GetRight().Accept(*this, right);
                    context = (boost::any_cast<int>(left) + boost::any_cast<int>(right));
                };
                
		void ExpressionSatisfiedVisitor::Visit(const BoolExpression& expr, boost::any& context)
		{
			context = expr.GetValue();
		}

		void ExpressionSatisfiedVisitor::Visit(const Query& query, boost::any& context)
		{
			query.Child().Accept(*this, context);
		}

		bool ExpressionSatisfiedVisitor::Compare(int numberOfTokensInPlace, const std::string& op, int n) const
		{
			if(op == "<") return numberOfTokensInPlace < n;
			else if(op == "<=") return numberOfTokensInPlace <= n;
			else if(op == "=" || op == "==") return numberOfTokensInPlace == n;
			else if(op == ">=") return numberOfTokensInPlace >= n;
			else if(op == ">") return numberOfTokensInPlace > n;
			else if(op == "!=") return numberOfTokensInPlace != n;
			else
				throw std::exception();
		}
	}
}
