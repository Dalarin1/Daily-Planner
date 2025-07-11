#pragma once
#include <iostream>
#include "Calendar.hpp"
#include "raphics.hpp"
#if 1
class UIManager
{
public:
    UIManager(std::shared_ptr<ShaderProgram> ui_program, std::shared_ptr<TextRenderer> text_renderer);
    void draw_calendar() const;

    void draw_calendar_month_mode() const;
    void draw_calendar_week_mode() const;
    void draw_calendar_day_mode() const;

    void update_tasks() {}
    void update(double mouseX, double mouseY, int windowWidth, int windowHeight);
    void update_geometry(int new_window_width, int new_window_height);

    void crupdate_view_switch_buttons();
    void crupdate_date_switch_elements();
    void crupdate_day_mode();
    void crupdate_week_mode();
    void crupdate_month_mode();

    void handle_click(double mouseX, double mouseY, int windowWidth, int windowHeight);
    ~UIManager();

    std::shared_ptr<ShaderProgram> ui_render_program;
    std::shared_ptr<TextRenderer> _text_renderer;

    Calendar _calendar;
    std::shared_ptr<GLFWwindow> _window;

    // лишние элементы
    // std::vector<UIButton *> _week_view_buttons;
    // std::vector<UIButton *> _month_view_buttons;
    // std::map<int, UICheckbox *> _task_checkboxes;

    // это не лишнее
    std::map<Calendar::ViewMode, UIButton *> _view_switch_buttons;
    std::vector<IUIElement *> _date_switch_elements;
    std::vector<IUIElement *> _top_menu;

    std::vector<IUIElement *> _day_view_elements;
    std::vector<IUIElement *> _week_view_elements;
    std::vector<IUIElement *> _month_view_elements;
};

UIManager::UIManager(std::shared_ptr<ShaderProgram> ui_program, std::shared_ptr<TextRenderer> text_renderer)
{
    ui_render_program = ui_program;
    _text_renderer = text_renderer;
    _calendar = Calendar();
    _calendar.set_view_mode(Calendar::ViewMode::Month);

    crupdate_view_switch_buttons();
    crupdate_date_switch_elements();
    /*
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
}*/
}
UIManager::~UIManager()
{
    for (auto &elem : _day_view_elements)
    {
        delete elem;
    }
    for (auto &elem : _week_view_elements)
    {
        delete elem;
    }
    for (auto &elem : _month_view_elements)
    {
        delete elem;
    }
    for (auto &[_, btn] : _view_switch_buttons)
    {
        delete btn;
    }
}

