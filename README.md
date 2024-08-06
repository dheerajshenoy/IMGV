# IMGV

The Image Viewer no one asked for.
Written in C++.

**NOTE** This is a work in progress. I'll add features as I seem fit for this software.

# Ehh....Why ?

Why ? I never liked any of the image viewers existing for linux, with the exception of nomacs. I did not like the super minimal approach of sxiv/nsxiv image viewer, and I did not like the full blown features of heavy image viewer. I just want something to open, click, press and it should do what I need it to do. This pretty much does the job for me.

# Table of Contents

1. [Screenshots](#screenshots)
2. [Introduction](#introduction)
3. [Features](#features)
4. [Installation](#installation)
5. [Changelogs and Bug fixes](#changelogs)

<a name="screenshots" />

# Screenshots

![commandline-arguments](https://github.com/user-attachments/assets/f2f912e8-a50b-45fd-b574-bc77c4b80f6a)

![image](https://github.com/user-attachments/assets/c5f42acb-0760-45e7-a41d-11fac2570c41)

<a name="introduction" />

# Introduction

IMGV is an image viewer aiming for the balance between minimalism and features. The aim is simple. Display IMAGES....quickly, switch between them, do whatever the user wants it to do (through a little bit of scripting).

<a name="features" />

# Features

1. Minimally intrusive user-interface
2. Everything is customizable
3. Session file support

    Session files allow you to group images together to open it at a later point. They are just plain text files with date and list of files. In the program, you can choose to open a session, and it will open all the listed images in them.
   
   ![image](https://github.com/user-attachments/assets/5e8e74c2-6b7a-4113-80c4-d2819ba09767)


5. Image format support
    1. JPG/JPEG
    2. PNG
    3. BMP
    4. SVG
    5. GIF
    6. WEBP

6. Command line arguments support.

<a name="installation" />

# Installation

This software depends on the following libraries.

1. Qt (GUI)
2. [sol2](https://github.com/ThePhD/sol2) and lua (Lua configuration support)
3. libwebp (for WEBP image support)
4. [argparser](https://github.com/p-ranav/argparse)(for commandline argument parsing)

<a name="changelogs" />

# Changelogs and Bug Fixes

- 06 Aug 2024

    - Add support for GIF and WEBP (libwebp) format.
