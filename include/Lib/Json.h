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


    template<class T>
    Nullable<T> CreateResponseFromObject(JObject value) {
        Nullable<T> result = Nullable<T>();
        if (isObjectType<T>(value)) {
            if constexpr (std::same_as<Object, T>) {
                result = Object(value);
            } else if constexpr (std::same_as<Array, T>) {
                result = Array(value);
            } else {
                result = value;
            }
        }
        return result;

    }

    class Object {
        JObject obj;    
        JObject contextItem;
        bool Contains(std::string& key);
        JObject GetLast();
    public:
        Object() {}
            Object(JObject obj) :obj(obj){
            }
            template<class T>
            inline Nullable<T> Get(std::string key) {
                if (Contains(key)) {
                    return CreateResponseFromObject<T>(GetLast());
                }
                return Nullable<T>();
            }
    };
    class Array{
            JObject obj; 
            size_t i = 0;
            JObject contextItem;
            JObject GetLast();
            bool GetNext();

        public:
            Array() {}
            Array(JObject obj) : obj(obj) {
            }
            template<class T>
            inline std::vector<JSON::Nullable<T>> GetAll() {
                std::vector<Nullable<T>> result;
                while (GetNext()) {
                    auto item = CreateResponseFromObject<T>(GetLast());
                    result.push_back(item);
                }
                return result;
            }
    };

    template <class T>
    class Nullable {
        T value;
        bool isNull = true;;
    public:
        Nullable() {}
        operator bool() const noexcept { return !isNull; }
        T& operator*() {
            return value;  // Assuming 'value' is an integer
        }
        Nullable& operator=(const T& other) {
            value = other;
            isNull = false;
            return *this;  // return a reference to this object
        }
        T* operator->() {
            if (isNull) {
                throw std::runtime_error("Accessing value of a null Nullable object");
            }
            return &value;
        }

        // Const version of operator->
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

