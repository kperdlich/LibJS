//
// AST based on Esprima
//

#pragma once

#include "Types.h"
#include "Value.h"
#include "Interpreter.h"

namespace LibJS {

    static void printIndent(int32_t indent) {
        for (int32_t i = 0; i < indent; ++i) {
            std::cout << "  ";
        }
    }

    class ASTNode {
    public:
        ASTNode() = default;

        virtual ~ASTNode() {
            std::cout << "~ASTNode" << std::endl;
        };

        virtual Value execute(Interpreter &interpreter) {
            return {};
        }

        virtual void print(int32_t indent) const {}
    };


    class Statement : public ASTNode {
    public:
        virtual ~Statement() {
            std::cout << "~Statement" << std::endl;
        }
    };

    class BlockStatement : public Statement {
    public:
        BlockStatement(Vector<SharedPtr<Statement>> body)
                : m_body{std::move(body)} {}

        virtual ~BlockStatement() {
            std::cout << "~BlockStatement" << std::endl;
        }

        virtual void print(int32_t indent) const override {
            if (!m_body.empty()) {
                printIndent(indent);
                std::cout << "[BlockStatement] " << std::endl;
                printIndent(indent + 1);
                std::cout << "body:" << std::endl;
                for (const auto &child : m_body) {
                    child->print(indent + 2);
                }
            }
        }

        virtual Value execute(Interpreter &interpreter) override {
            for (const auto &statements : m_body) {
                statements->execute(interpreter);
            }
            return {};
        }

    private:
        Vector<SharedPtr<Statement>> m_body;
    };

    class Expression : public ASTNode {
    public:
        virtual ~Expression() override {}
    };

    class Identifier : public Expression {
    public:
        Identifier(const String &name)
                : m_name{name} {}

        virtual void print(int32_t indent) const override {
            printIndent(indent);
            std::cout << "[Identifier]" << std::endl;
            printIndent(indent + 1);
            std::cout << "name: " << m_name << std::endl;
        }

        virtual Value execute(Interpreter &interpreter) override {
            return interpreter.getVariable(m_name).value_or(JsUndefined());
        }

        const String &name() const { return m_name; }

    private:
        String m_name;
    };

    class ScopeNode : public Statement {
    public:
        template<typename T, typename... Args>
        void append(Args &&... args) {
            auto node = std::make_shared<T>(std::forward<Args>(args)...);
            m_body.push_back(std::move(node));
        }

        virtual void print(int32_t indent) const override {
            for (const auto &child : m_body) {
                child->print(indent + 1);
            }
        }

        virtual Value execute(Interpreter &interpreter) {
            for (const auto &child : m_body) {
                child->execute(interpreter);
            }
            return {};
        }

    protected:
        Vector<SharedPtr<Statement>> m_body;
    };

    class FunctionDeclaration : public Statement {
    public:
        FunctionDeclaration(SharedPtr<Identifier> id,
                            Vector<SharedPtr<Identifier>> params,
                            SharedPtr<BlockStatement> body)
                : m_body(body),
                  m_id{id},
                  m_params{params},
                  m_async{false},
                  m_expression{false},
                  m_generator{false} {}

        FunctionDeclaration(SharedPtr<Identifier> id,
                            SharedPtr<BlockStatement> body)
                : m_body(body),
                  m_id(id),
                  m_async{false},
                  m_expression{false},
                  m_generator{false} {}

        virtual ~FunctionDeclaration() {
            std::cout << "~FunctionDeclaration" << std::endl;
        }

