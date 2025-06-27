#pragma once
#include <iostream>
#include <memory>
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

    void draw_button(const Button &btn) const;
    void check_buttons_bover(double mouseX, double mouseY, int windowWidth, int windowHeight) const;
    void draw_text_box(const std::string &txt, const Rectangle &bounds) const;
    ~UIManager();

private:
    Calendar _calendar;
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
    std::map<Calendar::ViewMode, std::unique_ptr<Button>> _view_switch_buttons;
};

UIManager::UIManager(unsigned int shader_program)
{
    _shader_program = shader_program;
    _calendar = Calendar();
    _calendar.set_view_mode(Calendar::ViewMode::Month);
    _view_switch_buttons[Calendar::ViewMode::Day] = std::make_unique<Button>(
        vector3(0, 0, 0), vector3(0.25, 0.125, 0),
        Color(0, 0, 0), Color(153, 50, 204),
        [this](Button *btn)
        {
            _calendar.set_view_mode(Calendar::ViewMode::Day);
        },
        [](Button *btn)
        {
            btn->current_bkg_color = btn->base_bkg_color.Lerp(Color(220, 208, 255), 25.0f);
        });

    _view_switch_buttons[Calendar::ViewMode::Week] = std::make_unique<Button>(
        vector3(0.35, 0, 0), vector3(0.25, 0.125, 0),
        Color(0, 0, 0), Color(153, 50, 204),
        [this](Button *btn)
        {
            _calendar.set_view_mode(Calendar::ViewMode::Week);
        },
        [](Button *btn)
        {
            btn->current_bkg_color = btn->base_bkg_color.Lerp(Color(220, 208, 255), 25.0f);
        });

    _view_switch_buttons[Calendar::ViewMode::Month] = std::make_unique<Button>(
        vector3(0.65, 0, 0), vector3(0.25, 0.125, 0),
        Color(0, 0, 0), Color(153, 50, 204),
        [this](Button *btn)
        {
            _calendar.set_view_mode(Calendar::ViewMode::Month);
        },
        [](Button *btn)
        {
            btn->current_bkg_color = btn->base_bkg_color.Lerp(Color(220, 208, 255), 25.0f);
        });
}
UIManager::~UIManager() = default;

void UIManager::draw_calendar() const
{
    // Арбайт
    for (const auto &[mode, btn_ptr] : _view_switch_buttons)
    {
        btn_ptr->Draw(_shader_program);
    }
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
void UIManager::check_buttons_bover(double mouseX, double mouseY, int windowWidth, int windowHeight) const
{
    for (const auto &[mode, btn_ptr] : _view_switch_buttons)
    {
        btn_ptr->CheckHover(mouseX, mouseY, windowWidth, windowHeight);
    }
}