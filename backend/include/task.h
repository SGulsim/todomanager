#ifndef TASK_H
#define TASK_H

#include <ctime>
#include <string>

class Task {
public:
    int id;
    int user_id;
    std::string title;
    std::string description;
    std::string due_date;
    std::string priority;
    std::string status;
    std::string created_at;
    std::string updated_at;

    Task();
    Task(int user_id,
         const std::string& title,
         const std::string& description,
         const std::string& due_date,
         const std::string& priority);

    std::string toJson() const;
    bool isValid() const;
    void refreshStatus(bool keepCompleted = true);
    static Task fromJson(const std::string& json);

private:
    static std::string currentTimestamp();
    static std::string normalizePriority(const std::string& value);
    static std::string normalizeStatus(const std::string& value);
    static bool parseDate(const std::string& dateString, std::tm& outTm);
    static std::time_t toUtcTimestamp(std::tm tm);
};

#endif // TASK_H