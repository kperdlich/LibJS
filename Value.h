//
// Created by Kevin on 23.06.2021.
//
#pragma once

#include <utility>
#include <iostream>
#include "Types.h"

namespace LibJS {
    class BigInt;

    class Value;

    class ScopeNode;

    class Function {
    public:
        Function(const String &name, SharedPtr<class BlockStatement> body)
                : m_name(name),
                  m_body{std::move(body)} {}

        Function(const String &name) {
            m_name = name;
        }

        ~Function() {
            std::cout << "Out of scope" << std::endl;
        }

        String toString() const {
            return "function " + m_name + "() { [native code] }";
        }

        SharedPtr<BlockStatement> body() {
            return m_body;
        }

    private:
        String m_name;
        SharedPtr<BlockStatement> m_body;
    };

    class Value {
    public:
        enum class Type {
            Undefined,
            Null,
            Boolean,
            Number,
            Int,
            String,
            BigInt,
            Object,
            Function
        };

        Value() : m_type(Type::Undefined) {}

        explicit Value(SharedPtr<Function> function) : m_type{Type::Function},
                                                       m_valueAsFunction{function} {}

        explicit Value(float value) : m_type{Type::Number}, m_valueAsDouble{value} {}

        explicit Value(double value) : m_type{Type::Number}, m_valueAsDouble{value} {}

        explicit Value(int32_t value) : m_type{Type::Int}, m_valueAsInt32{value} {}

        explicit Value(bool value) : m_type{Type::Boolean}, m_valueAsBool{value} {}

        explicit Value(String value) : m_type{Type::String}, m_valueAsString{value} {}

        explicit Value(const BigInt *value)
                : m_type{Type::BigInt} {
            m_valueAsBigInt = const_cast<BigInt *>(value);
        }

        bool asBool() const {
            return std::get<bool>(m_valueAsBool);
        }

        double asDouble() const {
            return std::get<double>(m_valueAsDouble);
        }

        int32_t asInt32() const {
            return std::get<int32_t>(m_valueAsInt32);
        }

        const String &asString() const {
            return std::get<String>(m_valueAsString);
        }

        SharedPtr<Function> asFunction() const {
            return std::get<SharedPtr<Function>>(m_valueAsFunction);
        }

        bool isBoolean() const {
            return m_type == Type::Boolean;
        }

        bool isNumber() const {
            return m_type == Type::Number;
        }

        bool isObject() const {
            return m_type == Type::Object;
        }

        bool isUndefined() const {
            return m_type == Type::Undefined;
        }

        bool isNull() const {
            return m_type == Type::Null;
        }

        bool isString() const {
            return m_type == Type::String;
        }

        bool isInt() const {
            return m_type == Type::Int;
        }

        bool isFunction() const {
            return m_type == Type::Function;
        }

        String toString() const {
            if (isString()) {
                return asString();
            }

            if (isNull()) {
                return "null";
            }

            if (isUndefined()) {
                return "undefined";
            }

            if (isBoolean()) {
                return asBool() ? "true" : "false";
            }

            if (isNumber()) {
                return std::to_string(asDouble());
            }

            if (isInt()) {
                return std::to_string(asInt32());
            }

            if (isFunction()) {
                return asFunction()->toString();
            }

            return "Type not defined";
        }

        bool toBoolean() const {
            if (isBoolean()) {
                return asBool();
            }

            if (isString()) {
                return asString().length() > 0;
            }

            if (isNumber()) {
                return asDouble();
            }

            if (isInt()) {
                return asInt32();
            }

            if (isObject() || isFunction()) {
                return true;
            }

            return false;
        }

    private:
        Type m_type;
        Variant<double, bool, int32_t, String, BigInt *, SharedPtr<Function>> m_valueAsDouble, m_valueAsBool,
                m_valueAsInt32, m_valueAsString, m_valueAsBigInt, m_valueAsFunction;
    };


    Value add(const Value &left, const Value &right);

    Value subtract(const Value &left, const Value &right);

    Value divide(const Value &left, const Value &right);

    Value multiply(const Value &left, const Value &right);

    Value greaterThan(const Value &left, const Value &right);

    using JsUndefined = Value;

} // namespace LibJS
