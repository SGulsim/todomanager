#include "../include/db.h"
#include <sqlite3.h>
#include <sstream>
#include <ctime>
#include <iomanip>

std::string Database::db_path_ = "";

static std::string getCurrentTimestamp() {
    auto now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&now), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

bool Database::initDatabase(const std::string& dbPath) {
    db_path_ = dbPath;
    sqlite3* db;
    
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        return false;
    }
    
    const char* createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            created_at TEXT NOT NULL
        )
    )";
    
    if (sqlite3_exec(db, createUsersTable, nullptr, nullptr, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }
    
    const char* createTasksTable = R"(
        CREATE TABLE IF NOT EXISTS tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            title TEXT NOT NULL,
            description TEXT,
            due_date TEXT,
            priority TEXT NOT NULL,
            status TEXT NOT NULL DEFAULT 'pending',
            created_at TEXT NOT NULL,
            updated_at TEXT NOT NULL,
            FOREIGN KEY (user_id) REFERENCES users(id)
        )
    )";
    
    if (sqlite3_exec(db, createTasksTable, nullptr, nullptr, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }
    
    sqlite3_close(db);
    return true;
}

void Database::closeDatabase() {
}

bool Database::createUser(const std::string& username, const std::string& password_hash) {
    sqlite3* db;
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO users (username, password_hash, created_at) VALUES (?, ?, ?)";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }
    
    std::string timestamp = getCurrentTimestamp();
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password_hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, timestamp.c_str(), -1, SQLITE_STATIC);
    
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return success;
}

User Database::getUserByUsername(const std::string& username) {
    User user;
    sqlite3* db;
    
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        return user;
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, username, password_hash, created_at FROM users WHERE username = ?";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return user;
    }
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user.id = sqlite3_column_int(stmt, 0);
        user.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        user.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        user.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return user;
}

User Database::getUserById(int id) {
    User user;
    sqlite3* db;
    
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        return user;
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, username, password_hash, created_at FROM users WHERE id = ?";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return user;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user.id = sqlite3_column_int(stmt, 0);
        user.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        user.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        user.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return user;
}

int Database::createTask(const Task& task) {
    sqlite3* db;
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        return -1;
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO tasks (user_id, title, description, due_date, priority, status, created_at, updated_at) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }
    
    std::string timestamp = getCurrentTimestamp();
    std::string created_at = task.created_at.empty() ? timestamp : task.created_at;
    sqlite3_bind_int(stmt, 1, task.user_id);
    sqlite3_bind_text(stmt, 2, task.title.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, task.description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, task.due_date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, task.priority.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, task.status.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, created_at.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, timestamp.c_str(), -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return -1;
    }
    
    int task_id = static_cast<int>(sqlite3_last_insert_rowid(db));
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return task_id;
}

std::vector<Task> Database::getTasksByUserId(int user_id) {
    std::vector<Task> tasks;
    sqlite3* db;
    
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        return tasks;
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, user_id, title, description, due_date, priority, status, created_at, updated_at FROM tasks WHERE user_id = ? ORDER BY created_at DESC";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return tasks;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Task task;
        task.id = sqlite3_column_int(stmt, 0);
        task.user_id = sqlite3_column_int(stmt, 1);
        task.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        task.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        task.due_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        task.priority = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        task.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        task.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        task.updated_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        tasks.push_back(task);
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return tasks;
}

Task Database::getTaskById(int task_id) {
    Task task;
    sqlite3* db;
    
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        return task;
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, user_id, title, description, due_date, priority, status, created_at, updated_at FROM tasks WHERE id = ?";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return task;
    }
    
    sqlite3_bind_int(stmt, 1, task_id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        task.id = sqlite3_column_int(stmt, 0);
        task.user_id = sqlite3_column_int(stmt, 1);
        task.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        task.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        task.due_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        task.priority = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        task.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        task.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        task.updated_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return task;
}

bool Database::updateTask(const Task& task) {
    sqlite3* db;
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE tasks SET title = ?, description = ?, due_date = ?, priority = ?, status = ?, updated_at = ? WHERE id = ? AND user_id = ?";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }
    
    std::string timestamp = getCurrentTimestamp();
    sqlite3_bind_text(stmt, 1, task.title.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, task.description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, task.due_date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, task.priority.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, task.status.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, timestamp.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, task.id);
    sqlite3_bind_int(stmt, 8, task.user_id);
    
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return success;
}

bool Database::deleteTask(int task_id) {
    sqlite3* db;
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        return false;
    }
    
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM tasks WHERE id = ?";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, task_id);
    
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return success;
}

