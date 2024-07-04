#include "Lib/Json.h"


bool equals(const std::string& a, const std::string& b) {
    return std::equal(a.begin(), a.end(), b.begin(), [](char a, char b) { return std::toupper(a) == std::toupper(b); });
}
bool JSON::Object::Contains(std::string& key) {
    for (const auto item : obj.items()) {
        if (equals(item.key(), key)) {
            contextItem = item.value();
            return true;
        }
    }
    return false;
}
JObject JSON::Object::GetLast() {
   return contextItem;
}

bool ReadFile(JObject & obj, std::string path) {
    std::ifstream i(path);
    if (i.is_open()) {
        i >> obj;
        i.close();
        return true;
    }
    return false;
}

JObject ReadText(std::string data) {
    return JObject::parse(data);
}

JSON::Object JSON::ObjectFromFile(std::string path) {
    JObject j;
    if (ReadFile(j, path)) {
        if (j.is_object()) {
            return Object(j);
        }
    }
    return Object(JObject::parse("{}"));
}
JSON::Object JSON::ObjectFromString(std::string data) {
    return Object(ReadText(data));
}
JSON::Array JSON::ArrayFromFile(std::string path) {
    JObject j;
    if (ReadFile(j, path)) {
        if (j.is_array()) {
            return Array(j);
        }
    }
    return Array(JObject::parse("[]"));
}
JSON::Array JSON::ArrayFromText(std::string data) {
    return Array(ReadText(data)); }

bool JSON::Object::HasFloat(std::string key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_number_float();
}

bool JSON::Object::FetchBool(std::string key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_boolean();
}

bool JSON::Object::FetchInt(std::string key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_boolean();
}

bool JSON::Object::FetchString(std::string key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_string();
}

bool JSON::Object::FetchArray(std::string key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_array();
}

bool JSON::Object::FetchObject(std::string key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_object();
}

float JSON::Object::GetFloat() { return GetLast(); }

bool JSON::Object::GetBool() { return GetLast(); }

int JSON::Object::GetInt() { return GetLast(); }

std::string JSON::Object::GetString() { return GetLast(); }

JSON::Object JSON::Object::GetObject() { return Object(GetLast()); }

JSON::Array JSON::Object::GetArray() { return Array(GetLast()); }

JObject JSON::Array::GetLast(){ 
    return contextItem;
}

bool JSON::Array::Contains(size_t key) { 
    auto exists = (key >= 0) && (key < obj.size());
    contextItem = obj[key];
    return exists;
}

bool JSON::Array::FetchFloat(size_t key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_number_float();
}

bool JSON::Array::FetchBool(size_t key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_boolean();
}

bool JSON::Array::FetchInt(size_t key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_boolean();
}

bool JSON::Array::FetchString(size_t key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_string();
}

bool JSON::Array::FetchArray(size_t key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_array();
}

bool JSON::Array::FetchObject(size_t key) {
    if (!Contains(key)) {
        return false;
    }
    return GetLast().is_object();
}

float JSON::Array::GetFloat() { return GetLast(); }

bool JSON::Array::GetBool() { return GetLast(); }

int JSON::Array::GetInt() { return GetLast(); }

std::string JSON::Array::GetString() { return GetLast(); }

JSON::Object JSON::Array::GetObject() { return Object(GetLast()); }

JSON::Array JSON::Array::GetArray() { return Array(GetLast()); }
