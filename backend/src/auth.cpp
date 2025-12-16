#include "../include/auth.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <functional>
#include <iomanip>
#include <sstream>

std::string Auth::hashPassword(const std::string& password) {
    std::hash<std::string> hasher;
    size_t hash = hasher(password);
    
    std::ostringstream oss;
    oss << std::hex << hash;
    return oss.str();
}

bool Auth::verifyPassword(const std::string& password, const std::string& hash) {
    std::string computed_hash = hashPassword(password);
    return computed_hash == hash;
}

std::string Auth::generateToken(int user_id, const std::string& username) {
    std::ostringstream oss;
    oss << user_id << ":" << std::time(nullptr);
    return oss.str();
}

bool Auth::verifyToken(const std::string& token, int& user_id, std::string& username) {
    size_t pos = token.find(':');
    if (pos == std::string::npos) return false;
    
    try {
        user_id = std::stoi(token.substr(0, pos));
        username = "";
        return true;
    } catch (...) {
        return false;
    }
}

std::string Auth::extractTokenFromHeader(const std::string& auth_header) {
    if (auth_header.find("Bearer ") == 0) {
        return auth_header.substr(7);
    }
    return auth_header;
}

