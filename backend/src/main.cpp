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

int main() {
    std::string dbPath = "../data/tasks.db";
    std::string dataDir = "../data";
    
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
    
    std::cout << "Database initialized successfully" << std::endl;
    
    httplib::Server server;
    setupRoutes(server);
    
    std::cout << "Starting server on http://localhost:8080" << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;
    
    if (!server.listen("0.0.0.0", 8080)) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    
    return 0;
}

