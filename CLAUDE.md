# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a Simple 2D Platformer game project built with raylib. The project is currently in its initial setup phase.

## Expected Development Commands

Since this is a raylib project, the typical development workflow will likely involve:

- Compilation: `gcc -o game main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11` (Linux)
- Or using a Makefile if one is created
- Running: `./game`

## Architecture Notes

This project will implement a 2D platformer game using raylib's graphics and input systems. Key components will likely include:

- Game loop with input handling, update, and render phases
- Player character with physics (gravity, jumping, movement)
- Level/tilemap system for platforms and obstacles
- Collision detection between player and environment
- Game state management (menu, playing, game over, etc.)

## Development Notes

- raylib uses C99 standard conventions
- Game objects typically structured with position, velocity, and state data
- raylib provides built-in functions for drawing, input, and audio
- Screen coordinate system: (0,0) at top-left, Y increases downward