#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
using JObject = nlohmann::json; 


namespace JSON {

    class Object;
    class Array;

    Object ObjectFromFile(std::string path);
    Object ObjectFromString(std::string data);
    Array ArrayFromFile(std::string path);
    Array ArrayFromText(std::string data);


    class Object {
        JObject obj;    
        JObject contextItem;
        bool Contains(std::string& key);
        JObject GetLast();

    public:
            Object(JObject obj) :obj(obj){
            }
            bool HasFloat(std::string key);
            bool FetchBool(std::string key);
            bool FetchInt(std::string key);
            bool FetchString(std::string key);
            bool FetchArray(std::string key);
            bool FetchObject(std::string key);
            float GetFloat();
            bool GetBool();
            int GetInt();
            std::string GetString();
            Object GetObject();
            Array GetArray();
    };
    class Array{
            JObject obj;        
            JObject contextItem;
            JObject GetLast();
            bool Contains(size_t key);
        public:
            Array(JObject obj) : obj(obj) {
            }
            bool FetchFloat(size_t key);
            bool FetchBool(size_t key);
            bool FetchInt(size_t key);
            bool FetchString(size_t key);
            bool FetchArray(size_t key);
            bool FetchObject(size_t key);
            float GetFloat();
            bool GetBool();
            int GetInt();
            std::string GetString();
            Object GetObject();
            Array GetArray();
    };
}

