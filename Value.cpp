//
// Created by Kevin on 25.06.2021.
//

#include <math.h>
#include "Value.h"

LibJS::Value LibJS::add(const LibJS::Value &left, const LibJS::Value &right) {
    if (left.isNumber() && right.isNumber()) {
        return Value(left.asDouble() + right.asDouble());
    }

    if (left.isInt() && right.isInt()) {
        return Value(left.asInt32() + right.asInt32());
    }

    if (left.isString() && left.isString()) {
        return Value(left.asString() + right.asString());
    }

    if (left.isBoolean() && right.isBoolean()) {
        Value(left.asBool() + right.asBool());
    }

    if (left.isString() || right.isString()) {
        return Value(left.toString() + right.toString());
    }

    return Value(NAN);
}

LibJS::Value LibJS::subtract(const LibJS::Value &left, const LibJS::Value &right) {
    if (left.isNumber() && right.isNumber()) {
        return Value(left.asDouble() - right.asDouble());
    }

    if (left.isInt() && right.isInt()) {
        return Value(left.asInt32() - right.asInt32());
    }

    if (left.isBoolean() && right.isBoolean()) {
        Value(left.asBool() - right.asBool());
    }

    return Value(NAN);
}

LibJS::Value LibJS::divide(const LibJS::Value &left, const LibJS::Value &right) {
    if (left.isNumber() && right.isNumber()) {
        return Value(left.asDouble() / right.asDouble());
    }

    if (left.isInt() && right.isInt()) {
        return Value(left.asInt32() / right.asInt32());
    }

    if (left.isBoolean() && right.isBoolean()) {
        Value(left.asBool() / right.asBool());
    }

    return Value(NAN);

    assert(false);
}

LibJS::Value LibJS::multiply(const LibJS::Value &left, const LibJS::Value &right) {
    if (left.isNumber() && right.isNumber()) {
        return Value(left.asDouble() * right.asDouble());
    }

    if (left.isInt() && right.isInt()) {
        return Value(left.asInt32() * right.asInt32());
    }

    if (left.isBoolean() && right.isBoolean()) {
        Value(left.asBool() * right.asBool());
    }

    return Value(NAN);
}

LibJS::Value LibJS::greaterThan(const LibJS::Value &left, const LibJS::Value &right) {
    if (left.isNumber() && right.isNumber()) {
        return Value{left.asDouble() > right.asDouble()};
    }

    if (left.isInt() && right.isInt()) {
        return Value{left.asInt32() > right.asInt32()};
    }

    if (left.asBool() && right.asBool()) {
        return Value{left.asBool() > right.asBool()};
    }

    if (left.isString() || right.isString()) {
        assert(false); // https://tc39.es/ecma262/#sec-abstract-relational-comparison
    }

    return LibJS::Value();
}
