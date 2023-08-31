# ShikoInventory

Inventory Sorting System project created for GD1P02 Algorithms and Data Structures Assessment 1. 


## Table of Contents

- [Description](#Description)
- [Features](#Features)
- [Requirements](#Requirements)
- [Installation](#Installation)
- [Controls](#Controls)
- [Disclaimer](#Disclaimer)
- [Credits](#Credits)


## Description

ShikoInventory is a small program improved from ShikoSort that allows the user to edit or create custom inventory lists. 
This program is designed with C++ and wxWidgets and to be operated with mouse and keyboard controls. 


## Features
### Loading and Saving
ShikoSort has a loading and saving function for the use of .txt files. 
.txt files may be imported from the local system with the format where:

- NAME = Item name (wxString)
- TYPE = Item type (Enum - Weapon, Armor, Consumable, Utility)
- PRICE = Item price (double)
- QUANTITY = Item Quantity (int)

.TXT (REMOVE THE DOTS)
- NAME // TYPE // PRICE // QUANTITY
- Shiko Sword // Weapon // 10.69 // 1
- Bad Item // Rod // 21.69 // 1                   <- invalid type name (Rod), line will not be used
- Another Bad Item // Utility // 16.45 // 0       <- if item does not exist (0 quantity), why should it be in the list?
- Bad Item 2 // Utility // 44.12 // -5            <- if item does not exist (<1 quantity), why should it be in the list?
- Health Potion // Consumable // 2.99 // 5
-                                                 <- line will be skipped
- Luminescence Cloak // Armor // 15.20 // 1
-   d      Hi           sdf                       <- invalid item format, line will not be used
- Appearance Dust // Utility // 1.98 // 7
- Appearance Dust // Utility // 1.98 // 7         <- duplicate item, line will not be used
- WIngs of Light // Utility // -09.23 // 7        <- invalid price, price must be a double value from 0.00 up
- WIngs of Light // Utility // 16a // 7           <- invalid price, price must be a double value from 0.00 up


Inventories can be saved at their current state to a .txt file too!

### QuickSort
ShikoInventory has QuickSort, allowing you to sort the inventory in ascending or descending order by item parameter NAME, TYPE, PRICE, or QUANTITY! 
NOTE: This feature is currently semi-broken with the GUI implementations. In code the feature works, but when implementing this with the GUI, it has not been consistent.


## Requirements

- Visual Studio 2022 (v143)
- Windows SDK 10.0
- C++ Language Standard: ISO C++20 Standard (/std:c++20)
- C Language Standard: ISO C17 (2018) Standard (/std:c17)
- Windows x64
- Linker > System > SubSystem > Windows (/SUBSYSTEM:WINDOWS)
- wxwidgets.props file to be added as a property file


## Installation

This program can be downloaded and built in x64 with Visual Studio 2022 by opening the solution file.


## Controls

Standard keyboard controls and mouse can be used.


## Disclaimer

This program is incomplete, it is "almost" finished for submission.
As this project is incomplete, the current issues are:
    - Edit function has not been implemented. There have been too many issues with it and will work on it later.
    - Sort function breaks and doesn't work properly unless sorting by Type. You will need to sort by other methods and keep coming back to the desired sort for it to eventually work.
Bugs should be minimal but may be present. 
All code was written by me from my own knowledge from classes and self driven research of the C++ coding language and use of wxWidgets GUI.


## Credits

Shikomisen (Ayoub) 2023
Media Design School
GD1P02 - Algorithms and Data Structures
Written for Assignment 1
