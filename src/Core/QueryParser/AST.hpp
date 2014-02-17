#ifndef AST_HPP_
#define AST_HPP_

#include <string>
#include <iostream>
#include "Visitor.hpp"
#include "boost/any.hpp"

namespace VerifyTAPN{
	namespace AST {

		class Visitable
		{
		public:
			virtual void Accept(Visitor& visitor, boost::any& context) const = 0;
		};

		class Expression : public Visitable
		{
		public:
			virtual ~Expression() { };
			//virtual void Accept(Visitor& visitor, boost::any& context) const = 0;
			virtual Expression* clone() const = 0;
		};

		class NotExpression : public Expression
		{
		public:
			explicit NotExpression(Expression* expr) : expr(expr) { };
			NotExpression(const NotExpression& other) : expr(other.expr->clone()) { };
			NotExpression& operator=(const NotExpression& other)
			{
				if(&other != this){
					delete expr;
					expr = other.expr->clone();
				}

				return *this;
			}

			virtual ~NotExpression(){
				if( expr ) delete expr;
			};

			virtual NotExpression* clone() const;
			virtual void Accept(Visitor& visitor, boost::any& context) const;

			const Expression& Child() const { return *expr; }
		private:
			Expression* expr;
		};

		class BoolExpression : public Expression
		{
		public:
			explicit BoolExpression(bool value) : value(value) {};
			virtual ~BoolExpression() { };

			virtual BoolExpression* clone() const;
			virtual void Accept(Visitor& visitor, boost::any& context) const;

			bool GetValue() const { return value; };
		private:
			bool value;
		};

		class AtomicProposition : public Expression
		{
		public:
			AtomicProposition(ArithmeticExpression* left, std::string* op,ArithmeticExpression* right) : left(left), op(op->begin(), op->end()), right(right){};
			AtomicProposition(const AtomicProposition& other) : left(other.left), op(other.op), right(other.right) { };
			AtomicProposition& operator=(const AtomicProposition& other)
			{
				if(&other != this){
					left = other.left;
					op = other.op;
					right = other.right;
				}
				return *this;
			}

			virtual ~AtomicProposition() { };

                        ArithmeticExpression& GetLeft() const {return *left;};
                        ArithmeticExpression& GetRight() const {return *right;};
                        std::string GetOperator() const {return op;};
                         
			virtual AtomicProposition* clone() const;
			virtual void Accept(Visitor& visitor, boost::any& context) const;

			const std::string& Operator() const { return op; }
		private:
			ArithmeticExpression* left;
			std::string op;
			ArithmeticExpression* right;
		};

		class AndExpression : public Expression
		{
		public:
			AndExpression(Expression* left, Expression* right) : left(left), right(right) { };
			AndExpression(const AndExpression& other) : left(other.left->clone()), right(other.right->clone()) {};
			AndExpression& operator=(const AndExpression& other)
			{
				if(&other != this){
					delete left;
					delete right;

					left = other.left->clone();
					right = other.right->clone();
				}
				return *this;
			}

			virtual ~AndExpression() {
				if( left ) delete left;
				if( right ) delete right;
			}

			virtual AndExpression* clone() const;
			void Accept(Visitor& visitor, boost::any& context) const;

			const Expression& Left() const { return *left; }
			const Expression& Right() const { return *right; }
		private:
			Expression* left;
			Expression* right;
		};

		class OrExpression : public Expression
		{
		public:
			OrExpression(Expression* left, Expression* right) : left(left), right(right) { };
			OrExpression(const OrExpression& other) : left(other.left->clone()), right(other.right->clone()) { };
			OrExpression& operator=(const OrExpression& other)
			{
				if(&other != this){
					delete left;
					delete right;

					left = other.left->clone();
					right = other.right->clone();
				}
				return *this;
			}


			virtual ~OrExpression(){
				if( left ) delete left;
				if( right ) delete right;
			};



			virtual OrExpression* clone() const;
			virtual void Accept(Visitor& visitor, boost::any& context) const;

			const Expression& Left() const { return *left; }
			const Expression& Right() const { return *right; }
		private:
			Expression* left;
			Expression* right;
		};
                
                class ArithmeticExpression : public Visitable
                {
                    public:
			virtual ~ArithmeticExpression() { };
			virtual ArithmeticExpression* clone() const = 0;
                };

                class OperationExpression : public ArithmeticExpression {
                protected:

                    OperationExpression(ArithmeticExpression* left, ArithmeticExpression* right) : left(left), right(right) {
                    };

                    OperationExpression(const OperationExpression& other) : left(other.left), right(other.right) {
                    };

                    OperationExpression& operator=(const OperationExpression& other) {
                        if (&other != this) {
                            delete left;
                            left = other.left;
                            delete right;
                            right = other.right;
                        }
                        return *this;
                    }
                    
                    virtual ~OperationExpression() {
                    };
                    
