#pragma once
#ifndef __AST_H
#define __AST_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include "token.h"

namespace Compiler {

	class Visitor;
	using std::unique_ptr;
	using std::shared_ptr;

	// Data types available in the language
	/*enum DataType {
		NUMERIC,
		STRING,
		ARRAY
	};*/

	enum class ASTType {
		BLOCK,
		NUMBER,
		NAME,
		ARRAY,
		ASSIGNMENT,
		FUNCCALL,
		BINARYOP,
		UNARYOP,
		TERNARYOP,
		IF,
		FOR,
		FUNCDEF
	};

	// Base AST node class
	class AST
	{
	public:
		virtual ~AST() = default;
		virtual const ASTType getType() = 0;
		// Hook into visitor class
		virtual void accept(Visitor *v) = 0;
	};

	// Represents a block with an arbitrary number of children
	class BlockAST : public AST {
		ASTType type = ASTType::BLOCK;
		std::vector<std::shared_ptr<AST>> children;
	public:
		BlockAST(std::vector<std::shared_ptr<AST>> children)
			: children(std::move(children)) {}
		const ASTType getType() override { return type; };
		std::vector<std::shared_ptr<AST>> getChildren() { return children; };

		// Visitor hook
		void accept(Visitor *v) override;
	};

	// Represents numeric literals.  Everything is a double at the moment??????
	class NumberAST : public AST {
		ASTType type = ASTType::NUMBER;
		double val;
	public:
		NumberAST(double val) : val(val) {}
		const ASTType getType() override { return type; };
		const double getVal() { return val; };

		// Visitor hook
		void accept(Visitor *v) override;

	};

	// Represents a variable
	class NameAST : public AST {
		ASTType type = ASTType::NAME;
		std::string name;
	public:
		NameAST(std::string name) : name(std::move(name)) {}
		const ASTType getType() override { return type; };
		const std::string toString() { return name; };

		// Visitor hook
		void accept(Visitor *v) override;
	};

	// Compound type
	class ArrayAST : public AST {
		ASTType type = ASTType::ARRAY;
		std::unique_ptr<AST> name;
		//DataType type;
	public:
		ArrayAST(std::unique_ptr<AST> name, std::vector<std::shared_ptr<AST>> vals)
			: name(std::move(name)), values(std::move(vals)) {}
		const ASTType getType() override { return type; };
		std::vector<std::shared_ptr<AST>> values;

        std::unique_ptr<AST> getName() { return std::move(name); };

        // Visitor hook
		void accept(Visitor *v) override;
	};

	// Represents an assignment expression
	class AssignmentAST : public AST {
		ASTType type = ASTType::ASSIGNMENT;
		std::unique_ptr<AST> name, rhs;
	public:
		AssignmentAST(std::unique_ptr<AST> name, std::unique_ptr<AST> rhs)
			: name(std::move(name)), rhs(std::move(rhs)) {}
		const ASTType getType() override { return type; };

		std::unique_ptr<AST> getName() { return std::move(name); };
		std::unique_ptr<AST> getRhs() { return std::move(rhs); };

		// Visitor hook
		void accept(Visitor *v) override;
	};

	// Represents a function definition
	class FuncDefAST : public AST {
		ASTType type = ASTType::FUNCDEF;
		std::unique_ptr<AST> name, body;
		std::vector<std::shared_ptr<AST>> args;
		bool isExternal;
	public:
		FuncDefAST(std::unique_ptr<AST> name, bool isExternal, std::vector<std::shared_ptr<AST>> args, unique_ptr<AST> body)
		: name(std::move(name)), isExternal(isExternal), args(std::move(args)), body(std::move(body)) {}
		const ASTType getType() override { return type; };

		std::unique_ptr<AST> getName() { return std::move(name); };
		std::unique_ptr<AST> getBod() { return std::move(body); };
		std::vector<shared_ptr<AST>> getArgs() { return args; };
		bool isExt() { return isExternal; };

		// Visitor hook
		void accept(Visitor *v) override;
	};

	// Represents a function call
	class FuncCallAST : public AST {
		ASTType type = ASTType::FUNCCALL;
		std::unique_ptr<AST> name;
		std::vector<std::shared_ptr<AST>> args;
	public:
		FuncCallAST(std::unique_ptr<AST> name, std::vector<std::shared_ptr<AST>> args)
			: name(std::move(name)), args(std::move(args)) {}
		const ASTType getType() override { return type; };

        std::unique_ptr<AST> getName() { return std::move(name); };
        std::vector<shared_ptr<AST>> getArgs() { return args; };

        // Visitor hook
		void accept(Visitor *v) override;
	};

