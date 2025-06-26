#pragma once
#include <map>
#include <functional>
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

    // Добавление задач
    void add_task(const Task &task);
    bool remove_task(const int &task_id);
    Task *get_task(const int &task_id);
    void update_task(const int &task_id, const Task &updated_task);

    // Навигация и представления
    void set_view_mode(ViewMode mode);
    void navigate_to_date(const std::chrono::system_clock::time_point &date);
    void go_to_today();

    // Получение задач с учетом повторений
    std::vector<Task *> get_tasks_for_day(
        const std::chrono::system_clock::time_point &day) const;

    std::vector<Task *> get_tasks_for_week(
        const std::chrono::system_clock::time_point &week_start) const;

    std::vector<Task *> get_tasks_for_month(
        const std::chrono::system_clock::time_point &month_start) const;

    // Управление напоминаниями
    void check_and_trigger_reminders(
        const std::function<void(const Task &)> &reminder_callback);

    // Категории и фильтры
    std::vector<std::string> get_all_categories() const;
    void set_category_filter(const std::string &category);
    void clear_category_filter();

private:
    // Хранилище задач с быстрым доступом по ID
    std::map<int, Task> _tasks;

    // Фильтры и состояние
    ViewMode _current_view = ViewMode::Week;
    std::chrono::system_clock::time_point _current_date;
    std::string _active_category_filter;

    // Внутренние методы для обработки повторений
    bool is_task_active_on_date(
        const Task &task,
        const std::chrono::system_clock::time_point &date) const;

    bool is_recurring_task_active(
        const Task &task,
        const std::chrono::system_clock::time_point &date) const;
};

void Calendar::add_task(const Task &task) { this->_tasks[task.get_id()] = task; }
bool Calendar::remove_task(const int &task_id) { this->_tasks.erase(task_id); }
Task *Calendar::get_task(const int &task_id) { return &(this->_tasks[task_id]); }
void Calendar::update_task(const int &task_id, const Task &updated_task) { this->_tasks[task_id] = updated_task; }

void Calendar::set_view_mode(ViewMode mode) { this->_current_view = mode; }
void Calendar::navigate_to_date(const std::chrono::system_clock::time_point &date) { this->_current_date = date; }
void Calendar::go_to_today() { this->_current_date = std::chrono::system_clock::now(); }

std::vector<Task *> Calendar::get_tasks_for_day(
    const std::chrono::system_clock::time_point &day) const
{
    std::vector<Task *> out = {};
    for (const auto &[id, task] : _tasks)
    {
        if (task.get_status() != Task::Status::Done && task.get_until_date() >= day)
        {
            out.push_back(const_cast<Task *>(&task));
        }
    }
    return out;
}

std::vector<Task *> Calendar::get_tasks_for_week(
    const std::chrono::system_clock::time_point &week_start) const
{

    std::vector<Task *> out = {};
    for (const auto &[id, task] : _tasks)
    {
        if (task.get_status() != Task::Status::Done && task.get_until_date() >= week_start)
        {
            out.push_back(const_cast<Task *>(&task));
        }
    }
    return out;
}

std::vector<Task *> Calendar::get_tasks_for_month(
    const std::chrono::system_clock::time_point &month_start) const
{
    std::vector<Task *> out = {};
    for (const auto &[id, task] : _tasks)
    {
        if (task.get_status() != Task::Status::Done && task.get_until_date() >= month_start)
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
        if (task.is_due_soon())
        {
            reminder_callback(task);
        }
    }
}

std::vector<std::string> Calendar::get_all_categories() const {}
void Calendar::set_category_filter(const std::string &category) {}
void Calendar::clear_category_filter() {}