#pragma once
#include <iostream>
#include "Calendar.hpp"
#include "raphics.hpp"

class DayModeUi
{
public:
    std::vector<IUIElement *> _elements;
    DayModeUi()
    {
    }
};

class UIManager
{
public:
    UIManager(ShaderProgram *ui_program, ShaderProgram *text_program, TextRenderer *_text_renderer);
    void draw_calendar() const;

    void draw_calendar_month_mode() const;
    void draw_calendar_week_mode() const;
    void draw_calendar_day_mode() const;

    void update_tasks();
    void update(double mouseX, double mouseY, int windowWidth, int windowHeight);
    void update_geometry(int new_window_width, int new_window_height);
    void handle_click(double mouseX, double mouseY, int windowWidth, int windowHeight);
    ~UIManager();

    ShaderProgram *ui_render_program;
    ShaderProgram *text_render_program;
    TextRenderer *_text_renderer;

    Calendar _calendar;
    GLFWwindow *_window;

    std::vector<Button *> _week_view_buttons;
    std::vector<Button *> _month_view_buttons;
    std::map<int, Checkbox *> _task_checkboxes;
    std::map<Calendar::ViewMode, Button *> _view_switch_buttons;
};

UIManager::UIManager(ShaderProgram *ui_program, ShaderProgram *text_program, TextRenderer *text_renderer)
{
    ui_render_program = ui_program;
    text_render_program = text_program;
    _text_renderer = text_renderer;
    _calendar = Calendar();
    _calendar.set_view_mode(Calendar::ViewMode::Month);
    _view_switch_buttons[Calendar::ViewMode::Day] = new Button(
        vector3(-0.5, 0.75, 0),
        vector3(0.25, 0.125, 0),
        Color(0, 0, 0), Color(153, 50, 204),
        [this]()
        { _calendar.set_view_mode(Calendar::ViewMode::Day); },
        [this]()
        {
            this->_view_switch_buttons[Calendar::ViewMode::Day]->current_bkg_color =
                this->_view_switch_buttons[Calendar::ViewMode::Day]->base_bkg_color.Lerp(Color(220, 208, 255), 25.0f);
        });

    _view_switch_buttons[Calendar::ViewMode::Week] = new Button(
        vector3(-0.25, 0.75, 0),
        vector3(0.25, 0.125, 0),
        Color(0, 0, 0), Color(153, 50, 204),
        [this]()
        { _calendar.set_view_mode(Calendar::ViewMode::Week); },
        [this]()
        {
            this->_view_switch_buttons[Calendar::ViewMode::Week]->current_bkg_color =
                this->_view_switch_buttons[Calendar::ViewMode::Week]->base_bkg_color.Lerp(Color(220, 208, 255), 25.0f);
        });

    _view_switch_buttons[Calendar::ViewMode::Month] = new Button(
        vector3(0, 0.75, 0),
        vector3(0.25, 0.125, 0),
        Color(0, 0, 0), Color(153, 50, 204),
        [this]()
        { _calendar.set_view_mode(Calendar::ViewMode::Month); },
        [this]()
        {
            this->_view_switch_buttons[Calendar::ViewMode::Month]->current_bkg_color =
                this->_view_switch_buttons[Calendar::ViewMode::Month]->base_bkg_color.Lerp(Color(220, 208, 255), 25.0f);
        });

    _task_checkboxes = {};
    auto curr_date = _calendar.get_current_date();
    auto week_start = date::sys_days(date::sys_days(curr_date) - (date::weekday(date::sys_days(curr_date)) - date::Monday));

    _week_view_buttons = {};
    for (int8_t i = 0; i < 7; i++)
    {
        _week_view_buttons.push_back(new Button(vector3(-0.75 + 0.25 * i, 0.5, 0), vector3(0.125, 0.8, 0), Color(), Color(0, 0, 0), [this, i, week_start]()
                                                {_calendar.navigate_to_date(week_start + date::days{i});_calendar.set_view_mode(Calendar::ViewMode::Day); }, nullptr));
    }

    _month_view_buttons = {};
    auto start_wkd = date::weekday(date::year_month_day(curr_date.year(), curr_date.month(), date::day{1}));
    auto days_count = (date::sys_days(date::year_month_day(date::year_month_day_last(curr_date.year(), date::month_day_last(curr_date.month())))) -
                       date::sys_days(date::year_month_day(curr_date.year(), curr_date.month(), date::day{1})))
                          .count() +
                      1;
    const float startX = 100.0f, startY = 600.0f;
    float currY = 0.5;
    for (int8_t i = 0; i < days_count; i++)
    {
        if ((i + 1) % 7 == 0)
        {
            currY -= 0.125;
        }
        _month_view_buttons.push_back(
            new Button(
                vector3(-0.75 + date::weekday{(unsigned int)((i + 1) % 7)}.c_encoding() * 0.125, currY, 0),
                vector3(0.1, 0.1, 0),
                Color(),
                Color(0, 0, 0),
                [this, i, curr_date]()
                {
                    _calendar.navigate_to_date(date::year_month_day(curr_date.year(), curr_date.month(), date::day{(unsigned int)(i + 1)}));
                    _calendar.set_view_mode(Calendar::ViewMode::Day);
                },
                nullptr));
    }
}
UIManager::~UIManager()
{
    delete ui_render_program;
    delete text_render_program;
    delete _text_renderer;

    for (auto &i : _week_view_buttons)
    {
        delete i;
    }
    for (auto &i : _month_view_buttons)
    {
        delete i;
    }
    for (auto &[_, cb] : _task_checkboxes)
    {
        delete cb;
    }
    for (auto &[_, btn] : _view_switch_buttons)
    {
        delete btn;
    }
}

