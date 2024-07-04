#include "Lib/Json.h"


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

Json::Object Json::ObjectFromFile(std::string path) {
    JObject j;
    if (ReadFile(j, path)) {
        if (j.is_object()) {
            return Object(j);
        }
    }
    return Object(JObject::parse("{}"));
}
Json::Object Json::ObjectFromString(std::string data) {
    return Object(ReadText(data));
}
Json::Array Json::ArrayFromFile(std::string path) {
    JObject j;
    if (ReadFile(j, path)) {
        if (j.is_array()) {
            return Array(j);
        }
    }
    return Array(JObject::parse("[]"));
}
Json::Array Json::ArrayFromText(std::string data) {
    return Array(ReadText(data)); }

bool Json::Object::HasFloat(std::string key) {
    if (!obj.contains(key)) {
        return false;
    }
    return obj[key].is_number_float();
}

bool Json::Object::HasBool(std::string key) {
    if (!obj.contains(key)) {
        return false;
    }
    return obj[key].is_boolean();
}

bool Json::Object::HasInt(std::string key) {
    if (!obj.contains(key)) {
        return false;
    }
    return obj[key].is_boolean();
}

bool Json::Object::HasString(std::string key) {
    if (!obj.contains(key)) {
        return false;
    }
    return obj[key].is_string();
}

bool Json::Object::HasArray(std::string key) {
    if (!obj.contains(key)) {
        return false;
    }
    return obj[key].is_array();
}

bool Json::Object::HasObject(std::string key) {
    if (!obj.contains(key)) {
        return false;
    }
    return obj[key].is_object();
}

float Json::Object::GetFloat(std::string key) { return obj[key]; }

bool Json::Object::GetBool(std::string key) { return obj[key]; }

int Json::Object::GetInt(std::string key) { return obj[key]; }

std::string Json::Object::GetString(std::string key) { return obj[key]; }

Json::Object Json::Object::GetObject(std::string key) { return Object(obj[key]); }

Json::Array Json::Object::GetArray(std::string key) { return Array(obj[key]); }

bool Json::Array::HasFloat(size_t key) {
    if (!HasKey(key)) {
        return false;
    }
    return obj[key].is_number_float();
}

bool Json::Array::HasBool(size_t key) {
    if (!HasKey(key)) {
        return false;
    }
    return obj[key].is_boolean();
}

bool Json::Array::HasInt(size_t key) {
    if (!HasKey(key)) {
        return false;
    }
    return obj[key].is_boolean();
}

bool Json::Array::HasString(size_t key) {
    if (!HasKey(key)) {
        return false;
    }
    return obj[key].is_string();
}

bool Json::Array::HasArray(size_t key) {
    if (!HasKey(key)) {
        return false;
    }
    return obj[key].is_array();
}

bool Json::Array::HasObject(size_t key) {
    if (!HasKey(key)) {
        return false;
    }
    return obj[key].is_object();
}

float Json::Array::GetFloat(size_t key) { return obj[key]; }

bool Json::Array::GetBool(size_t key) { return obj[key]; }

int Json::Array::GetInt(size_t key) { return obj[key]; }

std::string Json::Array::GetString(size_t key) { return obj[key]; }

Json::Object Json::Array::GetObject(size_t key) { return Object(obj[key]); }

Json::Array Json::Array::GetArray(size_t key) { return Array(obj[key]); }