        virtual void print(int32_t indent) const override {
            printIndent(indent);
            std::cout << "[FunctionDeclaration]" << std::endl;

            if (m_id) {
                printIndent(indent + 1);
                std::cout << "id: " << std::endl;
                m_id->print(indent + 2);
            }

            if (!m_params.empty()) {
                printIndent(indent + 1);
                std::cout << "params: " << std::endl;
                for (const auto &param : m_params) {
                    param->print(indent + 2);
                }
            }

            printIndent(indent + 1);
            std::cout << "body: " << std::endl;
            m_body->print(indent + 2);

            printIndent(indent + 1);
            std::cout << "async: " << (m_async ? "true" : "false") << std::endl;

            printIndent(indent + 1);
            std::cout << "expression: " << (m_expression ? "true" : "false") << std::endl;

            printIndent(indent + 1);
            std::cout << "generator: " << (m_generator ? "true" : "false") << std::endl;
        }

        virtual Value execute(Interpreter &interpreter) {
            const auto name = m_id->name();
            const auto fn = std::make_shared<Function>(name, m_body);
            std::cout << fn->toString() << std::endl;
            const auto functionValue = Value(fn);
            std::cout << functionValue.toString() << std::endl;
            interpreter.declareVariable(m_id->name(), functionValue);
            return {};
        }

    private:
        SharedPtr<Identifier> m_id;
        SharedPtr<BlockStatement> m_body;
        Vector<SharedPtr<Identifier>> m_params;
        bool m_async;
        bool m_expression;
        bool m_generator;
    };

    class ClassDeclaration : public ScopeNode {

    };

    class Program : public ScopeNode {
    public:
        enum class SourceType {
            Script,
            Module
        };

        explicit Program(SourceType type) : m_sourceType{type} {}

        virtual void print(int32_t indent = 0) const override {
            printIndent(indent);
            std::cout << "[Program Node]" << std::endl;
            printIndent(indent + 1);
            std::cout << "sourceType: " << static_cast<int>(m_sourceType) << std::endl;
            printIndent(indent + 1);
            std::cout << "body: " << std::endl;
            ScopeNode::print(indent + 1);
        }

    private:
        SourceType m_sourceType;
    };

    class Literal : public Expression {
    public:
        Literal(Value value)
                : m_value(value) {}

        virtual void print(int32_t indent) const override {
            printIndent(indent);
            std::cout << "[Literal]" << std::endl;
            printIndent(indent + 1);
            std::cout << "value: " << m_value.toString() << std::endl;
        }

        virtual Value execute(Interpreter &interpreter) override {
            return m_value;
        }

    private:
        Value m_value;
    };


    class CallExpression : public Expression {
    public:
        CallExpression(UniquePtr<Expression> callee, Vector<SharedPtr<Expression>> &&arguments)
                : m_callee{std::move(callee)},
                  m_arguments{arguments} {}

        CallExpression(UniquePtr<Expression> callee)
                : m_callee{std::move(callee)} {}

        virtual void print(int32_t indent) const override {
            printIndent(indent);
            std::cout << "[CallExpression]" << std::endl;

            printIndent(indent + 1);
            std::cout << "callee: " << std::endl;
            m_callee->print(indent + 2);

            if (!m_arguments.empty()) {
                printIndent(indent + 1);
                std::cout << "arguments: " << std::endl;
                for (const auto &arg : m_arguments) {
                    arg->print(indent + 2);
                }
            }
        }

        virtual Value execute(Interpreter &interpreter) {
            Vector<Value> argumentValues;
            for (const auto &args : m_arguments) {
                argumentValues.emplace_back(args->execute(interpreter));
            }
            if (const Identifier *identifier = dynamic_cast<Identifier *>(m_callee.get())) {
                interpreter.pushStackFrame();

                auto functionToCall = interpreter.getVariable(identifier->name());
                assert(functionToCall.has_value());

                auto function = functionToCall.value().asFunction();

                Value calleeReturnValue = functionToCall.value().asFunction()->body()->execute(interpreter);

                interpreter.popStackFrame();
                return calleeReturnValue;

            } else {
                assert(false);
            }
        }

    private:
        UniquePtr<Expression> m_callee;
        Vector<SharedPtr<Expression>> m_arguments;
    };

