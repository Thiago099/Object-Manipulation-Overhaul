#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include "Lib/Misc.h"
using JObject = nlohmann::json; 

#define NOT_FOUND "Item not found"
#define INVALID_TYPE "Item has a invalid type"

namespace JSON {
    template<class T>
    class Nullable;
    class Object;
    class Array;
    template <typename T>
    class TypeName;

    Object ObjectFromFile(std::string path);
    Object ObjectFromString(std::string data);
    Array ArrayFromFile(std::string path);
    Array ArrayFromString(std::string data);

    class Token {
    protected:
        template <class T>
        Nullable<T> CreateResponseFromObject(JObject value) {
            Nullable<T> result = Nullable<T>();
            if (isObjectType<T>(value)) {
                if constexpr (std::same_as<Object, T>) {
                    result.Set(Object(value));
                } else if constexpr (std::same_as<Array, T>) {
                    result.Set(Array(value));
                } else {
                    result.Set(value);
                }
            }
            return result;
        }
    };


    class Object :public Token{
        JObject obj;    
        bool GetCaseInsensitive(std::string& key, JObject & outItem);
    public:
        Object() {}
            Object(JObject obj) :obj(obj){
            }
            template<class T>
            inline Nullable<T> Get(std::string key) {
                JObject item;
                if (GetCaseInsensitive(key, item)) {
                    return CreateResponseFromObject<T>(item);
                }
                return Nullable<T>();
            }
    };
    class Array : public Token{
            JObject obj; 
        public:
            Array() {}
            Array(JObject obj) : obj(obj) {
            }
            template<class T>
            inline std::vector<JSON::Nullable<T>> GetAll() {
                std::vector<Nullable<T>> result;
                for (auto& inItem : obj){
                    auto item = CreateResponseFromObject<T>(inItem);
                    result.push_back(item);
                }
                return result;
            }
            template <class T, class E>
            inline std::vector<E> GetAll(std::function<E(T)> tranform) {
                std::vector<E> result;
                for (auto& inItem : obj) {
                    auto item = CreateResponseFromObject<T>(inItem);
                    if (item) {
                        result.push_back(tranform(*item));
                    }
                }
                return result;
            }
    };

    template <class T>
    class Nullable {
        T value;
        bool isNull = true;
    public:
        Nullable() {}
        operator bool() const noexcept { return !isNull; }
        void Set(T newValue) {
            value = newValue;
            isNull = false;
        }
        T& operator*() {
            return value; 
        }
        T* operator->() {
            if (isNull) {
                throw std::runtime_error("Accessing value of a null Nullable object");
            }
            return &value;
        }

        const T* operator->() const {
            if (isNull) {
                throw std::runtime_error("Accessing value of a null Nullable object");
            }
            return &value;
        }
    
    };

    template <typename T>
    inline bool isObjectType(JObject obj) {
        static_assert(true, "Type not implemented");
        return false;
    }

    template <>
    inline bool isObjectType<int>(JObject obj) {
        return obj.is_number();
    }

    template <>
    inline bool isObjectType<float>(JObject obj) {
        return obj.is_number();
    }

    template <>
    inline bool isObjectType<bool>(JObject obj) {
        return obj.is_boolean();
    }

    template <>
    inline bool isObjectType<std::string>(JObject obj) {
        return obj.is_string();
    }

    template <>
    inline bool isObjectType<JSON::Object>(JObject obj) {
        return obj.is_object();
    }

    template <>
    inline bool isObjectType<JSON::Array>(JObject obj) {
        return obj.is_array();
    }

    template <typename T>
    class TypeName {
    public:
        static constexpr const char* value = "";
    };
    
    template <>
    class TypeName<int> {
    public:
        static constexpr const char* value = "Int";
    };

    template <>
    class TypeName<float> {
    public:
        static constexpr const char* value = "Float";
    };

    template <>
    class TypeName<std::string> {
    public:
        static constexpr const char* value = "String";
    };

    template <>
    class TypeName<bool> {
    public:
        static constexpr const char* value = "Bool";
    };

    template <>
    class TypeName<JSON::Object> {
    public:
        static constexpr const char* value = "Object";
    };

    template <>
    class TypeName<JSON::Array> {
    public:
        static constexpr const char* value = "Array";
    };
}

