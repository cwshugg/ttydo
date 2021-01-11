```
  █  █        █
▀▀█▀▀█▀▀█  █▀▀█▀▀█    ===========================
  █  █  █▄▄█▄▄█▄▄█    A Command-Line ToDo Tracker
  ▄▄▄▄▄ ▄  █ ▄▄▄▄▄    ===========================
        ▀▀▀▀
```
I use the command line all the time. Git, Vim, shell scripting, you name it. While I use other external tools to keep track of large tasks and goals (such as [Notion](https://www.notion.so/)), I always find myself finding ways to keep track of smaller tasks without having to leave the terminal. (Such as: `echo "TODO: 1. <this> 2. <that> 3. <profit>"`, and `git commit -m "finished <this>. TODO NEXT: <that>"`.)

This inspired me to look into creating a command-line ToDo tracker. While there are some great solutions already out in the world, I was itching to write some C code after a college semester of Rust, JavaScript, and Java. `ttydo` is the result.

# Installation and Usage
To install, run `make install` (with or without `sudo` as necessary). This should build the program and move it to `/usr/local/bin`. If you'd like to install it in another location, run `make install INSTALL_LOCATION=<path>`.

Using ttydo is pretty simple. Extensive 'help' menus are displayed for every possible command. To view them, execute `ttydo help`. Each command has sub-commands, such as `task add` or `task delete`. These can be viewed by adding `help` after the command name (such as `task help`).

# Task Storage
To store task lists, ttydo attempts to create and write files to `~/.ttydo`. These `.tasklist` files are in plaintext and in a comma-separated format.

# Example
Here's an example of what a single task list in ttydo might look like:
```
┌─ This Afternoon's Code ────────────────────────────────────────────────────────────────────────────┐
│ [ ] Fix header file: Fix <whatever>.h's function definitions to use uint16_ts instead of ints.     │
│ [X] Implement search/sort: Implement a way to search for <whatevers> and sort them by <whatever>.  │
│ [ ] Create PR: Create a pull request once all the above tasks are done.                            │
├─ Progress ─────────────────────────────────────────────────────────────────────────────────────────┤
│ 33%  ├██████████████████████████████─────────────────────────────────────────────────────────────┤ │
└────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

# Future Goals
These are things that I'd like to implement into `ttydo`:

- Add the ability for tasks to have @tags
  - Let tasks be searchable/displayed by tags
- Add convenience commands
  - Reorder tasks

If you happen to stumble across this repo and have ideas of your own, feel free to share them with me, or open up a pull request!
