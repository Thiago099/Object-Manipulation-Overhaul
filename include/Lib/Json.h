#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
using JObject = nlohmann::json; 


namespace Json {

    class Object;
    class Array;

    Object ObjectFromFile(std::string path);
    Object ObjectFromString(std::string data);
    Array ArrayFromFile(std::string path);
    Array ArrayFromText(std::string data);


    class Object {
        JObject obj;    
        JObject getCaseInsensitive(const std::string& key);
    public:
            Object(JObject obj) :obj(obj){
            }
        bool HasFloat(std::string key);
            bool HasBool(std::string key);
            bool HasInt(std::string key);
            bool HasString(std::string key);
            bool HasArray(std::string key);
            bool HasObject(std::string key);
            float GetFloat(std::string key);
            bool GetBool(std::string key);
            int GetInt(std::string key);
            std::string GetString(std::string key);
            Object GetObject(std::string key);
            Array GetArray(std::string key);
    };
    class Array{
            JObject obj;        
            bool HasKey(size_t key) {
                return (key >= 0) && (key < obj.size());
            }
        public:
            Array(JObject obj) : obj(obj) {
            }
            bool HasFloat(size_t key);
            bool HasBool(size_t key);
            bool HasInt(size_t key);
            bool HasString(size_t key);
            bool HasArray(size_t key);
            bool HasObject(size_t key);
            float GetFloat(size_t key);
            bool GetBool(size_t key);
            int GetInt(size_t key);
            std::string GetString(size_t key);
            Object GetObject(size_t key);
            Array GetArray(size_t key);
    };
}

