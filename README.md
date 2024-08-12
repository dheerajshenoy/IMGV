# IMGV

The Image Viewer no one asked for.

Written in C++.

Only for **Linux**.

With [features](#features) I like.

# Table of Contents

1. [Screenshots](#screenshots)
2. [Introduction](#introduction)
3. [Features](#features)
4. [Installation](#installation)
5. [Theming](#theming)
6. [Configuration](#configuration)
6. [Changelogs and Bug fixes](#changelogs)
7. [Todo](#todo)
8. [Thanks](#thanks)

<a name="screenshots" />

# Screenshots

![commandline-arguments](https://github.com/user-attachments/assets/f2f912e8-a50b-45fd-b574-bc77c4b80f6a)

![image](https://github.com/user-attachments/assets/c5f42acb-0760-45e7-a41d-11fac2570c41)

<a name="introduction" />

# Introduction

IMGV is an image viewer aiming for the balance between minimalism and features. The aim is simple. Display IMAGES....quickly, switch between them, do whatever the user wants it to do (through a little bit of scripting).

## Ehh....Why ?

Why ? I never liked any of the image viewers existing for linux, with the exception of nomacs. I did not like the super minimal approach of sxiv/nsxiv image viewer, and I did not like the full blown features of heavy image viewer. I just want something to open, click, press and it should do what I need it to do. This pretty much does the job for me.

## Why the name IMGV ?

IMaGe Viewer ~ IMGV. **Original**.

<a name="features" />

# Features

1. Minimally intrusive user-interface
2. Everything is customizable
3. Session file support

    Session files allow you to group images together to open it at a later point. *NOTE*: Previously session files were just plain text files, but since the addition of notes features, session files are now stored as json files with the extension .imgv.
   
5. Image format support
    1. JPG/JPEG
    2. PNG
    3. BMP
    4. SVG
    5. GIF
    6. WEBP

6. Command line arguments support.
    - `-i` or `--input` takes in path to one or more image files of the supported format.
    - `-n` or `--no-config` load the software without any lua configuration
    - `-l` or `--list-sessions` list all the sessions
    - `-s` or `--session` takes path to a session file. Loads all the images of that session 

7. Notes

![image](https://github.com/user-attachments/assets/59a58073-295a-4e67-9666-e6e26fc8d0f0)

8. Image Properties (with EXIF support)

![image](https://github.com/user-attachments/assets/2b575390-0799-4524-9083-5618538c7a7d)

<a name="installation" />

# Installation

## Install from release package

Check out the [releases](https://github.com/dheerajshenoy/imgv/releases) page for packages.

This software has been built using `Qt v6.7.2`. So, at the time of writing, Ubuntu users are still on Qt v6.4.2 and hence this wont be support until I make a build against that version of Qt (really hectic). There is however Flatpak available.

### Flatpak

Install instruction for your distro can be found [here](https://flatpak.org/setup/)

- hazzle free, no need to download any stupid dependencies, no BS, just straight to the point.
- download the flatpak from the [releases](https://github.com/dheerajshenoy/imgv/releases) page.
- **NOTE**: The user configuration and sessions in the case of imgv flatpak are stored at `~/.var/app/org.dheerajshenoy.imgv/config/`
- Also, to do the theming for the flatpak version, please check [link1](https://itsfoss.com/flatpak-app-apply-theme/), [link2](https://www.reddit.com/r/flatpak/comments/y9jmqj/the_general_flatpak_qt_and_gtk_theming_guide/)

### Arch Linux users

- download the PKGBUILD file and install the program using the following command.

`mkdir imgv && wget https://github.com/dheerajshenoy/imgv/releases/download/1.2.0/PKGBUILD && makepkg -si`

- download the pkg.tar.zst file directly from the [releases](https://github.com/dheerajshenoy/imgv/releases) page and install it using `pacman -U <downloaded-file-name>`

## Build from Source

1. This software depends on the following programs/libraries. Install them first.

    1. Qt (GUI)
    2. lua
    3. webp (for WEBP image support)

2. Run the following set of commands

```
git clone https://github.com/dheerajshenoy/imgv.git
cd imgv
mkdir build && cd build
cmake ..
make
sudo make install
```

These commands do the following things

- Clone this repo.
- Go the project directory make a build directory and run the cmake command to generate the makefile
- Finally run the make command and also install it system wide

<a name="theming" />

# Theming

The theme for IMGV depends on the theme set for Qt6 applications. There is a handy tool called `qt6ct`. Install it and configure the theme and it will be applied to this software also.

<a name="changelogs" />

# Configuration

IMGV can be configured using the lua programming language. `config.lua` file should have already been created at the config location. The following code lists all the options possible:

```lua
Defaults = {
    statusbar = true,
    menubar = true,
    thumbnail_panel = true,
    auto_notes_popup = false,
    scrollbars = true,
    start_in_minimal_mode = false,
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
    }
}

Statusbar = {
    spacing = 10,
    note_indicator = "NOTE",
    note_modified_indicator = "[M]",
    --[[

    Change the order or removing statusbar elements 

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
        "size",
        "dimension",
        "session",
    }
}
```

Note that if the `keybindings` table is present in the `Defaults` table, then default keybindings will not be loaded.

# Changelogs and Bug Fixes

- 09 Aug 2024

    - Open sessions from Manage Session Dialog
    - Change build system from qmake to cmake

- 08 Aug 2024

    - Remove & Rename session files
    - Bulf remove session files. Ask confirmation based on number of files selected.
    - Removed `QListWidget` and replaced with `QListView`
    - Added search feature for thumbnails
    - Added note functionality
    - Changed session file format to json files for easy reading
    - Added dependency for rapidjson
    - Show note modification status on the statusbar
    - Markdown Highlighting support for the Note Widget
    - Lua support for configuration
    - Display full image properties

    ![image](https://github.com/user-attachments/assets/1c8f2e7d-7efd-420e-8ec0-dd3cf78a3409)

- 07 Aug 2024

    - EXIF Metadata support (JPEG metadata)
    - Added Image Caching support through `QPixmapCache`
    - Added icons

- 06 Aug 2024

    - Add support for GIF and WEBP (libwebp) format.
    - `--input` option directory as argument fix
    - Close sessions
    - Don't ask for save session each time after opening a session

<a name="todo" />

# Todo

1. [x] Ability to add notes to images inside sessions
2. [ ] ~~Load image from URL~~
3. [x] Caching images
4. [x] Lua configuration option
5. [x] Display Image Properties

<a name="thanks" />

# Thanks

I want to thank the authors of the following libraries I have integrated in this software.

1. [Sol2](https://github.com/ThePhD/sol2)
2. [ArgParse](https://github.com/p-ranav/argparse)
3. [libwebp](https://github.com/webmproject/libwebp)
4. [EasyEXIF](https://github.com/mayanklahiri/easyexif)
5. [rapidjson](https://github.com/tencent/rapidjson)
