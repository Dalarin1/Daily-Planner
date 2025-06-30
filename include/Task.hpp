#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <date/date.h>

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
         const date::year_month_day &due_date = std::chrono::floor<date::days>(std::chrono::system_clock::now()),
         const date::hh_mm_ss<std::chrono::minutes> &due_time = date::hh_mm_ss<std::chrono::minutes>{},      // Время
         const date::hh_mm_ss<std::chrono::minutes> &reminder_time = date::hh_mm_ss<std::chrono::minutes>{}, // Время напоминания
         std::chrono::hours repeat_interval = std::chrono::hours{0});                                        // Период повтора в часах

    // Обновление задачи
    void update(const std::string &new_title = "New task",
                const std::string &new_description = "",
                const std::string &new_category = "",
                Priority new_priority = Priority::Medium,
                const date::year_month_day &new_due_date = date::year_month_day{},
                const date::hh_mm_ss<std::chrono::minutes> &new_due_time = date::hh_mm_ss<std::chrono::minutes>{},
                const date::hh_mm_ss<std::chrono::minutes> &new_reminder_time = date::hh_mm_ss<std::chrono::minutes>{},
                std::chrono::hours new_repeat_interval = std::chrono::hours{0});

    bool is_due_soon() const;
    bool is_due_today() const;

    date::sys_time<std::chrono::milliseconds> get_full_due_time() const;
    std::string get_due_time_string() const;

    int get_id() const;
    std::string get_title() const;
    std::string get_description() const;
    std::string get_category() const;
    Priority get_priority() const;
    Status get_status() const;
    bool is_repeating() const;
    std::chrono::hours get_repeat_interval() const;
    date::year_month_day get_due_date() const;
    date::hh_mm_ss<std::chrono::minutes> get_due_time() const;
    date::hh_mm_ss<std::chrono::minutes> get_reminder_time() const;

    void set_title(const std::string &new_title);
    void set_description(const std::string &new_description);
    void set_category(const std::string &new_category);
    void set_priority(Priority new_priority);
    void set_status(Status new_status);
    void set_repeating(bool should_repeat);
    void set_repeat_interval(std::chrono::hours new_repeat_interval);
    void set_due_date(const date::year_month_day &new_due_date);
    void set_due_time(const date::hh_mm_ss<std::chrono::minutes> &new_due_time);
    void set_reminder_time(const date::hh_mm_ss<std::chrono::minutes> &new_reminder_time);

private:
    static int last_id;
    int _id;
    std::string _title;
    std::string _description;
    std::string _category;
    date::year_month_day _due_date;                      // Дата выполнения
    date::hh_mm_ss<std::chrono::minutes> _due_time;      // Время выполнения
    date::hh_mm_ss<std::chrono::minutes> _reminder_time; // Время напоминания
    Priority _priority;
    Status _status;
    bool _is_repeating;
    std::chrono::hours _repeat_interval; // Интервал повтора
};

#include <sstream>
#include <iomanip>

int Task::last_id = 0;

Task::Task(const std::string &title,
           const std::string &description,
           const std::string &category,
           Priority priority,
           const date::year_month_day &due_date,
           const date::hh_mm_ss<std::chrono::minutes> &due_time,
           const date::hh_mm_ss<std::chrono::minutes> &reminder_time,
           std::chrono::hours repeat_interval)
    : _title(title),
      _description(description),
      _category(category),
      _due_date(due_date),
      _due_time(due_time),
      _reminder_time(reminder_time),
      _priority(priority),
      _status(Status::Undone),
      _is_repeating(repeat_interval > std::chrono::hours{0}),
      _repeat_interval(repeat_interval),
      _id(last_id++)
{
}

void Task::update(const std::string &new_title,
                  const std::string &new_description,
                  const std::string &new_category,
                  Priority new_priority,
                  const date::year_month_day &new_due_date,
                  const date::hh_mm_ss<std::chrono::minutes> &new_due_time,
                  const date::hh_mm_ss<std::chrono::minutes> &new_reminder_time,
                  std::chrono::hours new_repeat_interval)
{
    _title = new_title;
    _description = new_description;
    _category = new_category;
    _priority = new_priority;
    _due_date = new_due_date;
    _due_time = new_due_time;
    _reminder_time = new_reminder_time;
    _repeat_interval = new_repeat_interval;
    _is_repeating = new_repeat_interval > std::chrono::hours{0};
}

bool Task::is_due_soon() const
{
    auto now = std::chrono::system_clock::now();
    auto due_time_point = get_full_due_time();
    return (due_time_point - now) < std::chrono::hours{24};
}

bool Task::is_due_today() const
{
    auto today = date::year_month_day{floor<date::days>(std::chrono::system_clock::now())};
    return _due_date == today;
}

date::sys_time<std::chrono::milliseconds> Task::get_full_due_time() const
{
    auto due_sys_days = date::sys_days(_due_date);
    auto due_time_point = std::chrono::time_point_cast<std::chrono::milliseconds>(due_sys_days + _due_time.to_duration());
    return due_time_point;
}

std::string Task::get_due_time_string() const
{
    std::ostringstream oss;
    oss << _due_date << " "
        << std::setfill('0') << std::setw(2) << _due_time.hours().count() << ":"
        << std::setw(2) << _due_time.minutes().count();
    return oss.str();
}

int Task::get_id() const { return _id; }
std::string Task::get_title() const { return _title; }
std::string Task::get_description() const { return _description; }
std::string Task::get_category() const { return _category; }
Task::Priority Task::get_priority() const { return _priority; }
Task::Status Task::get_status() const { return _status; }
bool Task::is_repeating() const { return _is_repeating; }
std::chrono::hours Task::get_repeat_interval() const { return _repeat_interval; }
date::year_month_day Task::get_due_date() const { return _due_date; }
date::hh_mm_ss<std::chrono::minutes> Task::get_due_time() const { return _due_time; }
date::hh_mm_ss<std::chrono::minutes> Task::get_reminder_time() const { return _reminder_time; }

void Task::set_title(const std::string &new_title) { _title = new_title; }
void Task::set_description(const std::string &new_description) { _description = new_description; }
void Task::set_category(const std::string &new_category) { _category = new_category; }
void Task::set_priority(Priority new_priority) { _priority = new_priority; }
void Task::set_status(Status new_status) { _status = new_status; }
void Task::set_repeating(bool should_repeat) { _is_repeating = should_repeat; }
void Task::set_repeat_interval(std::chrono::hours new_repeat_interval)
{
    _repeat_interval = new_repeat_interval;
    _is_repeating = new_repeat_interval > std::chrono::hours{0};
}
void Task::set_due_date(const date::year_month_day &new_due_date) { _due_date = new_due_date; }
void Task::set_due_time(const date::hh_mm_ss<std::chrono::minutes> &new_due_time) { _due_time = new_due_time; }
void Task::set_reminder_time(const date::hh_mm_ss<std::chrono::minutes> &new_reminder_time) { _reminder_time = new_reminder_time; }