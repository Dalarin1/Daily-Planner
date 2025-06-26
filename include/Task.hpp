#pragma once
#include <string>
#include <chrono>
#include <vector>

class Task
{
public:
    enum Status
    {
        Pending, // пер.: В процессе
        Done,
        Undone,
    };
    enum Priority
    {
        Low,
        Medium,
        High
    };
    Task(const std::string &title = "New task",
         const std::string &description = "",
         const std::string &category = "",
         Priority priority = Priority::Medium,
         const std::chrono::system_clock::time_point &start_date = std::chrono::system_clock::time_point(),
         const std::chrono::system_clock::time_point &until_date = std::chrono::system_clock::time_point(),
         const std::chrono::system_clock::time_point &reminder = std::chrono::system_clock::time_point());

    void update(const std::string &new_title = "New task",
                const std::string &new_description = "",
                const std::string &new_category = "",
                Priority new_priority = Priority::Medium,
                const std::chrono::system_clock::time_point &new_start_date = std::chrono::system_clock::time_point(),
                const std::chrono::system_clock::time_point &new_until_date = std::chrono::system_clock::time_point(),
                const std::chrono::system_clock::time_point &new_reminder = std::chrono::system_clock::time_point());

    bool is_due_soon() const;
    int get_id() const;
    std::string get_title() const;
    std::string get_description() const;
    std::string get_category() const;
    Priority get_priority() const;
    Status get_status() const;
    bool is_repeating() const;
    std::chrono::system_clock::period get_repeat_period() const;
    std::chrono::system_clock::time_point get_until_date() const;

    void set_title(const std::string &new_title);
    void set_description(const std::string &new_description);
    void set_category(const std::string &new_category);
    void set_priority(Task::Priority new_priority);
    void set_status(Status new_status);
    void set_repeating(bool should_repeat);
    void set_repeat_period(std::chrono::system_clock::period new_repeat_period);
    void set_until_date(std::chrono::system_clock::time_point new_until_date);

private:
    static int last_id;
    int _id;
    std::string _title;
    std::string _description;
    std::string _category;
    std::chrono::system_clock::time_point _start_date;
    std::chrono::system_clock::time_point _until_date;
    std::chrono::system_clock::time_point _reminder;
    Priority _priority;
    Status _status;
    bool _is_repeating;
    std::chrono::system_clock::period _repeat_period;
};

Task::Task(const std::string &title,
           const std::string &description,
           const std::string &category,
           Priority priority,
           const std::chrono::system_clock::time_point &start_date,
           const std::chrono::system_clock::time_point &until_date,
           const std::chrono::system_clock::time_point &reminder)
{
    _title = title;
    _description = description;
    _category = category;
    _priority = priority;
    _status = Status::Undone;
    _start_date = start_date;
    _until_date = until_date;
    _reminder = reminder;
    _is_repeating = false;
    _repeat_period = std::chrono::system_clock::period();
    _id = last_id;
    ++last_id;
}
void Task::update(const std::string &new_title,
                  const std::string &new_description,
                  const std::string &new_category,
                  Priority new_priority,
                  const std::chrono::system_clock::time_point &new_start_date,
                  const std::chrono::system_clock::time_point &new_until_date,
                  const std::chrono::system_clock::time_point &new_reminder)
{
    _title = new_title;
    _description = new_description;
    _category = new_category;
    _priority = new_priority;
    _start_date = new_start_date;
    _until_date = new_until_date;
    _reminder = new_reminder;
}

int Task::last_id = 0;
int Task::get_id() const { return this->_id; }
std::string Task::get_title() const { return this->_title; }
std::string Task::get_description() const { return this->_description; }
std::string Task::get_category() const { return this->_category; }
Task::Priority Task::get_priority() const { return this->_priority; }
Task::Status Task::get_status() const { return this->_status; }
bool Task::is_repeating() const { return this->_is_repeating; }
std::chrono::system_clock::period Task::get_repeat_period() const { return this->_repeat_period; }
std::chrono::system_clock::time_point Task::get_until_date() const { return this->_until_date; }

void Task::set_title(const std::string &new_title) { this->_title = new_title; }
void Task::set_description(const std::string &new_description) { this->_description = new_description; }
void Task::set_category(const std::string &new_category) { this->_category = new_category; }
void Task::set_priority(Task::Priority new_priority) { this->_priority = new_priority; }
void Task::set_status(Task::Status new_status) { this->_status = new_status; }
void Task::set_repeating(bool should_repeat) { this->_is_repeating = should_repeat; }
void Task::set_repeat_period(std::chrono::system_clock::period new_repeat_period) { this->_repeat_period = new_repeat_period; }
void Task::set_until_date(std::chrono::system_clock::time_point new_until_date) { this->_until_date = new_until_date; }

bool Task::is_due_soon() const {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto time_until_due = _until_date - now;
    return time_until_due < 24h; 
}