                public:
                    ArithmeticExpression& GetLeft() const {return *left;};
                    ArithmeticExpression& GetRight() const {return *right;};
                    
                protected:
                    ArithmeticExpression* left;
                    ArithmeticExpression* right;
                };

                class PlusExpression : public OperationExpression {
                public:

                    PlusExpression(ArithmeticExpression* left, ArithmeticExpression* right)
                    : OperationExpression(left, right) {
                    };

                    PlusExpression(const PlusExpression& other)
                    : OperationExpression(other) {
                    };

                    PlusExpression& operator=(const PlusExpression& other) {
                        if (&other != this) {
                            left = other.left;
                            right = other.right;
                        }
                        return *this;
                    }

                    virtual ~PlusExpression() {
                    };

                    virtual PlusExpression* clone() const;
                    virtual void Accept(Visitor& visitor, boost::any& context) const;

                };

                class SubtractExpression : public OperationExpression {
                public:

                    SubtractExpression(ArithmeticExpression* left, ArithmeticExpression* right)
                    : OperationExpression(left, right) {
                    };

                    SubtractExpression(const SubtractExpression& other)
                    : OperationExpression(other) {
                    };

                    SubtractExpression& operator=(const SubtractExpression& other) {
                        if (&other != this) {
                            left = other.left;
                            right = other.right;
                        }
                        return *this;
                    }

                    virtual ~SubtractExpression() {
                    };

                    virtual SubtractExpression* clone() const;
                    virtual void Accept(Visitor& visitor,  boost::any& context) const;
                };

                class MinusExpression : public ArithmeticExpression {
                public:
                    MinusExpression(ArithmeticExpression* value) : value(value) {
                    };

                    MinusExpression(const MinusExpression& other)
                    : value(other.value) {
                    };

                    MinusExpression& operator=(const MinusExpression& other) {
                        if (&other != this) {
                            value = other.value;
                        }
                        return *this;
                    }

                    ArithmeticExpression& GetValue() const { return *value;};
                    
                    virtual ~MinusExpression() {
                    };
                    virtual MinusExpression* clone() const;
                    virtual void Accept(Visitor& visitor,  boost::any& context) const;
                private:
                    ArithmeticExpression* value;
                };

                class MultiplyExpression : public OperationExpression {
                public:

                    MultiplyExpression(ArithmeticExpression* left, ArithmeticExpression* right)
                    : OperationExpression(left, right) {
                    };

                    MultiplyExpression(const MultiplyExpression& other)
                    : OperationExpression(other) {
                    };

                    MultiplyExpression& operator=(const MultiplyExpression& other) {
                        if (&other != this) {
                            left = other.left;
                            right = other.right;
                        }
                        return *this;
                    }

                    virtual ~MultiplyExpression() {
                    };
                    virtual MultiplyExpression* clone() const;
                    virtual void Accept(Visitor& visitor,  boost::any& context) const;
                };

                class NumberExpression : public ArithmeticExpression {
                public:

                    NumberExpression(int i) : value(i) {
                    }

                    NumberExpression(const NumberExpression& other) : value(other.value) {
                    };

                    NumberExpression& operator=(const NumberExpression& other) {
                        value = other.value;
                        return *this;
                    };

                    int GetValue() const {return value;};
                    
                    virtual ~NumberExpression() {
                    };
                    virtual NumberExpression* clone() const;
                    virtual void Accept(Visitor& visitor,  boost::any& context) const;
                private:
                    int value;
                };

                class IdentifierExpression : public ArithmeticExpression {
                public:

                    IdentifierExpression(int placeIndex) : place(placeIndex) {
                    }

                    IdentifierExpression(const IdentifierExpression& other) : place(other.place) {
                    };

                    IdentifierExpression& operator=(const IdentifierExpression& other) {
                        place = other.place;
                        return *this;
                    };

                    int GetPlace() const { return place;};
                    
                    virtual ~IdentifierExpression() {
                    };
                    virtual IdentifierExpression* clone() const;
                    virtual void Accept(Visitor& visitor,  boost::any& context) const;
                private:
                    int place;
                };
        
                
		enum Quantifier { EF, AG, EG, AF};

		class Query : public Visitable
		{
		public:
			Query(Quantifier quantifier, Expression* expr) : quantifier(quantifier), expr(expr) { };
			Query(const Query& other) : quantifier(other.quantifier), expr(other.expr->clone()) { };
			Query& operator=(const Query& other)
			{
				if(&other != this){
					delete expr;
					expr = other.expr->clone();
				}
				return *this;
			}

			virtual ~Query() { if( expr ) delete expr; }

			virtual Query* clone() const;
			virtual void Accept(Visitor& visitor, boost::any& context) const;

			Quantifier GetQuantifier() const { return quantifier; }
			const Expression& Child() const { return *expr; }
		private:
			Quantifier quantifier;
			Expression* expr;
		};
	}
}

#endif /* AST_HPP_ */
