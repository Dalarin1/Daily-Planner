#pragma once
#include <iostream>
#include "Calendar.hpp"
#include "raphics.hpp"

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
    void handle_click(double mouseX, double mouseY, int windowWidth, int windowHeight);
    void draw_all(unsigned int shader_program) const;

    ~UIManager();

    ShaderProgram *ui_render_program;
    ShaderProgram *text_render_program;
    TextRenderer *_text_renderer;

    Calendar _calendar;
    std::vector<IUIElement *> _elements;
    GLFWwindow *_window;
    Color background_color;
    Color border_color;
    Button *day_btn;
    Button *week_btn;
    Button *month_btn;
    std::vector<date::year_month_day> _current_week_dates;
    std::vector<Button *> _week_view_buttons;
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
        {
            _calendar.set_view_mode(Calendar::ViewMode::Day);
        },
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
        {
            _calendar.set_view_mode(Calendar::ViewMode::Week);
        },
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
        {
            _calendar.set_view_mode(Calendar::ViewMode::Month);
        },
        [this]()
        {
            this->_view_switch_buttons[Calendar::ViewMode::Month]->current_bkg_color =
                this->_view_switch_buttons[Calendar::ViewMode::Month]->base_bkg_color.Lerp(Color(220, 208, 255), 25.0f);
        });

    _elements = {};
    _elements.push_back(_view_switch_buttons.at(Calendar::ViewMode::Month));
    _elements.push_back(_view_switch_buttons.at(Calendar::ViewMode::Day));
    _elements.push_back(_view_switch_buttons.at(Calendar::ViewMode::Week));

    _task_checkboxes = {};
    auto dat = date::sys_days(_calendar.get_current_date());
    auto wkd = date::weekday(dat);
    auto wkd_diff = (wkd - date::Monday);
    auto week_start = date::sys_days(dat - wkd_diff);
    _current_week_dates = {};
    for (int8_t i = 0; i < 7; i++)
    {
        _current_week_dates.push_back(week_start + date::days{i});
    }
    _week_view_buttons = {};
    for (int8_t i = 0; i < 7; i++)
    {
        _week_view_buttons.push_back(new Button(vector3(-0.75 + 0.25 * i, 0.5, 0), vector3(0.125, 0.8, 0), Color(), Color(0, 0, 0), [this, i]()
                                                {_calendar.navigate_to_date(_current_week_dates[i]);_calendar.set_view_mode(Calendar::ViewMode::Day); }, nullptr));
        _elements.push_back(_week_view_buttons[i]);
    }
}
UIManager::~UIManager() = default;

void UIManager::draw_calendar() const
{
    ui_render_program->use();
    // Арбайт
    for (const auto &[mode, btn_ptr] : _view_switch_buttons)
    {
        btn_ptr->draw(ui_render_program->program);
    }
    // В процессе
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
        _text_renderer->render_text_GL_coords(tasks[i]->get_title(), -0.5 + 0.05, 0.55f - 0.1 * i, 1.0f, Color(0, 0, 0), 800, 800);
    }
}
void UIManager::draw_calendar_week_mode() const
{
    const float cellW = 100.0f;
    const float startX = 100.0f, startY = 600.0f;
    const char *days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    ui_render_program->use();

    for (int i = 0; i < 7; ++i)
    {
        _week_view_buttons[i]->draw(ui_render_program->program);
    }

    text_render_program->use();

    for (int i = 0; i < 7; ++i)
    {
        _text_renderer->render_text(
            days[i],
            startX + (i * cellW), startY, 1.0f, Color(0, 0, 255));
    }
}
void UIManager::draw_calendar_month_mode() const
{
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
            _elements.push_back(_task_checkboxes[task->get_id()]);
        }
    }
}
void UIManager::update(double mouseX, double mouseY, int windowWidth, int windowHeight)
{
    for (int i = 0; i < _elements.size(); i++)
    {
        _elements[i]->update(mouseX, mouseY, windowWidth, windowHeight);
    }
}
void UIManager::handle_click(double mouseX, double mouseY, int windowWidth, int windowHeight)
{
    for (int i = 0; i < _elements.size(); i++)
    {
        if (_elements[i]->contains_point(mouseX, mouseY, windowWidth, windowHeight))
        {
            _elements[i]->handle_click();
        }
    }
}
