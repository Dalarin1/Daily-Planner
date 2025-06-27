#pragma once
#include <map>
#include <functional>
#include <set>
#include "Task.hpp"

using days = std::chrono::duration<int, std::ratio<3600 * 24>>;
using weeks = std::chrono::duration<int, std::ratio<3600 * 24 * 7>>;

class Calendar
{
public:
    enum class ViewMode
    {
        Day,
        Week,
        Month
    };

    Calendar() : _current_date(std::chrono::system_clock::now()) {}

    
    void add_task(const Task &task);
    bool remove_task(int task_id);
    Task *get_task(int task_id);
    void update_task(int task_id, const Task &updated_task);

    void set_view_mode(ViewMode mode);
    void navigate_to_date(const std::chrono::system_clock::time_point &date);
    void go_to_today();

    
    std::vector<Task *> get_tasks_for_day(
        const std::chrono::system_clock::time_point &day) const;

    std::vector<Task *> get_tasks_for_week(
        const std::chrono::system_clock::time_point &week_start) const;

    std::vector<Task *> get_tasks_for_month(
        const std::chrono::system_clock::time_point &month_start) const;

    
    void check_and_trigger_reminders(
        const std::function<void(const Task &)> &reminder_callback);

    
    std::vector<std::string> get_all_categories() const;
    void set_category_filter(const std::string &category);
    void clear_category_filter();

private:
    std::map<int, Task> _tasks;
    ViewMode _current_view = ViewMode::Week;
    std::chrono::system_clock::time_point _current_date;
    std::string _active_category_filter;

    bool is_task_active_on_date(
        const Task &task,
        const std::chrono::system_clock::time_point &date) const;

    bool is_recurring_task_active(
        const Task &task,
        const std::chrono::system_clock::time_point &date) const;
};


void Calendar::add_task(const Task &task)
{
    _tasks[task.get_id()] = task;
}

bool Calendar::remove_task(int task_id)
{
    return _tasks.erase(task_id) > 0;
}

Task *Calendar::get_task(int task_id)
{
    auto it = _tasks.find(task_id);
    return (it != _tasks.end()) ? &it->second : nullptr;
}

void Calendar::update_task(int task_id, const Task &updated_task)
{
    if (auto it = _tasks.find(task_id); it != _tasks.end())
    {
        it->second = updated_task;
    }
}

void Calendar::set_view_mode(ViewMode mode) { _current_view = mode; }
void Calendar::navigate_to_date(const std::chrono::system_clock::time_point &date) { _current_date = date; }
void Calendar::go_to_today() { _current_date = std::chrono::system_clock::now(); }

std::vector<Task *> Calendar::get_tasks_for_day(
    const std::chrono::system_clock::time_point &day) const
{
    std::vector<Task *> out;
    for (const auto &[id, task] : _tasks)
    {
        if (is_task_active_on_date(task, day) &&
            task.get_status() != Task::Status::Done &&
            (_active_category_filter.empty() || task.get_category() == _active_category_filter))
        {
            out.push_back(const_cast<Task *>(&task));
        }
    }
    return out;
}

std::vector<Task *> Calendar::get_tasks_for_week(
    const std::chrono::system_clock::time_point &week_start) const
{
    std::vector<Task *> out;
    const auto week_end = week_start + weeks{1};

    for (const auto &[id, task] : _tasks)
    {
        if (task.get_until_date() < week_end &&
            task.get_until_date() >= week_start &&
            task.get_status() != Task::Status::Done &&
            (_active_category_filter.empty() || task.get_category() == _active_category_filter))
        {
            out.push_back(const_cast<Task *>(&task));
        }
    }
    return out;
}

std::vector<Task *> Calendar::get_tasks_for_month(
    const std::chrono::system_clock::time_point &month_start) const
{
    std::vector<Task *> out;
    const auto month_end = month_start + days{31}; // Упрощенно

    for (const auto &[id, task] : _tasks)
    {
        if (task.get_until_date() < month_end &&
            task.get_until_date() >= month_start &&
            task.get_status() != Task::Status::Done &&
            (_active_category_filter.empty() || task.get_category() == _active_category_filter))
        {
            out.push_back(const_cast<Task *>(&task));
        }
    }
    return out;
}

void Calendar::check_and_trigger_reminders(
    const std::function<void(const Task &)> &reminder_callback)
{
    for (const auto &[id, task] : _tasks)
    {
        if (task.is_due_soon() && task.get_status() != Task::Status::Done)
        {
            reminder_callback(task);
        }
    }
}

std::vector<std::string> Calendar::get_all_categories() const
{
    std::set<std::string> unique_categories;
    for (const auto &[id, task] : _tasks)
    {
        if (!task.get_category().empty())
        {
            unique_categories.insert(task.get_category());
        }
    }
    return {unique_categories.begin(), unique_categories.end()};
}

void Calendar::set_category_filter(const std::string &category)
{
    _active_category_filter = category;
}

void Calendar::clear_category_filter()
{
    _active_category_filter.clear();
}

bool Calendar::is_task_active_on_date(
    const Task &task,
    const std::chrono::system_clock::time_point &date) const
{
    return false;
}

bool Calendar::is_recurring_task_active(
    const Task &task,
    const std::chrono::system_clock::time_point &date) const
{
    return false;
}