    class BinaryExpression : public Expression {
    public:
        enum class BinaryOperator {
            Add,
            Subtract,
            Divide,
            Multiply,
            Modulo,
            Power,
            BitwiseAnd,
            BitwiseOr,
            BitwiseXor,
            LeftShift,
            RightShift,
            Equal,
            NotEqual,
            GreaterThanOrEqual,
            GreaterThan,
            LessThan,
            LessThanOrEqual
        };

        BinaryExpression(BinaryOperator op, UniquePtr<Expression> left, UniquePtr<Expression> right)
                : m_operator{op},
                  m_left{std::move(left)},
                  m_right{std::move(right)} {}

        virtual void print(int32_t indent) const override {
            printIndent(indent);
            std::cout << "[BinaryExpression]" << std::endl;

            printIndent(indent + 1);
            std::cout << "operator: ";
            switch (m_operator) {
                case BinaryOperator::Add:
                    std::cout << "+" << std::endl;
                    break;
                case BinaryOperator::Subtract:
                    std::cout << "-" << std::endl;
                    break;
                case BinaryOperator::Divide:
                    std::cout << "/" << std::endl;
                    break;
                case BinaryOperator::Multiply:
                    std::cout << "*" << std::endl;
                    break;
                case BinaryOperator::Modulo:
                    std::cout << "%" << std::endl;
                    break;
                case BinaryOperator::Power:
                    std::cout << "**" << std::endl;
                    break;
                case BinaryOperator::BitwiseAnd:
                    std::cout << "&" << std::endl;
                    break;
                case BinaryOperator::BitwiseOr:
                    std::cout << "|" << std::endl;
                    break;
                case BinaryOperator::BitwiseXor:
                    std::cout << "^" << std::endl;
                    break;
                case BinaryOperator::LeftShift:
                    std::cout << "<<" << std::endl;
                    break;
                case BinaryOperator::RightShift:
                    std::cout << ">>" << std::endl;
                    break;
                case BinaryOperator::Equal:
                    std::cout << "==" << std::endl;
                    break;
                case BinaryOperator::NotEqual:
                    std::cout << "!=" << std::endl;
                    break;
                case BinaryOperator::GreaterThanOrEqual:
                    std::cout << ">=" << std::endl;
                    break;
                case BinaryOperator::GreaterThan:
                    std::cout << ">" << std::endl;
                    break;
                case BinaryOperator::LessThan:
                    std::cout << "<" << std::endl;
                    break;
                case BinaryOperator::LessThanOrEqual:
                    std::cout << "<=" << std::endl;
                    break;

            }
            if (m_left) {
                printIndent(indent + 1);
                std::cout << "left: " << std::endl;
                m_left->print(indent + 2);
            }
            if (m_right) {
                printIndent(indent + 1);
                std::cout << "right: " << std::endl;
                m_right->print(indent + 2);
            }
        }

        virtual Value execute(Interpreter &interpreter) override {
            const Value &valueLeft = m_left->execute(interpreter);
            const Value &valueRight = m_right->execute(interpreter);

            switch (m_operator) {
                case BinaryOperator::Multiply:
                    return multiply(valueLeft, valueRight);
                case BinaryOperator::Divide:
                    return divide(valueLeft, valueRight);
                case BinaryOperator::Add:
                    return add(valueLeft, valueRight);
                case BinaryOperator::Subtract:
                    return subtract(valueLeft, valueRight);
                case BinaryOperator::GreaterThan:
                    return greaterThan(valueLeft, valueRight);
                default:
                    assert(false);
                    break;
            }
        }

    private:
        BinaryOperator m_operator;
        UniquePtr<Expression> m_left;
        UniquePtr<Expression> m_right;
    };

    class ExpressionStatement : public Statement {
    public:
        ExpressionStatement(UniquePtr<Expression> expression)
                : m_expression{std::move(expression)} {}

        virtual void print(int32_t indent) const override {
            printIndent(indent);
            std::cout << "[ExpressionStatement]" << std::endl;

            printIndent(indent + 1);
            std::cout << "expression: " << std::endl;
            m_expression->print(indent + 2);
        }

