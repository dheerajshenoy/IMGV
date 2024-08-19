-- This is the config file for IMGV
-- Check out the documentation for more information about configuring IMGV

Defaults = {
    image_cache = 10240 * 2, -- image caching in kilobytes
    menubar = true,
    auto_notes_popup = false,
    scrollbars = true,
    start_in_minimal_mode = false,
    scroll_factor = { horizontal = 40.0, vertical = 40.0 },
    zoom_factor = 5.0, -- (default : 2.0)
    fit_on_load = true, -- (default : false)
    fit_on_load_mode = "height", -- fit to "height" or "width" or "none" ; (default : "none")
    minimap = {
        show = false,
        auto_hide = true, -- hide minimap when not necessary
        size = { width = 200, height = 200 }, -- size of the minimap
        rect = {
            outline = "#FF5000", -- color of the overview box on the minimap
            color = "#FF5000", -- fill color, if empty, no color is filled
            alpha = 0.5, -- 0 is opaque and 1 is transparent
        },
        location = "bottom-right", -- "bottom-right", "bottom-left", "top-right", "top-left",
    },

    thumbnails = {
        show = true, -- show panel or not
        layout = "left", -- "left", "bottom", "top", "right"
        mode = "list", -- icon or list
        resize = true,
        uniform = true,
        selection = "extended", -- "extended", "multi", "single", "contiguous",
        spacing = 0,
        icon_size = { width = 400, height = 400 },
        text = {
            elide = "middle", -- "none", "left", "right", "middle"
            wrap = false,
        },
        alternate_colors = true,
    },

    keybindings = {
        zoom_in = "=",
        zoom_out = "-",
        zoom_reset = "0",
        fit_width = "w",
        fit_height = "Shift+h",
        rotate_clockwise = ".",
        rotate_anticlockwise = ",",
        flip_vertical = "'",
        flip_horizontal = ";",
        left = "h",
        down = "j",
        up = "k",
        right = "l",
        next = "Shift+j",
        prev = "Shift+k",
        notes = "n",
        maximize = "Shift+m",
        toggle_menubar = "Ctrl+m",
        toggle_statusbar = "Ctrl+x",
        toggle_thumbnail_panel = "Ctrl+t",
        search = "/",
    },

    statusbar = {
        show = true, -- false to hide
        spacing = 10,
        note_indicator = "NOTE",
        note_modified_indicator = "[M]",

        --[[

    Elements:
        message - shows important messages
        path - shows full file path while mouse hover, short otherwise
        size - file size
        stretch - separator
        dimension - shows the (width, height) of the image
        session - shows the session name
        note-indicator - shows an indicator when there is a non-empty note associated with an image file
        note-modified-indicator - shows an indicator when there is any unsaved changes to a note for an image file

    --]]
        elements = {
            "message",
            "path",
            "stretch",
            "note-indicator",
            "note-modified-indicator",
            "zoom",
            "size",
            "dimension",
            "session",
        }
    }
}

