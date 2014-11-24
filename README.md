MTW_Marlin
==========

Project based on a snapshot of a Makers Tool Works Marlin directory

Intial purpose is to add I2C LED controller support

November 2014: LED event support for MendelMax 3 release

NOTE: This repo is my personal working directory, so the Configuration.h and
Configuration_adv.h and pins.h have values set for my own printers that differ
from the MTW defaults. All the LED user options are at the bottom of Configuration.h
so it's possible to just copy that portrion into your own Configuration.h then
overwrite those 3 files with your own copies if your version of Marlin is close enough.

The files touched by the LED code are:

Configuration.h
Marlin_main.cpp
temperature.cpp
mtwled.h
mtwled.cpp

