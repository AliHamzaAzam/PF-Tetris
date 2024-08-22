# PF-Tetris

This is a Tetris game developed using SFML as the Programming Fundamentals (CS1002) course project.

## Table of Contents
- [Game Description](#game-description)
- [Features](#features)
- [Gameplay](#gameplay)
- [Build Instructions](#build-instructions)
- [How to Play](#how-to-play)
- [Screenshots](#screenshots)
- [License](#license)

## Game Description

The game is a simple implementation of the classic Tetris where players earn points by clearing horizontal lines. The players can control the movement of the Tetriminos in the game matrix. The game ends when the blocks stack up to the top of the screen.

## Features

- **Matrix Playing Field:** All blocks and lines are enclosed within the matrix.
- **Random Shape Generation:** The game randomly introduces one of the seven different Tetrimino shapes.
- **Movement, Rotation, and Dropping:** Use arrow keys for movement and rotation of blocks.
- **Shadow of Dropping Shapes:** Shows where the block will land before it drops.
- **Bomb Feature:** Randomly falling bombs can destroy blocks, clearing the entire matrix if colors match.
- **Scoring System:**
  - Score scales with the number of lines cleared simultaneously.
- **Game Levels:**
  - Speed: Block speed increases every five horizontal lines cleared
- **High Scores:** Tracks and displays the top 10 high scores.
- **Menu System:** Options for starting a new game, pausing a running game, viewing high scores, and accessing help.

## Gameplay

The game starts with blocks falling from the top. The player can move these blocks left, right, or down and rotate them to fit into the available spaces. The objective is to complete horizontal lines without gaps, which will then disappear and score points. The game ends if the blocks reach the top of the playing field.

## Build Instructions

1. Ensure that you have [SFML](https://www.sfml-dev.org/download.php) and CMake version 3.28 or higher installed on your system.
2. Clone the repository:
    ```bash
    git clone https://github.com/AliHamzaAzam/PF-Tetris.git
    cd PF-Tetris
    ```
3. Build the project using CMake:
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```
4. Run the game:
    ```bash
    ./PF_Tetris
    ```
    
