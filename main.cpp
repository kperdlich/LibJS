#include <iostream>
#include "Types.h"
#include "AST.h"


/**
 * AST for following JS program:
 */
//  const b = 10 * 2;
//  const a = b + 1;
//
//  function inc(a) {
//      return 3 + 1;
//  }
//  inc();

int main() {
    LibJS::UniquePtr<LibJS::Program> program = std::make_unique<LibJS::Program>(LibJS::Program::SourceType::Script);
    program->append<LibJS::VariableDeclaration>(
            LibJS::VariableDeclaration::Kind::Const,
            std::vector<LibJS::SharedPtr<LibJS::VariableDeclarator>>{
                    std::make_shared<LibJS::VariableDeclarator>(
                            std::make_unique<LibJS::Identifier>("b"),
                            std::make_unique<LibJS::BinaryExpression>(
                                    LibJS::BinaryExpression::BinaryOperator::Multiply,
                                    std::make_unique<LibJS::Literal>(LibJS::Value(10)),
                                    std::make_unique<LibJS::Literal>(LibJS::Value(2))
                            )
                    )
            }

    );
    program->append<LibJS::VariableDeclaration>(
            LibJS::VariableDeclaration::Kind::Const,
            std::vector<LibJS::SharedPtr<LibJS::VariableDeclarator>>{
                    std::make_shared<LibJS::VariableDeclarator>(
                            std::make_unique<LibJS::Identifier>("a"),
                            std::make_unique<LibJS::BinaryExpression>(
                                    LibJS::BinaryExpression::BinaryOperator::Add,
                                    std::make_unique<LibJS::Identifier>("b"),
                                    std::make_unique<LibJS::Literal>(LibJS::Value(1))
                            )
                    )
            }
    );

    program->append<LibJS::FunctionDeclaration>(LibJS::FunctionDeclaration(
            std::make_shared<LibJS::Identifier>("inc"),
            std::make_shared<LibJS::BlockStatement>(
                    LibJS::Vector<LibJS::SharedPtr<LibJS::Statement>>{
                            std::make_shared<LibJS::ReturnStatement>(
                                    std::make_unique<LibJS::BinaryExpression>(
                                            LibJS::BinaryExpression::BinaryOperator::Add,
                                            std::make_unique<LibJS::Literal>(LibJS::Value(3)),
                                            std::make_unique<LibJS::Literal>(LibJS::Value(1))
                                    )
                            )
                    }
            )));

    program->append<LibJS::ExpressionStatement>(
            std::make_unique<LibJS::CallExpression>(
                    std::make_unique<LibJS::Identifier>("inc")
            )
    );


    // let t = 2;
    // if (10 > 2) {
    //     t *= 2;
    // }

    program->append<LibJS::VariableDeclaration>(
            LibJS::VariableDeclaration::Kind::Let,
            std::vector<LibJS::SharedPtr<LibJS::VariableDeclarator>>{
                    std::make_shared<LibJS::VariableDeclarator>(
                            std::make_unique<LibJS::Identifier>("t"),
                            std::make_unique<LibJS::Literal>(LibJS::Value(2))
                    )
            }

    );

    program->append<LibJS::IfStatement>(
            std::make_unique<LibJS::BinaryExpression>(
                    LibJS::BinaryExpression::BinaryExpression::BinaryOperator::GreaterThan,
                    std::make_unique<LibJS::Literal>(LibJS::Value(10)),
                    std::make_unique<LibJS::Literal>(LibJS::Value(2))
            ),
            std::make_unique<LibJS::BlockStatement>(
                    LibJS::Vector<LibJS::SharedPtr<LibJS::Statement>>{
                            std::make_shared<LibJS::ExpressionStatement>(
                                    std::make_unique<LibJS::AssignmentExpression>(
                                            LibJS::AssignmentExpression::AssignmentOperator::MultiplicationAssignment,
                                            std::make_unique<LibJS::Identifier>("t"),
                                            std::make_unique<LibJS::Literal>(LibJS::Value(2))
                                    )
                            )
                    }
            )

    );

    program->print();

    LibJS::Interpreter interpreter;
    program->execute(interpreter);
    interpreter.dumpStack();

    return 0;
}
