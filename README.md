distortosDisplayExamples
==========================

Display usage examples for [distortos](http://distortos.org/) - object-oriented C++ RTOS for microcontrollers

Configuration & building
------------------------

Following commands may be executed in POSIX-compatible shell (e.g. *Bash*).

#### 1. Download

Clone the repository with distortos submodule:

    $ git clone --recursive https://github.com/CezaryGapinski/distortosDisplayExamples
    $ cd distortosDisplayExamples

#### 2. Configure

Either use one of existing configurations:

    $ make configure CONFIG_PATH=configurations/ST_32F429IDISCOVERY

#### 3. Build

Build the project with *make*:

    $ make

#### 4. Edit configuration & rebuild

To edit any option in the selected configuration just run *kconfig* tool:

    $ make menuconfig
    ... edit some options, overwrite configuration file ...

You can rebuild the project immediatelly by running *make*:

    $ make

Planes Operations Example description
-------------------------------------

This example presents usage of planes and moving overlay plane over primary plane.
Example can be used with boards with SF-TC240T-9370A-T display panel (like for [32F429IDISCOVERY
(http://www.st.com/en/evaluation-tools/32f429idiscovery.html) board, also known as *STM32F429I-DISCO*
or *STM32F429I-DISC1*).

Display contains ILI9341 driver configured in 4-wire 8 bit data serial MIPI Display Bus Interface.
Initial sequence of the SF-TC240T-9370A-T panel is used to configure display in RGB 24 bit mode
(MIPI Display Pixel Interface).

#### Sequence

1. Picture in ARGB888 pixel format from flash memory is displayed on primary layer.
2. After 5 seconds on overlay layer 128x128 px square with transparent area 64x64 px inside is created.
Plane use AL44 format with 16 bytes Color Look Up Table.
3. Plane is moved to the right-down display corner.
4. When reach down border of the display, overlay plane switch Framebuffer with the square in different color.
5. Overlay plane moving back to the left-up display corner.