        virtual Value execute(Interpreter &interpreter) {
            return m_expression->execute(interpreter);
        }

    private:
        UniquePtr<Expression> m_expression;
    };

    class VariableDeclarator : public ASTNode {
    public:

        VariableDeclarator(UniquePtr<Expression> id, UniquePtr<Expression> init)
                : m_id{std::move(id)},
                  m_init{std::move(init)} {}

        virtual void print(int32_t indent) const override {
            printIndent(indent);
            std::cout << "[VariableDeclarator]" << std::endl;
            printIndent(indent + 1);
            std::cout << "id: " << std::endl;
            m_id->print(indent + 2);
            printIndent(indent + 1);
            std::cout << "init: " << std::endl;
            m_init->print(indent + 2);
        }

        virtual Value execute(Interpreter &interpreter) override {
            return m_init->execute(interpreter);
        }

    public:
        UniquePtr<Expression> m_id;
        UniquePtr<Expression> m_init;
    };

    class VariableDeclaration : public Statement {
    public:
        enum class Kind {
            Var,
            Const,
            Let,
        };

        virtual ~VariableDeclaration() {
            std::cout << "~VariableDeclaration" << std::endl;
        }

        VariableDeclaration(Kind kind, Vector<SharedPtr<VariableDeclarator>> &&declarators)
                : m_kind{kind},
                  m_declarators{std::move(declarators)} {}

        virtual Value execute(Interpreter &interpreter) override {
            for (const auto &dec : m_declarators) {
                if (const Identifier *identifier = dynamic_cast<Identifier *>(dec->m_id.get())) {
                    const auto &value = dec->execute(interpreter);
                    interpreter.declareVariable(identifier->name(), value);
                } else {
                    assert(false); // Id Expression not supported
                }
                return {};
            }
        }

        virtual void print(int32_t indent) const override {
            printIndent(indent);
            std::cout << "[VariableDeclaration]" << std::endl;
            printIndent(indent + 1);
            std::cout << "kind: " << static_cast<int>(m_kind) << std::endl;
            printIndent(indent + 1);
            std::cout << "declarators: " << std::endl;
            for (const auto &dec : m_declarators) {
                dec->print(indent + 2);
            }
        }

    private:
        Vector<SharedPtr<VariableDeclarator>> m_declarators;
        Kind m_kind;
    };

    class ReturnStatement : public Statement {
    public:
        ReturnStatement(UniquePtr<Expression> argument)
                : m_argument{std::move(argument)} {}

        virtual void print(int32_t indent) const override {
            printIndent(indent);
            std::cout << "[ReturnStatement]" << std::endl;
            printIndent(indent + 1);
            std::cout << "argument: " << std::endl;
            m_argument->print(indent + 2);
        }

        virtual Value execute(Interpreter &interpreter) override {
            const auto &value = m_argument->execute(interpreter);
            interpreter.returnFromStackFrame(value);
            return value;
        }

    private:
        UniquePtr<Expression> m_argument;
    };


    class AssignmentExpression : public Expression {
    public:
        enum class AssignmentOperator {
            Assignment,
            AdditionAssignment,
            SubtractionAssignment,
            DivisionAssignment,
            MultiplicationAssignment,
            Increment,
            Decrement,
        };

        AssignmentExpression(AssignmentOperator op,
                             UniquePtr<Expression> left,
                             UniquePtr<Expression> right)
                : m_operator{op},
                  m_left{std::move(left)},
                  m_right{std::move(right)} {}

