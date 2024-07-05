#include "Lib/Json.h"



bool JSON::Object::Contains(std::string& key) {
    for (const auto item : obj.items()) {
        if (Misc::IsEqual(item.key(), key)) {
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
JSON::Array JSON::ArrayFromString(std::string data) {
    return Array(ReadText(data)); 
}

void JSON::Object::FetchFloat(
    std::string key, std::function<void(float)> success, std::function<void(std::string)> failure) {
    if (Contains(key)) {
        if (GetLast().is_number()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    } else {
        failure(NOT_FOUND);
    }
}
void JSON::Object::FetchBool(
    std::string key, std::function<void(bool)> success,  std::function<void(std::string)> failure) {
    if (Contains(key)) {
        if (GetLast().is_boolean()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    } else {
        failure(NOT_FOUND);
    }
}

void JSON::Object::FetchInt(
    std::string key, std::function<void(int)> success,  std::function<void(std::string)> failure) {
    if (Contains(key)) {
        if (GetLast().is_number()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    } else {
        failure(NOT_FOUND);
    }
}

void JSON::Object::FetchString(
    std::string key, std::function<void(std::string)> success,  std::function<void(std::string)> failure) {
    if (Contains(key)) {
        if (GetLast().is_string()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    } else {
        failure(NOT_FOUND);
    }
}

void JSON::Object::FetchObject(
    std::string key, std::function<void(Object)> success,  std::function<void(std::string)> failure) {
    if (Contains(key)) {
        if (GetLast().is_object()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    } else {
        failure(NOT_FOUND);
    }
}

void JSON::Object::FetchArray(
    std::string key, std::function<void(Array)> success, std::function<void(std::string)> failure) {
    if (Contains(key)) {
        if (GetLast().is_array()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    } else {
        failure(NOT_FOUND);
    }
}


JObject JSON::Array::GetLast(){ 
    return contextItem;
}

bool JSON::Array::GetNext() {
    auto exists = i < obj.size();
    if (exists) {
        contextItem = obj[i];
        i++;
    }
    return exists;
}


void JSON::Array::FetchFloat(std::function<void(float)> success, std::function<void(std::string)> failure) {
    while (GetNext()) {
        if (GetLast().is_number()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    }
}
void JSON::Array::FetchInt(std::function<void(int)> success, std::function<void(std::string)> failure) {
    while (GetNext()) {
        if (GetLast().is_number()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    }
}
void JSON::Array::FetchBool(std::function<void(bool)> success, std::function<void(std::string)> failure) {
    while (GetNext()) {
        if (GetLast().is_boolean()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    }
}
void JSON::Array::FetchString(std::function<void(std::string)> success, std::function<void(std::string)> failure) {
    while (GetNext()) {
        if (GetLast().is_string()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    }
}
void JSON::Array::FetchObject(std::function<void(Object)> success, std::function<void(std::string)> failure) {
    while (GetNext()) {
        if (GetLast().is_object()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    }
}
void JSON::Array::FetchArray(std::function<void(Array)> success, std::function<void(std::string)> failure) {
    while (GetNext()) {
        if (GetLast().is_array()) {
            success(GetLast());
        } else {
            failure(INVALID_TYPE);
        }
    }
}
