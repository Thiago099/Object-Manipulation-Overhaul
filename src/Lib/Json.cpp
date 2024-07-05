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



