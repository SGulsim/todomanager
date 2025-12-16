#include "../include/routes.h"
#include "../include/db.h"
#include "../include/auth.h"
#include "../include/task.h"
#include "../include/user.h"
#include <sstream>
#include <regex>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>

std::map<std::string, std::string> parseSimpleJson(const std::string& json) {
    std::map<std::string, std::string> result;
    
    // Парсим JSON без удаления пробелов из строковых значений
    // Удаляем только пробелы между ключами и значениями, но не внутри кавычек
    std::string cleaned = json;
    
    // Удаляем пробелы только вне кавычек
    bool inQuotes = false;
    bool escaped = false;
    std::string processed;
    for (size_t i = 0; i < cleaned.length(); ++i) {
        char c = cleaned[i];
        if (escaped) {
            processed += c;
            escaped = false;
            continue;
        }
        if (c == '\\') {
            escaped = true;
            processed += c;
            continue;
        }
        if (c == '"') {
            inQuotes = !inQuotes;
            processed += c;
            continue;
        }
        if (inQuotes) {
            // Внутри кавычек - сохраняем все символы, включая пробелы
            processed += c;
        } else {
            // Вне кавычек - удаляем пробелы, табы, переносы строк
            if (!std::isspace(c)) {
                processed += c;
            }
        }
    }
    
    std::regex stringPattern("\"([^\"]+)\":\"([^\"]*)\"");
    std::sregex_iterator iter(processed.begin(), processed.end(), stringPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        if (match.size() == 3) {
            result[match[1].str()] = match[2].str();
        }
    }
    
    std::regex numPattern("\"([^\"]+)\":(\\d+)");
    std::sregex_iterator numIter(processed.begin(), processed.end(), numPattern);
    for (; numIter != end; ++numIter) {
        std::smatch match = *numIter;
        if (match.size() == 3) {
            result[match[1].str()] = match[2].str();
        }
    }
    
    return result;
}

int getUserIdFromRequest(const httplib::Request& req) {
    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.empty()) {
        return -1;
    }
    
    std::string token = Auth::extractTokenFromHeader(authHeader);
    int user_id;
    std::string username;
    
    if (!Auth::verifyToken(token, user_id, username)) {
        return -1;
    }
    
    return user_id;
}

