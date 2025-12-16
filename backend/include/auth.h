#ifndef AUTH_H
#define AUTH_H

#include <string>

class Auth {
public:
    static std::string hashPassword(const std::string& password);
    static bool verifyPassword(const std::string& password, const std::string& hash);
    
    static std::string generateToken(int user_id, const std::string& username);
    static bool verifyToken(const std::string& token, int& user_id, std::string& username);
    
    static std::string extractTokenFromHeader(const std::string& auth_header);
};

#endif