void UIManager::draw_calendar() const
{

    switch (_calendar.get_view_mode())
    {
    case Calendar::ViewMode::Month:
        // draw_calendar_month_mode();
        break;
    case Calendar::ViewMode::Week:
        // draw_calendar_week_mode();
        break;
    case Calendar::ViewMode::Day:
        draw_calendar_day_mode();
        break;
    default:
        throw std::runtime_error("Unexpected calendar ViewMode");
    }
    ui_render_program->use();
    for (const auto &[mode, btn_ptr] : _view_switch_buttons)
    {
        btn_ptr->draw(ui_render_program->program);
    }
    for (const auto &elem : _date_switch_elements)
    {
        ui_render_program->use();
        elem->draw(ui_render_program->program);
    }
}
void UIManager::draw_calendar_day_mode() const
{
    for (auto &i : _day_view_elements)
    {
        ui_render_program->use();
        i->draw(ui_render_program->program);
    }
}
/*
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
        _text_renderer->render_text(days[i],
                                    startX + (i * cellW), startY, 1.0f, Color(0, 0, 255));
    }
}
void UIManager::draw_calendar_month_mode() const
{
    const float cellW = 100.0f;
    const float startX = 100.0f, startY = 600.0f;
    const char *days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    ui_render_program->use();

    for (int i = 0; i < _month_view_buttons.size(); ++i)
    {
        _month_view_buttons[i]->draw(ui_render_program->program);
    }

    text_render_program->use();

    for (int i = 0; i < 7; ++i)
    {
        _text_renderer->render_text(days[i],
                                    startX + (i * cellW), startY, 1.0f, Color(0, 0, 255));
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
}*/
void UIManager::update(double mouseX, double mouseY, int windowWidth, int windowHeight)
{
    for (auto &[_, btn] : _view_switch_buttons)
    {
        btn->update(mouseX, mouseY, windowWidth, windowHeight);
    }
    switch (_calendar.get_view_mode())
    {
    case Calendar::ViewMode::Day:
        for (auto &elem : _day_view_elements)
        {
            elem->update(mouseX, mouseY, windowWidth, windowHeight);
        }
        break;
    case Calendar::ViewMode::Week:
        for (auto &elem : _week_view_elements)
        {
            elem->update(mouseX, mouseY, windowWidth, windowHeight);
        }
        break;
    case Calendar::ViewMode::Month:
        for (auto &elem : _month_view_elements)
        {
            elem->update(mouseX, mouseY, windowWidth, windowHeight);
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
        for (auto &elem : _day_view_elements)
        {
            if (elem->contains_point(mouseX, mouseY, windowWidth, windowHeight))
            {
                elem->handle_click();
            }
        }
        break;
    case Calendar::ViewMode::Week:
        for (auto &elem : _week_view_elements)
        {
            if (elem->contains_point(mouseX, mouseY, windowWidth, windowHeight))
            {
                elem->handle_click();
            }
        }
        break;
    case Calendar::ViewMode::Month:
        for (auto &elem : _month_view_elements)
        {
            if (elem->contains_point(mouseX, mouseY, windowWidth, windowHeight))
            {
                elem->handle_click();
            }
        }
        break;
    }
}

void UIManager::crupdate_view_switch_buttons()
{
    _view_switch_buttons = {
        {Calendar::ViewMode::Day,
         new UIButton(vector3(0.45, 0.75, 0), vector3(0.15, 0.125, 0), Color(0, 0, 0), Color(153, 50, 204), Color(200, 0, 200), 1, "", [this]()
                      { _calendar.set_view_mode(Calendar::ViewMode::Day); }, [this]()
                      { this->_view_switch_buttons[Calendar::ViewMode::Day]->BackgroundColor =
                            this->_view_switch_buttons[Calendar::ViewMode::Day]->get_base_background_color().Lerp(Color(220, 208, 255), 25.0f); }, _text_renderer)},
        {Calendar::ViewMode::Week, new UIButton(vector3(0.6, 0.75, 0), vector3(0.15, 0.125, 0), Color(0, 0, 0), Color(153, 50, 204), Color(200, 0, 200), 1, "", [this]()
                                                { _calendar.set_view_mode(Calendar::ViewMode::Week); }, [this]()
                                                { this->_view_switch_buttons[Calendar::ViewMode::Week]->BackgroundColor =
                                                      this->_view_switch_buttons[Calendar::ViewMode::Week]->get_base_background_color().Lerp(Color(220, 208, 255), 25.0f); }, _text_renderer)},
        {Calendar::ViewMode::Month, new UIButton(vector3(0.75, 0.75, 0), vector3(0.15, 0.125, 0), Color(0, 0, 0), Color(153, 50, 204), Color(200, 0, 200), 1, "", [this]()
                                                 { _calendar.set_view_mode(Calendar::ViewMode::Month); }, [this]()
                                                 { this->_view_switch_buttons[Calendar::ViewMode::Month]->BackgroundColor =
                                                       this->_view_switch_buttons[Calendar::ViewMode::Month]->get_base_background_color().Lerp(Color(220, 208, 255), 25.0f); }, _text_renderer)}};
}
void UIManager::crupdate_date_switch_elements()
{
    _date_switch_elements = {
        new UITextfield(vector3(-0.2, 0.75, 0), vector3(0.4, 0.125, 0), Color(100, 100, 100), Color(0, 0, 0), Color(0, 0, 0), 1, "DAY ONE", _text_renderer),
        new UIButton(vector3(0.22, 0.75, 0), vector3(0.125, 0.125, 0), Color(100, 100, 100), Color(0, 0, 0), Color(0, 0, 0), 1, "->", nullptr, nullptr, _text_renderer),
        new UIButton(vector3(-0.325, 0.75, 0), vector3(0.125, 0.125, 0), Color(100, 100, 100), Color(0, 0, 0), Color(0, 0, 0), 1, "<-", nullptr, nullptr, _text_renderer)};
}
void UIManager::crupdate_day_mode()
{
    std::vector<Task *> tasks = _calendar.get_tasks_for_day(_calendar.get_current_date());
    _day_view_elements = {
        new UIRectangle(vector3(-0.9, 0.6, 0), vector3(1.8, 1.5, 0), Color(), Color(100, 100, 100), 1, _text_renderer),  // общий фон
        new UIRectangle(vector3(-0.85, 0.55, 0), vector3(0.8, 0.975, 0), Color(92, 92, 92), Color(), 0, _text_renderer), // фон блока задач
        new UIRectangle(vector3(0.0, 0.55, 0), vector3(0.85, 1.25, 0), Color(34, 34, 34), Color(), 0, _text_renderer),   // фон блока просмотра/редактирования задачи
    };
    for (int i = 0; i < tasks.size(); i++)
    {
        _day_view_elements.push_back(new UICheckbox(vector3(-0.8f, 0.5f - 0.1 * i, 0), vector3(0.05, 0.05, 0), Color(), Color(0, 0, 0), Color(0, 0, 1), 1, tasks[i]->get_title(), nullptr, _text_renderer));
    }
}
void UIManager::crupdate_week_mode()
{
    _week_view_elements = {
        new UIRectangle(vector3(), vector3(), Color(), Color(), 0), // общий фон
        // дни недели
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Mon", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Tue", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Wen", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Thu", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Fri", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Sat", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Sun", _text_renderer),
        // Фон
        new UIRectangle(),

        // отрисовка линиий границ между часами
        // new UILine(),
        // new UILine(),
        // new UILine(),
        //<...>

    };
}
void UIManager::crupdate_month_mode()
{
    _month_view_elements = {
        new UIRectangle(vector3(), vector3(), Color(), Color(), 0), // общий фон
                                                                    // дни недели
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Mon", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Tue", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Wen", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Thu", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Fri", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Sat", _text_renderer),
        new UITextfield(vector3(), vector3(), Color(176, 146, 146), Color(), Color(0, 255, 0), 0, "Sun", _text_renderer),

        // набор кнопок для каждого дня месяца
        // new UIButton(),
        // new Button(),
        // new Button(),
        //<...>
    };
}
#endif