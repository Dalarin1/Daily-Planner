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
    UITextfield *m_active_textfield = nullptr;
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

void UIManager::draw_calendar_week_mode() const
{
    for (auto &elem : _week_view_elements)
    {
        ui_render_program->use();
        elem->draw(ui_render_program->program);
    }
}
void UIManager::draw_calendar_month_mode() const
{
    for (auto &elem : _month_view_elements)
    {
        ui_render_program->use();
        elem->draw(ui_render_program->program);
    }
}
/*
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
    if (m_active_textfield != nullptr)
    {
        if (!m_active_textfield->contains_point(mouseX, mouseY, windowWidth, windowHeight))
        {
            m_active_textfield = nullptr;
        }
    }
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
        for (auto* elem : _day_view_elements)
        {

            if (elem->contains_point(mouseX, mouseY, windowWidth, windowHeight))
            {
                elem->handle_click();
                
                if (auto* txtfield = dynamic_cast<UITextfield*>(elem)) // если элемент UITextfield
                {
                    m_active_textfield = txtfield;
                }
                // else if(m_active_textfield){
                //     m_active_textfield->set_focus(false);
                //     m_active_textfield = nullptr;
                // }
                
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
                      { _calendar.set_view_mode(Calendar::ViewMode::Day); crupdate_date_switch_elements(); }, [this]()
                      { this->_view_switch_buttons[Calendar::ViewMode::Day]->BackgroundColor =
                            this->_view_switch_buttons[Calendar::ViewMode::Day]->get_base_background_color().Lerp(Color(220, 208, 255), 25.0f); }, _text_renderer)},
        {Calendar::ViewMode::Week, new UIButton(vector3(0.6, 0.75, 0), vector3(0.15, 0.125, 0), Color(0, 0, 0), Color(153, 50, 204), Color(200, 0, 200), 1, "", [this]()
                                                { _calendar.set_view_mode(Calendar::ViewMode::Week);  crupdate_date_switch_elements(); }, [this]()
                                                { this->_view_switch_buttons[Calendar::ViewMode::Week]->BackgroundColor =
                                                      this->_view_switch_buttons[Calendar::ViewMode::Week]->get_base_background_color().Lerp(Color(220, 208, 255), 25.0f); }, _text_renderer)},
        {Calendar::ViewMode::Month, new UIButton(vector3(0.75, 0.75, 0), vector3(0.15, 0.125, 0), Color(0, 0, 0), Color(153, 50, 204), Color(200, 0, 200), 1, "", [this]()
                                                 { _calendar.set_view_mode(Calendar::ViewMode::Month); crupdate_date_switch_elements(); }, [this]()
                                                 { this->_view_switch_buttons[Calendar::ViewMode::Month]->BackgroundColor =
                                                       this->_view_switch_buttons[Calendar::ViewMode::Month]->get_base_background_color().Lerp(Color(220, 208, 255), 25.0f); }, _text_renderer)}};
}
void UIManager::crupdate_date_switch_elements()
{
    auto curr_mode = _calendar.get_view_mode();
    auto curr_date = _calendar.get_current_date();
    auto week_start = date::sys_days(date::sys_days(curr_date) - (date::weekday(date::sys_days(curr_date)) - date::Monday));
    auto week_end = date::sys_days(date::sys_days(curr_date) + (date::Sunday - date::weekday(date::sys_days(curr_date))));
    std::string curr_date_string = "";
    switch (curr_mode)
    {
    case Calendar::ViewMode::Day:
        curr_date_string = date_to_string(_calendar.get_current_date());
        break;
    case Calendar::ViewMode::Week:
        curr_date_string = date_to_string(week_start) + " - " + date_to_string(week_end);
        break;
    case Calendar::ViewMode::Month:
        curr_date_string = date::format("%B, %Y", curr_date);
        break;
    }
    _date_switch_elements = {
        new UITextfield(vector3(-0.2, 0.75, 0), vector3(0.4, 0.125, 0), Color(100, 100, 100), Color(0, 0, 0), Color(0, 0, 0), 1, curr_date_string, _text_renderer),
        new UIButton(vector3(0.22, 0.75, 0), vector3(0.125, 0.125, 0), Color(100, 100, 100), Color(0, 0, 0), Color(0, 0, 0), 1, "->", [this]()
                     {
            switch(_calendar.get_view_mode()){
                case Calendar::ViewMode::Day:
                _calendar.navigate_to_date(date::sys_days(_calendar.get_current_date()) + date::days{1});
                crupdate_date_switch_elements();
                break;
                case Calendar::ViewMode::Week:
                _calendar.navigate_to_date(date::sys_days(_calendar.get_current_date()) + date::weeks{1});
                crupdate_date_switch_elements();
                break;
                case Calendar::ViewMode::Month:
                crupdate_date_switch_elements();
                break;
            } }, nullptr, _text_renderer),
        new UIButton(vector3(-0.345, 0.75, 0), vector3(0.125, 0.125, 0), Color(100, 100, 100), Color(0, 0, 0), Color(0, 0, 0), 1, "<-", [this]()
                     {
            switch(_calendar.get_view_mode()){
                case Calendar::ViewMode::Day:
                _calendar.navigate_to_date(date::sys_days(_calendar.get_current_date()) - date::days{1});
                crupdate_date_switch_elements();
                break;
                case Calendar::ViewMode::Week:
                _calendar.navigate_to_date(date::sys_days(_calendar.get_current_date()) - date::weeks{1});
                crupdate_date_switch_elements();
                break;
                case Calendar::ViewMode::Month:
                crupdate_date_switch_elements();
                break;
            } }, nullptr, _text_renderer)};
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
    _day_view_elements.push_back(new UITextfield(vector3(0.05, 0.545, 0),  vector3(0.84, 0.25, 0), Color(), Color(0,0,0), Color(0,0,0), 1, "PLAIN TEXT", _text_renderer));

}
void UIManager::crupdate_week_mode()
{

    _week_view_elements = {
        new UIRectangle(vector3(-0.9, 0.6, 0), vector3(1.8, 1.5, 0), Color(), Color(100, 100, 100), 1, _text_renderer), // общий фон
    };

    const char *days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    auto curr_date = _calendar.get_current_date();
    auto week_start = date::sys_days(date::sys_days(curr_date) - (date::weekday(date::sys_days(curr_date)) - date::Monday));
    std::vector<Task *> temp_tasks = {};
    for (int i = 0; i < 7; i++)
    {
        _week_view_elements.push_back(new UITextfield(vector3(-0.875 + 0.25 * i, 0.575, 0), vector3(0.25, 0.125, 0), Color(146, 146, 146), Color(0, 0, 0), Color(0, 255, 0), 1, days[i], _text_renderer));
        _week_view_elements.push_back(new UIButton(vector3(-0.875 + 0.25 * i, 0.275, 0), vector3(0.25, 0.5, 0), Color(), Color(0, 0, 0), Color(0, 0, 0), 1, std::to_string(_calendar.get_tasks_for_day(week_start + date::days{i}).size()) + " Tasks", [this, i, week_start]()
                                                   {
            _calendar.navigate_to_date(week_start + date::days{i});
            _calendar.set_view_mode(Calendar::ViewMode::Day);
           crupdate_date_switch_elements();
           crupdate_day_mode(); }, nullptr, _text_renderer));
    }
}
void UIManager::crupdate_month_mode()
{
    _month_view_elements = {
        new UIRectangle(vector3(-0.9, 0.6, 0), vector3(1.8, 1.5, 0), Color(), Color(100, 100, 100), 1, _text_renderer), // общий фон
    };
    const char *days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    auto curr_date = _calendar.get_current_date();
    auto week_start = date::sys_days(date::sys_days(curr_date) - (date::weekday(date::sys_days(curr_date)) - date::Monday));

    auto start_wkd = date::weekday(date::year_month_day(curr_date.year(), curr_date.month(), date::day{1}));
    auto days_count = (date::sys_days(date::year_month_day(date::year_month_day_last(curr_date.year(), date::month_day_last(curr_date.month())))) -
                       date::sys_days(date::year_month_day(curr_date.year(), curr_date.month(), date::day{1})))
                          .count() +
                      1;
    std::vector<Task *> temp_tasks = {};
    for (int i = 0; i < 7; i++)
    {
        _month_view_elements.push_back(new UITextfield(vector3(-0.875 + 0.25 * i, 0.575, 0), vector3(0.25, 0.125, 0), Color(146, 146, 146), Color(0, 0, 0), Color(0, 255, 0), 1, days[i], _text_renderer));
    }

    float currY = 0.425;
    UIButton *btn = nullptr;
    for (int8_t i = 0; i < days_count; i++)
    {
        if ((i + 1) % 7 == 0)
        {
            currY -= 0.25;
        }

        btn = new UIButton(
            vector3(-0.875 + date::weekday{(unsigned int)((i + 1) % 7)}.c_encoding() * 0.25, currY, 0),
            vector3(0.25, 0.25, 0),
            Color(),
            Color(0, 0, 0),
            Color(0, 0, 0),
            0,
            std::to_string(i + 1),
            [this, i, curr_date]()
            {
                _calendar.navigate_to_date(date::year_month_day(curr_date.year(), curr_date.month(), date::day{(unsigned int)(i + 1)}));
                _calendar.set_view_mode(Calendar::ViewMode::Day);
                crupdate_date_switch_elements();
                crupdate_day_mode();
            },
            nullptr, _text_renderer);
        btn->OnHover = [btn]()
        {
            btn->BackgroundColor = Color(100, 100, 100);
            btn->BorderWidth = 1;
        };
        _month_view_elements.push_back(btn);
    }
}
#endif