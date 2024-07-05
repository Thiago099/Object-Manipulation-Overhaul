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
                Nullable<T> result = Nullable<T>(key);
                if (Contains(key)) {
                    if (isObjectType<T>(GetLast())) {
                        if constexpr (std::same_as<Object, T>) {
                            result = Object(GetLast());
                        } else if constexpr (std::same_as<Array, T>) {
                            result = Array(GetLast());
                        } else {
                            result = GetLast();
                        }
                    } else {
                        result.SetItemIsInvalidType();
                    }
                } else {
                    result.SetKeyNotFound();
                }
                return result;
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
                    std::string key = std::format("[{}]", i - 1);
                    Nullable<T> item = Nullable<T>(key);
                    if (isObjectType<T>(GetLast())) {
                        if constexpr (std::same_as<Object, T>) {
                            item = Object(GetLast());
                        } else if constexpr (std::same_as<Array, T>) {
                            item = Array(GetLast());
                        } else {
                            item = GetLast();
                        }
                    } else {
                        item.SetItemIsInvalidType();
                    }
                    result.push_back(item);
                }
                return result;
            }
    };

    template <class T>
    class Nullable {
        T value;
        std::string fieldName;
        enum Reason {
            Valid,
            KeyNotFound,
            ItemIsInvalidType
        };
        Reason reason = Reason::Valid;
    public:
        Nullable(std::string& fieldName) : fieldName(fieldName) {}
        std::string GetError() {
            if (reason == KeyNotFound) {
                return std::format("Field {} is not present", fieldName);
            }
            if (reason == ItemIsInvalidType) {
                return std::format("Field {} is expected to be {}", fieldName, TypeName<T>::value);
            }
            return std::format("Unespecified error occured on field {}", fieldName);
        }

        void SetValid() {
            reason = Valid;
        }
        void SetKeyNotFound() {
            reason = KeyNotFound;
        }
        void SetItemIsInvalidType() {
            reason = ItemIsInvalidType;
        }

        operator bool() const noexcept { return reason == Valid; }
        T& operator*() {
            return value;  // Assuming 'value' is an integer
        }
        Nullable& operator=(const T& other) {
            value = other;
            return *this;  // return a reference to this object
        }
        T* operator->() {
            if (reason != Valid) {
                throw std::runtime_error("Accessing value of a null Nullable object");
            }
            return &value;
        }

        // Const version of operator->
        const T* operator->() const {
            if (reason != Valid) {
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

