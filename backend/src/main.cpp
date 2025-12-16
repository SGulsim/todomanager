#include "../include/routes.h"
#include "../include/db.h"
#include <iostream>
#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define access _access
#define mkdir _mkdir
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

std::string getEnvVar(const std::string& key, const std::string& defaultValue) {
    const char* val = std::getenv(key.c_str());
    return val ? std::string(val) : defaultValue;
}

int getEnvInt(const std::string& key, int defaultValue) {
    const char* val = std::getenv(key.c_str());
    if (val) {
        try {
            return std::stoi(val);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

int main() {
    std::string dbPath = getEnvVar("DB_PATH", "./data/tasks.db");
    std::string dataDir = dbPath.substr(0, dbPath.find_last_of("/\\"));
    if (dataDir.empty()) {
        dataDir = "./data";
        dbPath = "./data/tasks.db";
    }
    
    #ifdef _WIN32
        if (_access(dataDir.c_str(), 0) != 0) {
            _mkdir(dataDir.c_str());
        }
    #else
        if (access(dataDir.c_str(), F_OK) != 0) {
            mkdir(dataDir.c_str(), 0755);
        }
    #endif
    
    if (!Database::initDatabase(dbPath)) {
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }
    
    std::cout << "Database initialized successfully at: " << dbPath << std::endl;
    
    httplib::Server server;
    setupRoutes(server);
    
    int port = getEnvInt("PORT", 8080);
    std::string host = getEnvVar("HOST", "0.0.0.0");
    
    std::cout << "Starting server on http://" << host << ":" << port << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;
    
    if (!server.listen(host.c_str(), port)) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    
    return 0;
}

