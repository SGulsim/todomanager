#include "../include/task.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <cctype>

Task::Task() : id(0), user_id(0), priority("medium"), status("pending") {
}

Task::Task(int user_id, const std::string& title, const std::string& description,
           const std::string& due_date, const std::string& priority)
    : id(0), user_id(user_id), title(title), description(description),
      due_date(due_date), priority(priority), status("pending") {
}

static std::string escapeJson(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') 
                        << static_cast<int>(c);
                } else {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}

std::string Task::toJson() const {
    std::ostringstream oss;
    oss << "{"
        << "\"id\":" << id << ","
        << "\"user_id\":" << user_id << ","
        << "\"title\":\"" << escapeJson(title) << "\","
        << "\"description\":\"" << escapeJson(description) << "\","
        << "\"due_date\":\"" << escapeJson(due_date) << "\","
        << "\"priority\":\"" << escapeJson(priority) << "\","
        << "\"status\":\"" << escapeJson(status) << "\","
        << "\"created_at\":\"" << escapeJson(created_at) << "\","
        << "\"updated_at\":\"" << escapeJson(updated_at) << "\""
        << "}";
    return oss.str();
}

bool Task::isValid() const {
    return !title.empty() && 
           (priority == "high" || priority == "medium" || priority == "low") &&
           (status == "pending" || status == "in_progress" || status == "completed");
}

Task Task::fromJson(const std::string& json) {
    Task task;
    return task;
}

