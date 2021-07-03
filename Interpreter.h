//
// Created by Kevin on 25.06.2021.
//

#pragma once

#include "Types.h"
#include "Value.h"

namespace LibJS {

    class FunctionDeclaration;

    class StackFrame final {
    public:
        Optional <Value> getVariable(const String &name) {
            auto foundVariable = m_variables.find(name);
            if (foundVariable != m_variables.end()) {
                return foundVariable->second;
            }
            return {};
        }

        Value &setVariable(const String &id, Value value) {
            return m_variables[id] = value;
        }

        void dump() const {
            std::cout << "<----------------->" << std::endl;
            for (const auto &var : m_variables) {
                std::cout << var.first << ": " << var.second.toString() << std::endl;
            }
        }

    private:
        HashSet <String, Value> m_variables;
    };

    class Interpreter final {
    public:

        Interpreter() {
            m_stackFrames.emplace_back(StackFrame()); // Global Scope;
        }

        Optional <Value> getVariable(const String &name) {
            if (m_stackFrames.empty()) {
                return {};
            }

            for (int32_t i = m_stackFrames.size() - 1; i >= 0; --i) {
                StackFrame &currentStackFrame = m_stackFrames[i];
                auto value = currentStackFrame.getVariable(name);
                if (value.has_value()) {
                    return value;
                }
            }

            return {}; // Add to global scope?
        }


        Value& declareVariable(const String &id, const Value &value) {
            return m_stackFrames[m_stackFrames.size() - 1].setVariable(id, value);
        }

        Value& setVariable(const String &id, const Value &value) {
            for (int32_t i = m_stackFrames.size() - 1; i >= 0; --i) {
                StackFrame &currentStackFrame = m_stackFrames[i];
                if (currentStackFrame.getVariable(id).has_value()) {
                    return currentStackFrame.setVariable(id, value);
                }
            }
            assert(false);
        }

        void pushStackFrame() {
            m_stackFrames.emplace_back(StackFrame{});
        }

        void popStackFrame() {
            m_stackFrames.pop_back();
        }

        void dumpStack() const {
            std::cout << "Begin Stack Dump:" << std::endl;
            for (const auto &frame : m_stackFrames) {
                frame.dump();
            }
            std::cout << "<----------------->" << std::endl;
            std::cout << "End Stack Dump:" << std::endl;
        }

        void returnFromStackFrame(const Value &value) {

        }

    private:
        Vector <StackFrame> m_stackFrames;
    };

}
