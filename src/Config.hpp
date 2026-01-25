#pragma once

#include "Minimap.hpp"

#include <QHash>
#include <QSize>
#include <QString>

struct Config
{

    struct Minimap
    {
        bool shown{true};
        bool auto_hide{true};
        bool image{true};
        float padding{10.0f};
        QSize size{QSize(200, 200)};
        float image_opacity{0.7f};
        ::Minimap::Location location{::Minimap::Location::BOTTOM_RIGHT};
        bool clickable{true};
        uint32_t border_color{0x550000FF};
        int border_width{1};

        struct Overlay
        {
            bool shown{true};
            bool movable{true};
            uint32_t color{0x55FF0000};
            uint32_t border_color{0x5500FF00};
            int border_width{1};
        } overlay;

    };

    struct Scrollbar
    {
        bool shown{true};
        bool auto_hide{true};
    };

    struct Tabs
    {
        bool shown{true};
        bool autohide{true};
        QString location{"top"};
    };

    struct Statusbar
    {
        bool shown{true};
        QString location{"bottom"};
        int padding{5};
        bool filepath_complete{true};
    };

    struct Menubar
    {
        bool shown{true};
    };

    struct Rendering
    {
        std::variant<float, QMap<QString, float>> dpr{};
    };

    struct FocusMode
    {
        bool statusbar_shown{false};
        bool menubar_shown{false};
        bool minimap_shown{false};
        bool hscrollbar_shown{false};
        bool vscrollbar_shown{false};
        bool tabs_shown{false};
    };

    struct Behavior
    {
        bool auto_reload{false};
        bool config_hot_reload{true};
        bool save_recent_files{true};
        int recent_files_limit{10};
        bool auto_fit{false};
        bool keybind_conflict_warning{true};
        bool copy_transformed_image{false};
    };

    Scrollbar hscrollbar{}, vscrollbar{};
    Tabs tabs{};
    Statusbar statusbar{};
    Menubar menubar{};
    Rendering rendering{};
    Behavior behavior{};
    FocusMode focus_mode{};
    Minimap minimap{};

    QHash<QString, QString> shortcutMap;
};
