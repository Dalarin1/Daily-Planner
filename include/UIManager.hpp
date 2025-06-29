#pragma once
#include <iostream>
#include "Calendar.hpp"
#include "raphics.hpp"

class UIManager
{
public:
    UIManager(unsigned int shader_program);
    void draw_calendar() const;

    void draw_calendar_month_mode() const;
    void draw_calendar_week_mode() const;
    void draw_calendar_day_mode() const;

    void update(double mouseX, double mouseY, int windowWidth, int windowHeight);
    void handle_click(double mouseX, double mouseY, int windowWidth, int windowHeight);
    void draw_all(unsigned int shader_program) const;

    ~UIManager();

private:
    Calendar _calendar;
    std::vector<IUIElement *> _elements;
    GLFWwindow *_window;
    unsigned int _vbo;
    unsigned int _vao;
    unsigned int _font_texture;
    unsigned int _shader_program;
    unsigned int _icon_textures;
    Color background_color;
    Color border_color;
    Button *day_btn;
    Button *week_btn;
    Button *month_btn;
    std::map<Calendar::ViewMode, Button *> _view_switch_buttons;
};

UIManager::UIManager(unsigned int shader_program)
{
    _shader_program = shader_program;
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
}
UIManager::~UIManager() = default;

void UIManager::draw_calendar() const
{
    // Арбайт
    // for (const auto &[mode, btn_ptr] : _view_switch_buttons)
    // {
    //     btn_ptr->draw(_shader_program);
    // }
    // В процессе
    /*switch (_calendar.get_view_mode())
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
        std::__throw_runtime_error("Unexpected calendar ViewMode");
    }*/
}
void UIManager::draw_calendar_day_mode() const
{
}
