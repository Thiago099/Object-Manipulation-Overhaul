#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include "Lib/Misc.h"
using JObject = nlohmann::json; 

#define NOT_FOUND "Item not found"
#define INVALID_TYPE "Item has a invalid type"

namespace JSON {

    class Object;
    class Array;

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
            void FetchFloat(std::string key, std::function<void(float)> success, std::function<void(std::string)> failure = [](std::string) {});
            void FetchBool(std::string key, std::function<void(bool)> success, std::function<void(std::string)> failure = [](std::string) {});
            void FetchInt(std::string key, std::function<void(int)> success, std::function<void(std::string)> failure = [](std::string) {});
            void FetchString(std::string key, std::function<void(std::string)> success, std::function<void(std::string)> failure = [](std::string) {});
            void FetchObject(std::string key, std::function<void(Object)> success, std::function<void(std::string)> failure = [](std::string) {});
            void FetchArray(std::string key, std::function<void(Array)> success, std::function<void(std::string)> failure = [](std::string) {});
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
            void FetchFloat(std::function<void(float)> success, std::function<void(std::string)> failure = [](std::string) {});
            void FetchBool(std::function<void(bool)> success, std::function<void(std::string)> failure = [](std::string) {});
            void FetchInt(std::function<void(int)> success, std::function<void(std::string)> failure = [](std::string) {});
            void FetchString(std::function<void(std::string)> success, std::function<void(std::string)> failure = [](std::string) {});
            void FetchObject(std::function<void(Object)> success, std::function<void(std::string)> failure = [](std::string) {});
            void FetchArray(std::function<void(Array)> success, std::function<void(std::string)> failure = [](std::string) {});
    };
}

