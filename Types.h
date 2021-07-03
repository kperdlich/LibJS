//
// Created by Kevin on 24.06.2021.
//

#pragma once

#include <variant>
#include <string>
#include <memory>
#include <vector>
#include <stack>
#include <unordered_map>
#include <optional>
#include <assert.h>

namespace LibJS {
    template<typename... T>
    using Variant = std::variant<T...>;

    using String = std::string;

    template<typename T>
    using UniquePtr = std::unique_ptr<T>;

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    template<typename T>
    using WeakPtr = std::weak_ptr<T>;

    template<typename T>
    using Vector = std::vector<T>;

    template<typename T>
    using Stack = std::stack<T>;

    template<typename Key, typename Value>
    using HashSet = std::unordered_map<Key, Value>;

    template<typename T>
    using Optional = std::optional<T>;

    template<typename First, typename Second>
    using Pair = std::pair<First, Second>;
}