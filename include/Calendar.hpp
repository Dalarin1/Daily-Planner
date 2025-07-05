#pragma once
#include <map>
#include <functional>
#include <set>
#include "Task.hpp"

class Calendar
{
public:
    enum class ViewMode
    {
        Day,
        Week,
        Month
    };

    Calendar() : _current_date(std::chrono::floor<date::days>(std::chrono::system_clock::now())) {}

    void add_task(const Task &task);
    bool remove_task(int task_id);
    Task *get_task_by_id(int task_id);
    void update_task(int task_id, const Task &updated_task);

    void set_view_mode(ViewMode mode);
    ViewMode get_view_mode() const;
    void navigate_to_date(const date::year_month_day &date);
    void go_to_today();

    date::year_month_day get_current_date() const;
    std::wstring get_active_filter() const;

    std::vector<Task *> get_all_tasks() const;
    std::vector<Task *> get_tasks_for_day(const date::year_month_day &day) const;
    std::vector<Task *> get_tasks_for_week(const date::year_month_day &week_start) const;
    std::vector<Task *> get_tasks_for_month(const date::year_month_day &month_start) const;

    void check_and_trigger_reminders(
        const std::function<void(const Task &)> &reminder_callback);

    std::set<std::wstring> get_all_categories() const;
    void set_category_filter(const std::wstring &category);
    void clear_category_filter();

private:
    std::map<int, Task> _tasks;
    ViewMode _current_view = ViewMode::Month;
    date::year_month_day _current_date;
    std::wstring _active_category_filter;

    bool is_task_active_on_date(const Task &task, const date::year_month_day &date) const;
    bool is_recurring_task_active(const Task &task, const date::year_month_day &date) const;
};

void Calendar::add_task(const Task &task) { _tasks[task.get_id()] = task; }
bool Calendar::remove_task(int task_id) { return _tasks.erase(task_id) > 0; }
Task *Calendar::get_task_by_id(int task_id) { return &_tasks[task_id]; }

void Calendar::update_task(int task_id, const Task &updated_task)
{
    if (auto it = _tasks.find(task_id); it != _tasks.end())
    {
        it->second = updated_task;
    }
}

void Calendar::set_view_mode(ViewMode mode) { _current_view = mode; }
Calendar::ViewMode Calendar::get_view_mode() const { return this->_current_view; }
void Calendar::navigate_to_date(const date::year_month_day &date) { this->_current_date = date; }
void Calendar::go_to_today() { this->_current_date = std::chrono::floor<date::days>(std::chrono::system_clock::now()); }

date::year_month_day Calendar::get_current_date() const { return _current_date; } 
std::wstring Calendar::get_active_filter() const { return this->_active_category_filter; }

std::vector<Task *> Calendar::get_all_tasks() const {
    std::vector<Task* > out = {};
    for (const auto &[id, task] : _tasks)
    {
       out.push_back(&const_cast<Task &>(task));
    }
    return out;
}
std::vector<Task *> Calendar::get_tasks_for_day(const date::year_month_day &day) const
{
    std::vector<Task *> out = {};
    for (const auto &[id, task] : _tasks)
    {
        if (task.get_due_date() == day &&
            (_active_category_filter.empty() || task.get_category() == _active_category_filter))
        {
            out.push_back(&const_cast<Task &>(task));
        }
    }
    return out;
}
std::vector<Task *> Calendar::get_tasks_for_week(const date::year_month_day &week_start) const
{
    std::vector<Task *> out = {};
    auto start_sys = date::sys_days(week_start);
    auto end_sys = start_sys + date::weeks{1};

    for (const auto &[id, task] : _tasks)
    {
        if (task.get_due_date() >= start_sys && task.get_due_date() <= end_sys)
        {
            out.push_back(&const_cast<Task &>(task));
        }
    }
    return out;
}
std::vector<Task *> Calendar::get_tasks_for_month(const date::year_month_day &month_start) const
{
    std::vector<Task *> out = {};
    auto start_sys = date::sys_days(month_start);
    auto end_sys = start_sys + date::months{1};

    for (const auto &[id, task] : _tasks)
    {
        auto task_due_sys = date::sys_days(task.get_due_date());
        if (task_due_sys >= start_sys && task_due_sys < end_sys &&
            (_active_category_filter.empty() || task.get_category() == _active_category_filter))
        {
            out.push_back(&const_cast<Task &>(task));
        }
    }
    return out;
}

void Calendar::check_and_trigger_reminders(
    const std::function<void(const Task &)> &reminder_callback)
{
    for (const auto &[id, task] : _tasks)
    {
        if (task.is_due_soon())
        {
            reminder_callback(task);
        }
    }
}

std::set<std::wstring> Calendar::get_all_categories() const
{
    std::set<std::wstring> out = {};
    for (const auto &[id, task] : _tasks)
    {
        out.insert(task.get_category());
    }
    return out;
}
void Calendar::set_category_filter(const std::wstring &category) { this->_active_category_filter = category; }
void Calendar::clear_category_filter() { this->_active_category_filter = L""; }