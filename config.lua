-- This is the config file for IMGV

Defaults = {
    menubar = true,
    auto_notes_popup = false,
    scrollbars = true,
    start_in_minimal_mode = false,

    minimap = {
        rect_color = "#FF5000", -- size of the overview box on the minimap
        size = { width = 200, height = 200 }, -- size of the minimap
    },

    thumbnails = {
        show = true, -- show panel or not
        mode = "icon", -- icon or list
        resize = true,
        uniform = true,
        selection = "extended", -- "extended", "multi", "single", "contiguous",
        spacing = 20,
        icon_size = { width = 100, height = 100 },
        text_elide = "middle", -- "none", "left", "right", "middle"
    },
    
    keybindings = {
        zoom_in = "=",
        zoom_out = "-",
        zoom_reset = "0",
        fit_width = "w",
        fit_window = "Shift+h",
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
        path - shows full file path
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
            "note-indicator",
            "note-modified-indicator",
            "stretch",
            "zoom",
            "size",
            "dimension",
            "session",
        }
    }
}

