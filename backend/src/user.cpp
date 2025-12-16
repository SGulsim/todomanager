#include "../include/user.h"

User::User() : id(0) {
}

User::User(const std::string& username, const std::string& password_hash)
    : id(0), username(username), password_hash(password_hash) {
}

bool User::isValid() const {
    return !username.empty() && !password_hash.empty() && username.length() >= 3;
}

