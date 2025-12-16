#ifndef DB_H
#define DB_H

#include <string>
#include <vector>
#include "task.h"
#include "user.h"

class Database {
public:
    static bool initDatabase(const std::string& dbPath);
    static void closeDatabase();
    
    static bool createUser(const std::string& username, const std::string& password_hash);
    static User getUserByUsername(const std::string& username);
    static User getUserById(int id);
    
    static int createTask(const Task& task);
    static std::vector<Task> getTasksByUserId(int user_id);
    static Task getTaskById(int task_id);
    static bool updateTask(const Task& task);
    static bool deleteTask(int task_id);
    
private:
    static std::string db_path_;
};

#endif