	// Represents binary operators.  Can have 2 children
	class BinaryOpAST : public AST {
		ASTType type = ASTType::BINARYOP;
		TokenType op;
		std::unique_ptr<AST> lhs, rhs;

	public:
		BinaryOpAST(TokenType op, std::unique_ptr<AST> lhs, std::unique_ptr<AST> rhs)
			: op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
		const ASTType getType() override { return type; };

		// Visitor hook
		void accept(Visitor *v) override;

		std::unique_ptr<AST> getLhs() { return std::move(lhs); };
		std::unique_ptr<AST> getRhs() { return std::move(rhs); };
		TokenType getOp() { return op; };
	};

	// Represents a unary operator
	class UnaryOpAST : public AST {
		ASTType type = ASTType::UNARYOP;
		TokenType op;
		std::unique_ptr<AST> operand;

	public:
		UnaryOpAST(TokenType op, std::unique_ptr<AST> operand)
			: op(op), operand(std::move(operand)) {}
		const ASTType getType() override { return type; };

		// Visitor hook
		void accept(Visitor *v) override;

		std::unique_ptr<AST> getOperand() { return std::move(operand); };
		TokenType getOp() { return op; };
	};

	// Represents a ternary operator
	class TernaryOpAST : public AST {
		ASTType type = ASTType::TERNARYOP;
		std::unique_ptr<AST> condition, thenArm, elseArm;
	public:
		TernaryOpAST(std::unique_ptr<AST> condition, std::unique_ptr<AST> thenArm, std::unique_ptr<AST> elseArm)
			: condition(std::move(condition)), thenArm(std::move(thenArm)), 
			  elseArm(std::move(elseArm)) {}
		const ASTType getType() override { return type; };

		// Visitor hook
		void accept(Visitor *v) override;

		std::unique_ptr<AST> getCond() { return std::move(condition); };
		std::unique_ptr<AST> getThen() { return std::move(thenArm); };
		std::unique_ptr<AST> getElse() { return std::move(elseArm); };

	};

	// If/else statement
	class IfAST : public AST {
		ASTType type = ASTType::IF;
		std::unique_ptr<AST> condition, thenBlock, elseBlock;
	public:
		IfAST(std::unique_ptr<AST> condition, std::unique_ptr<AST> thenBlock, std::unique_ptr<AST> elseBlock)
			: condition(std::move(condition)), thenBlock(std::move(thenBlock)), 
			  elseBlock(std::move(elseBlock)) {}
		const ASTType getType() override { return type; };

		// Visitor hook
		void accept(Visitor *v) override;

		std::unique_ptr<AST> getCond() { return std::move(condition); };
		std::unique_ptr<AST> getThen() { return std::move(thenBlock); };
		std::unique_ptr<AST> getElse() { return std::move(elseBlock); };
	};

	class ForAST : public AST {
		ASTType type = ASTType::FOR;
		// Should this be an AST class?
		std::string varName;
		std::unique_ptr<AST> start, end, step, body;

	public:
		ForAST(std::string varName, std::unique_ptr<AST> start,
			std::unique_ptr<AST> end,
			std::unique_ptr<AST> step,
			std::unique_ptr<AST> body)
			: varName(std::move(varName)), start(std::move(start)), end(std::move(end)),
			step(std::move(step)), body(std::move(body)) {}
		const ASTType getType() override { return type; };

		// Visitor hook
		void accept(Visitor *v) override;

		std::string getVarName() { return varName; };
		std::unique_ptr<AST> getStart() { return std::move(start); };
		std::unique_ptr<AST> getEnd() { return std::move(end); };
		std::unique_ptr<AST> getStep() { return std::move(step); };
		std::unique_ptr<AST> getBody() { return std::move(body); };

	};

	/* AST visitor */

	// Interface for other visitor classes to inherit from (interpreter, compiler)
	class Visitor {
	public:
		virtual void visit(BlockAST* node) = 0;
		virtual void visit(NumberAST* node) = 0;
		virtual void visit(NameAST* node) = 0;
		virtual void visit(ArrayAST* node) = 0;
		virtual void visit(AssignmentAST* node) = 0;
		virtual void visit(FuncCallAST* node) = 0;
		virtual void visit(BinaryOpAST* node) = 0;
		virtual void visit(UnaryOpAST* node) = 0;
		virtual void visit(TernaryOpAST* node) = 0;
		virtual void visit(IfAST* node) = 0;
		virtual void visit(ForAST* node) = 0;
		virtual void visit(FuncDefAST* node) = 0;
	};
}  // namespace Compiler

#endif