        virtual void print(int32_t indent) const override {
            printIndent(indent);
            std::cout << "[AssignmentExpression]" << std::endl;

            printIndent(indent + 1);
            std::cout << "m_operator: ";
            switch (m_operator) {
                case AssignmentOperator::Assignment:
                    std::cout << "'='" << std::endl;
                    break;
                case AssignmentOperator::AdditionAssignment:
                    std::cout << "'+='" << std::endl;
                    break;
                case AssignmentOperator::SubtractionAssignment:
                    std::cout << "'-='" << std::endl;
                    break;
                case AssignmentOperator::DivisionAssignment:
                    std::cout << "'/='" << std::endl;
                    break;
                case AssignmentOperator::MultiplicationAssignment:
                    std::cout << "'*='" << std::endl;
                    break;
                case AssignmentOperator::Increment:
                    std::cout << "'++'" << std::endl;
                    break;
                case AssignmentOperator::Decrement:
                    std::cout << "'--'" << std::endl;
                    break;
                default:
                    assert(false);
                    break;
            }

            printIndent(indent + 1);
            std::cout << "left: " << std::endl;
            m_left->print(indent + 2);


            printIndent(indent + 1);
            std::cout << "right: " << std::endl;
            m_right->print(indent + 2);
        }

        virtual Value execute(Interpreter &interpreter) override {
            if (const Identifier *identifier = dynamic_cast<Identifier *>(m_left.get())) {
                switch (m_operator) {
                    case AssignmentOperator::Assignment: {
                        interpreter.setVariable(identifier->name(), m_right->execute(interpreter));
                        break;
                    }
                    case AssignmentOperator::AdditionAssignment: {
                        const Value &addedValue = add(m_left->execute(interpreter), m_right->execute(interpreter));
                        interpreter.setVariable(identifier->name(), addedValue);
                        break;
                    }
                    case AssignmentOperator::SubtractionAssignment: {
                        const Value &subtractedValue = subtract(m_left->execute(interpreter),
                                                                m_right->execute(interpreter));
                        interpreter.setVariable(identifier->name(), subtractedValue);
                        break;
                    }
                    case AssignmentOperator::DivisionAssignment: {
                        const Value &value = divide(m_left->execute(interpreter),
                                                    m_right->execute(interpreter));
                        interpreter.setVariable(identifier->name(), value);
                        break;
                    }
                    case AssignmentOperator::MultiplicationAssignment: {
                        const Value &value = multiply(m_left->execute(interpreter),
                                                      m_right->execute(interpreter));
                        interpreter.setVariable(identifier->name(), value);
                        break;
                    }
                    default:
                        assert(false);
                        break;
                }
                return {};
            }
            assert(false);
        }

    private:
        AssignmentOperator m_operator;
        UniquePtr<Expression> m_left;
        UniquePtr<Expression> m_right;
    };

    class IfStatement : public Statement {
    public:
        IfStatement(UniquePtr<Expression> test,
                    UniquePtr<Statement> consequent,
                    UniquePtr<Statement> alternate)
                : m_test{std::move(test)},
                  m_consequent{std::move(consequent)},
                  m_alternate{std::move(alternate)} {}

        IfStatement(UniquePtr<Expression> test,
                    UniquePtr<Statement> consequent)
                : m_test{std::move(test)},
                  m_consequent{std::move(consequent)} {}

        virtual void print(int32_t indent) const override {
            printIndent(indent);
            std::cout << "[IfStatement]" << std::endl;

            printIndent(indent + 1);
            std::cout << "test: " << std::endl;
            m_test->print(indent + 2);

            printIndent(indent + 1);
            std::cout << "consequent: " << std::endl;
            m_consequent->print(indent + 2);

            if (m_alternate) {
                printIndent(indent + 1);
                std::cout << "alternate: " << std::endl;
                m_alternate->print(indent + 2);
            }
        }

        virtual Value execute(Interpreter &interpreter) override {
            const auto &value = m_test->execute(interpreter);
            if (value.toBoolean()) {
                return m_consequent->execute(interpreter);
            } else if (m_alternate) {
                return m_alternate->execute(interpreter);
            }
            return {};
        }

    private:
        UniquePtr<Expression> m_test;
        UniquePtr<Statement> m_consequent;
        UniquePtr<Statement> m_alternate;
    };

}
