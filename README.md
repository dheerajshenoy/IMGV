# IMGV

The Image Viewer world never asked for. Written in C++.

# Ehh....Why ?

Why ? I never liked any of the image viewers existing for linux, with the exception of nomacs. I did not like the super minimal approach of sxiv/nsxiv image viewer, and I did not like the full blown features of heavy image viewer. I just want something to open, click, press and it should do what I need it to do. This pretty much does the job for me.

# Table of Contents

1. [Screenshots](#screenshots)
2. [Introduction](#introduction)
3. [Features](#features)
4. [Installation](#installation)
4. [Changelogs and Bug fixes](#changelogs)


<a name="screenshots" />

# Screenshots


<a name="introduction" />

# Introduction

<a name="features" />

# Features

1. Minimally intrusive user-interface
2. Everything is customizable
3. Session file support

    Session files allow you to group images together to open it at a later point.

4. Image format support
    1. JPG/JPEG
    2. PNG
    3. BMP
    4. SVG
    5. GIF
    6. WEBP

5. Command line arguments support.

<a name="installation" />

# Installation

This software depends on the following libraries.

    1. Qt (GUI)
    2. [sol2](https://github.com/ThePhD/sol2) and lua (Lua configuration support)
    3. libwebp (for WEBP image support)
    4. [argparser](https://github.com/p-ranav/argparse)(for commandline argument parsing)

<a name="changelog" />

# Changelogs and Bug Fixes

    - 06 Aug 2024

        - Add support for GIF and WEBP (libwebp) format.
