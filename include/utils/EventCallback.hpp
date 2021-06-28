#pragma once

#include <functional>
#include <unordered_set>
#include <set>
#include <concepts>
#include <type_traits>
#include <memory>

template<template<typename Item> typename Container, typename Item>
concept is_valid_container = requires (Container<Item> coll, Item item) {
    coll.erase(item);
    coll.emplace(item);
    coll.clear();
    coll.begin();
    coll.end();
    coll.size();
};

template<class T>
struct AbstractFunction;

template<typename R, typename T, typename... TArgs>
struct AbstractFunction<R (T*, TArgs...)> {
    virtual T* instance() const = 0;
    virtual void* ptr() const = 0;

    virtual R operator()(TArgs... args) const noexcept = 0;
};

template<class T>
struct FunctionWrapper;

template<typename R, typename... TArgs>
struct FunctionWrapper<R (*)(TArgs...)> : AbstractFunction<R (void*, TArgs...)> {
    void* instance() const override {
        return nullptr;
    }
    void* ptr() const override {
        return reinterpret_cast<void*>(held);
    }
    R (*held)(TArgs...);
    template<class F>
    FunctionWrapper(F&& f) : held(f) {}
    R operator()(TArgs... args) const noexcept override {
        if constexpr (std::is_same_v<R, void>) {
            held(args...);
        } else {
            return held(args...);
        }
    }
};

template<typename R, typename T, typename... TArgs>
struct FunctionWrapper<R (T::*)(TArgs...)> : AbstractFunction<R (void*, TArgs...)> {
    void* instance() const override {
        return _instance;
    }
    void* ptr() const override {
        using fptr = R (T::*)(TArgs...);
        union dat {
            fptr wrapper;
            void* data;
        };
        dat d {.wrapper = held};
        return d.data;
    }
    R (T::*held)(TArgs...);
    T* _instance;
    template<class F>
    FunctionWrapper(F&& f, T* inst) : held(f), _instance(inst) {}
    R operator()(TArgs... args) const noexcept override {
        if constexpr (std::is_same_v<R, void>) {
            (reinterpret_cast<T*>(_instance)->*held)(args...);
        } else {
            return (reinterpret_cast<T*>(_instance)->*held)(args...);
        }
    }
};

namespace std {
    template<typename R, typename T, typename... TArgs>
    struct hash<AbstractFunction<R (T*, TArgs...)>> {
        std::size_t operator()(const AbstractFunction<R (T*, TArgs...)>& obj) const noexcept {
            auto seed = std::hash<void*>{}(obj.instance());
            return seed ^ std::hash<void*>{}(reinterpret_cast<void*>(obj.ptr())) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}

template<typename R, typename T, typename... TArgs>
bool operator==(const AbstractFunction<R (T*, TArgs...)>& a, const AbstractFunction<R (T*, TArgs...)>& b) {
    return a.instance() == b.instance() && a.ptr() == b.ptr();
}

template<class T>
struct ThinVirtualLayer;

template<typename R, typename T, typename... TArgs>
struct std::hash<ThinVirtualLayer<R (T*, TArgs...)>>;

template<typename R, typename T, typename... TArgs>
struct ThinVirtualLayer<R (T*, TArgs...)> {
    friend struct std::hash<ThinVirtualLayer<R (T*, TArgs...)>>;
private:
    std::shared_ptr<AbstractFunction<R (T*, TArgs...)>> func;

public:
    template<class F>
    ThinVirtualLayer(F&& f) : func(new FunctionWrapper<R (*)(TArgs...)>(f)) {}
    template<class F, typename Q>
    ThinVirtualLayer(F&& f, Q* inst) : func(new FunctionWrapper<R (Q::*)(TArgs...)>(f, inst)) {}

    R operator()(TArgs... args) const noexcept {
        (*func)(args...);
    }
    void* instance() const {
        return func->instance();
    }
    void* ptr() const {
        return func->ptr();
    }

    bool operator==(const ThinVirtualLayer<R (T*, TArgs...)> other) const {
        return *func == (*other.func);
    }
};

namespace std {
    template<typename R, typename T, typename... TArgs>
    struct hash<ThinVirtualLayer<R (T*, TArgs...)>> {
        std::size_t operator()(const ThinVirtualLayer<R (T*, TArgs...)>& obj) const noexcept {
            return std::hash<AbstractFunction<R (T*, TArgs...)>>{}(*obj.func);
        }
    };
}

// TODO: Make a version of this for C# delegates?
// TODO: Also require the function type to be invokable and all that
template<template<typename> typename Container, typename ...TArgs>
requires (is_valid_container<Container, ThinVirtualLayer<void (void*, TArgs...)>>)
class BasicEventCallback {
private:
    using functionType = ThinVirtualLayer<void (void*, TArgs...)>;
    Container<functionType> callbacks;
public:
    void invoke(TArgs... args) const {
        for (auto& callback : callbacks) {
            callback(args...);
        }
    }

    BasicEventCallback& operator+=(ThinVirtualLayer<void (void*, TArgs...)> callback) {
        callbacks.emplace(std::move(callback));
        return *this;
    }

    BasicEventCallback& operator-=(void (*callback)(TArgs...)) {
        removeCallback(callback);
        return *this;
    }

    BasicEventCallback& operator-=(ThinVirtualLayer<void (void*, TArgs...)> callback) {
        callbacks.erase(callback);
        return *this;
    }

    template<typename T>
    BasicEventCallback& operator-=(void (T::*callback)(TArgs...)) {
        removeCallback(callback);
        return *this;
    }

    void addCallback(void (*callback)(TArgs...)) {
        callbacks.emplace(callback);
    }
    // The instance provide here should have lifetime > calls to invoke.
    // If the provided instance dies before this instance, or before invoke is called, invoke will crash.
    template<typename T>
    void addCallback(void (T::*callback)(TArgs...), T* inst) {
        callbacks.emplace(callback, inst);
    }

    void removeCallback(void (*callback)(TArgs...)) {
        callbacks.erase(callback);
    }

    template<typename T>
    void removeCallback(void (T::*callback)(TArgs...)) {
        // Removal of member functions is expensive because we need to remove all member functions regardless of instance
        for (auto itr = callbacks.begin(); itr != callbacks.end();) {
            union dat {
                decltype(callback) wrapper;
                void* data;
            };
            dat d {.wrapper = callback};
            if (itr->ptr() == d.data) {
                itr = callbacks.erase(itr);
            } else {
                ++itr;
            }
        }
    }
    auto size() {
        return callbacks.size();
    }
    void clear() {
        callbacks.clear();
    }
};


template<typename Item>
using default_ordered_set = std::set<Item>;

template<typename Item>
using default_unordered_set = std::unordered_set<Item>;

// Good default for most
template<typename ...TArgs>
using EventCallback = BasicEventCallback<default_ordered_set, TArgs...>;


template<typename ...TArgs>
using UnorderedEventCallback = BasicEventCallback<default_unordered_set, TArgs...>;