void UIManager::draw_calendar() const
{
    ui_render_program->use();

    for (const auto &[mode, btn_ptr] : _view_switch_buttons)
    {
        btn_ptr->draw(ui_render_program->program);
    }
    switch (_calendar.get_view_mode())
    {
    case Calendar::ViewMode::Month:
        draw_calendar_month_mode();
        break;
    case Calendar::ViewMode::Week:
        draw_calendar_week_mode();
        break;
    case Calendar::ViewMode::Day:
        draw_calendar_day_mode();
        break;
    default:
        throw std::runtime_error("Unexpected calendar ViewMode");
    }
}
void UIManager::draw_calendar_day_mode() const
{
    // Текстовые координаты
    const float startPosX = 200.0f, startPosY = 560.0f;

    std::vector<Task *> tasks = _calendar.get_tasks_for_day(_calendar.get_current_date());
    ui_render_program->use();

    for (int i = 0; i < tasks.size(); i++)
    {
        _task_checkboxes.at(tasks[i]->get_id())->draw_at(ui_render_program->program, vector3(-0.5f, 0.6f - 0.1 * i, 0));
    }

    text_render_program->use();
    for (int i = 0; i < tasks.size(); i++)
    {
        _text_renderer->render_text_GL_coords("arial32", tasks[i]->get_title(), -0.5 + 0.05, 0.55f - 0.1 * i, 1.0f, Color(0, 0, 0), 800, 800);
    }
}
void UIManager::draw_calendar_week_mode() const
{
    const float cellW = 100.0f;
    const float startX = 100.0f, startY = 600.0f;

    const wchar_t *days[] = {L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat", L"Sun"};

    ui_render_program->use();

    for (int i = 0; i < 7; ++i)
    {
        _week_view_buttons[i]->draw(ui_render_program->program);
    }

    text_render_program->use();

    for (int i = 0; i < 7; ++i)
    {
        _text_renderer->render_text("arial32", days[i],
                                    vector2{startX + (i * cellW), startY}, Color(0, 0, 255), 1.0f);
    }
}
void UIManager::draw_calendar_month_mode() const
{
    const float cellW = 100.0f;
    const float startX = 100.0f, startY = 600.0f;
    const wchar_t *days[] = {L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat", L"Sun"};

    ui_render_program->use();

    for (int i = 0; i < _month_view_buttons.size(); ++i)
    {
        _month_view_buttons[i]->draw(ui_render_program->program);
    }

    text_render_program->use();

    for (int i = 0; i < 7; ++i)
    {
        _text_renderer->render_text("arial32", days[i],
                                    vector2{startX + (i * cellW), startY}, Color(0, 0, 255), 1.0f);
    }
}

void UIManager::update_tasks()
{
    auto tasks = _calendar.get_all_tasks();
    for (auto task : tasks)
    {
        if (_task_checkboxes.find(task->get_id()) == _task_checkboxes.end())
        {
            _task_checkboxes[task->get_id()] = new Checkbox(
                vector3(),
                vector3(0.05f, 0.05f, 0),
                Color(),
                Color(0, 0, 0),
                [this, task](bool checked)
                {
                    task->set_status(task->get_status() == Task::Status::Undone ? Task::Status::Done : Task::Status::Pending);
                });
        }
    }
}
void UIManager::update(double mouseX, double mouseY, int windowWidth, int windowHeight)
{
    for (auto &[_, btn] : _view_switch_buttons)
    {
        btn->update(mouseX, mouseY, windowWidth, windowHeight);
    }
    switch (_calendar.get_view_mode())
    {
    case Calendar::ViewMode::Day:
        for (auto &[id, cb] : _task_checkboxes)
        {
            cb->update(mouseX, mouseY, windowWidth, windowHeight);
        }
        break;
    case Calendar::ViewMode::Week:
        for (int i = 0; i < _week_view_buttons.size(); i++)
        {
            _week_view_buttons[i]->update(mouseX, mouseY, windowWidth, windowHeight);
        }
        break;
    case Calendar::ViewMode::Month:
        for (int i = 0; i < _month_view_buttons.size(); i++)
        {
            _month_view_buttons[i]->update(mouseX, mouseY, windowWidth, windowHeight);
        }
        break;
    }
}
void UIManager::handle_click(double mouseX, double mouseY, int windowWidth, int windowHeight)
{
    for (auto &[_, btn] : _view_switch_buttons)
    {
        if (btn->contains_point(mouseX, mouseY, windowWidth, windowHeight))
        {
            btn->handle_click();
        }
    }
    switch (_calendar.get_view_mode())
    {
    case Calendar::ViewMode::Day:
        for (auto &[id, cb] : _task_checkboxes)
        {
            if (cb->contains_point(mouseX, mouseY, windowWidth, windowHeight))
            {
                cb->handle_click();
            }
        }
        break;
    case Calendar::ViewMode::Week:
        for (int i = 0; i < _week_view_buttons.size(); i++)
        {
            if (_week_view_buttons[i]->contains_point(mouseX, mouseY, windowWidth, windowHeight))
            {
                _week_view_buttons[i]->handle_click();
            }
        }
        break;
    case Calendar::ViewMode::Month:
        for (int i = 0; i < _month_view_buttons.size(); i++)
        {
            if (_month_view_buttons[i]->contains_point(mouseX, mouseY, windowWidth, windowHeight))
            {
                _month_view_buttons[i]->handle_click();
            }
        }
        break;
    }
}
