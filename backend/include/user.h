#ifndef USER_H
#define USER_H

#include <string>

class User {
public:
    int id;
    std::string username;
    std::string password_hash;
    std::string created_at;

    User();
    User(const std::string& username, const std::string& password_hash);
    
    bool isValid() const;
};

#endif // USER_H