void setupRoutes(httplib::Server& server) {
    // CORS middleware функция - проверяет, не установлены ли заголовки уже
    auto addCorsHeaders = [](httplib::Response& res) {
        // Проверяем и устанавливаем заголовки только если их еще нет
        if (!res.has_header("Access-Control-Allow-Origin")) {
            res.set_header("Access-Control-Allow-Origin", "*");
        }
        if (!res.has_header("Access-Control-Allow-Methods")) {
            res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        }
        if (!res.has_header("Access-Control-Allow-Headers")) {
            res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        }
    };
    
    // Обработка preflight запросов
    server.Options(".*", [addCorsHeaders](const httplib::Request&, httplib::Response& res) {
        addCorsHeaders(res);
        res.status = 200;
    });
    
    // Добавляем CORS заголовки ко всем ответам через post-routing handler
    server.set_post_routing_handler([addCorsHeaders](const httplib::Request&, httplib::Response& res) {
        addCorsHeaders(res);
    });
    
    server.Post("/api/auth/register", [](const httplib::Request& req, httplib::Response& res) {
        auto body = req.body;
        auto json = parseSimpleJson(body);
        
        if (json.find("username") == json.end() || json.find("password") == json.end()) {
            res.status = 400;
            res.set_content("{\"error\":\"Username and password are required\"}", "application/json");
            return;
        }
        
        std::string username = json["username"];
        std::string password = json["password"];
        
        if (username.length() < 3 || password.length() < 3) {
            res.status = 400;
            res.set_content("{\"error\":\"Username and password must be at least 3 characters\"}", "application/json");
            return;
        }
        
        User existingUser = Database::getUserByUsername(username);
        if (existingUser.id != 0) {
            res.status = 409;
            res.set_content("{\"error\":\"Username already exists\"}", "application/json");
            return;
        }
        
        std::string password_hash = Auth::hashPassword(password);
        if (Database::createUser(username, password_hash)) {
            res.status = 201;
            res.set_content("{\"message\":\"User created successfully\"}", "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"error\":\"Failed to create user\"}", "application/json");
        }
    });
    
    server.Post("/api/auth/login", [](const httplib::Request& req, httplib::Response& res) {
        auto body = req.body;
        auto json = parseSimpleJson(body);
        
        if (json.find("username") == json.end() || json.find("password") == json.end()) {
            res.status = 400;
            res.set_content("{\"error\":\"Username and password are required\"}", "application/json");
            return;
        }
        
        std::string username = json["username"];
        std::string password = json["password"];
        
        User user = Database::getUserByUsername(username);
        if (user.id == 0) {
            res.status = 401;
            res.set_content("{\"error\":\"Invalid credentials\"}", "application/json");
            return;
        }
        
        if (!Auth::verifyPassword(password, user.password_hash)) {
            res.status = 401;
            res.set_content("{\"error\":\"Invalid credentials\"}", "application/json");
            return;
        }
        
        std::string token = Auth::generateToken(user.id, user.username);
        
        std::string escaped_username = username;
        size_t pos = 0;
        while ((pos = escaped_username.find('"', pos)) != std::string::npos) {
            escaped_username.replace(pos, 1, "\\\"");
            pos += 2;
        }
        
        std::ostringstream response;
        response << "{\"token\":\"" << token << "\",\"user_id\":" << user.id << ",\"username\":\"" << escaped_username << "\"}";
        res.set_content(response.str(), "application/json");
    });
    
    server.Get("/api/tasks", [](const httplib::Request& req, httplib::Response& res) {
        int user_id = getUserIdFromRequest(req);
        if (user_id == -1) {
            res.status = 401;
            res.set_content("{\"error\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        auto tasks = Database::getTasksByUserId(user_id);
        std::ostringstream json;
        json << "[";
        for (size_t i = 0; i < tasks.size(); i++) {
            json << tasks[i].toJson();
            if (i < tasks.size() - 1) json << ",";
        }
        json << "]";
        
        res.set_content(json.str(), "application/json");
    });
    
    server.Post("/api/tasks", [](const httplib::Request& req, httplib::Response& res) {
        int user_id = getUserIdFromRequest(req);
        if (user_id == -1) {
            res.status = 401;
            res.set_content("{\"error\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        auto body = req.body;
        auto json = parseSimpleJson(body);
        
        if (json.find("title") == json.end()) {
            res.status = 400;
            res.set_content("{\"error\":\"Title is required\"}", "application/json");
            return;
        }
        
        std::string title = json["title"];
        std::string description = json.find("description") != json.end() ? json["description"] : "";
        std::string due_date = json.find("due_date") != json.end() ? json["due_date"] : "";
        std::string priority = json.find("priority") != json.end() ? json["priority"] : "medium";
        std::string created_at = json.find("created_at") != json.end() ? json["created_at"] : "";
        
        if (priority != "high" && priority != "medium" && priority != "low") {
            priority = "medium";
        }
        
        Task task(user_id, title, description, due_date, priority);
        if (!task.isValid()) {
            res.status = 400;
            res.set_content("{\"error\":\"Invalid task data\"}", "application/json");
            return;
        }
        
        // Если указана дата создания, используем её, иначе будет установлена автоматически
        if (!created_at.empty()) {
            // Преобразуем дату в формат YYYY-MM-DD HH:MM:SS
            if (created_at.length() == 10) { // YYYY-MM-DD
                created_at += " 00:00:00";
            }
            task.created_at = created_at;
        }
        
        int task_id = Database::createTask(task);
        if (task_id > 0) {
            task.id = task_id;
            res.status = 201;
            res.set_content(task.toJson(), "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"error\":\"Failed to create task\"}", "application/json");
        }
    });
    
    server.Put("/api/tasks/(\\d+)", [](const httplib::Request& req, httplib::Response& res) {
        int user_id = getUserIdFromRequest(req);
        if (user_id == -1) {
            res.status = 401;
            res.set_content("{\"error\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        int task_id = std::stoi(req.matches[1]);
        Task existingTask = Database::getTaskById(task_id);
        
        if (existingTask.id == 0) {
            res.status = 404;
            res.set_content("{\"error\":\"Task not found\"}", "application/json");
            return;
        }
        
        if (existingTask.user_id != user_id) {
            res.status = 403;
            res.set_content("{\"error\":\"Forbidden\"}", "application/json");
            return;
        }
        
        auto body = req.body;
        auto json = parseSimpleJson(body);
        
        if (json.find("title") != json.end()) {
            existingTask.title = json["title"];
        }
        if (json.find("description") != json.end()) {
            existingTask.description = json["description"];
        }
        if (json.find("due_date") != json.end()) {
            existingTask.due_date = json["due_date"];
        }
        if (json.find("priority") != json.end()) {
            existingTask.priority = json["priority"];
        }
        if (json.find("status") != json.end()) {
            existingTask.status = json["status"];
        }
        
        if (!existingTask.isValid()) {
            res.status = 400;
            res.set_content("{\"error\":\"Invalid task data\"}", "application/json");
            return;
        }
        
        if (Database::updateTask(existingTask)) {
            existingTask = Database::getTaskById(task_id);
            res.set_content(existingTask.toJson(), "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"error\":\"Failed to update task\"}", "application/json");
        }
    });
    
    server.Delete("/api/tasks/(\\d+)", [](const httplib::Request& req, httplib::Response& res) {
        int user_id = getUserIdFromRequest(req);
        if (user_id == -1) {
            res.status = 401;
            res.set_content("{\"error\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        int task_id = std::stoi(req.matches[1]);
        Task existingTask = Database::getTaskById(task_id);
        
        if (existingTask.id == 0) {
            res.status = 404;
            res.set_content("{\"error\":\"Task not found\"}", "application/json");
            return;
        }
        
        if (existingTask.user_id != user_id) {
            res.status = 403;
            res.set_content("{\"error\":\"Forbidden\"}", "application/json");
            return;
        }
        
        if (Database::deleteTask(task_id)) {
            res.status = 200;
            res.set_content("{\"message\":\"Task deleted successfully\"}", "application/json");
        } else {
            res.status = 500;
            res.set_content("{\"error\":\"Failed to delete task\"}", "application/json");
        }
    });
}

