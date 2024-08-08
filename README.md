# IMGV

The Image Viewer no one asked for.
Written in C++.
Only for **Linux**.

**NOTE** This is a work in progress. I'll add features as I seem fit for this software.

# Table of Contents

1. [Screenshots](#screenshots)
2. [Introduction](#introduction)
3. [Features](#features)
4. [Installation](#installation)
5. [Theming](#theming)
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

I aM an imaGe Viewer ~ IMGV

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

<a name="installation" />

# Installation

1. This software depends on the following programs/libraries. Install them first.

    1. Qt (GUI)
    2. lua
    3. libwebp (for WEBP image support)

2. Clone this repo.

3. Go the project directory and run `qmake6 .`

4. Finally, if all went well, run `make` and copy the binary file inside the bin directory to `/usr/bin` if you want to access the software system-wide.

5. Enjoy...hopefully

One line command (assuming you are inside the project directory)

`qmake6 . && make && sudo cp ./bin/imgv /usr/bin`

<a name="theming" />

# Theming

The theme for IMGV depends on the theme set for Qt6 applications. There is a handy tool called `qt6ct`. Install it and configure the theme and it will be applied to this software also.

<a name="changelogs" />

# Changelogs and Bug Fixes

- 08 Aug 2024

    - Removed `QListWidget` and replaced with `QListView`
    - Added search feature for thumbnails
    - Added note functionality
    - Changed session file format to json files for easy reading
    - Added dependency for rapidjson

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
4. [ ] Lua configuration option

<a name="thanks" />

# Thanks

I want to thank the authors of the following libraries I have integrated in this software.

1. [Sol2](https://github.com/ThePhD/sol2)
2. [ArgParse](https://github.com/p-ranav/argparse)
3. [libwebp](https://github.com/webmproject/libwebp)
4. [EasyEXIF](https://github.com/mayanklahiri/easyexif)
5. [rapidjson](https://github.com/tencent/rapidjson)
