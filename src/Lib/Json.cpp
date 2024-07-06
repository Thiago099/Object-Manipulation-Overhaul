#include "Lib/Json.h"



bool JSON::Object::GetCaseInsensitive(std::string& key, JObject& outItem) {
    for (const auto item : obj.items()) {
        if (Misc::IsEqual(item.key(), key)) {
            outItem = item.value();
            return true;
        }
    }
    return false;
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

