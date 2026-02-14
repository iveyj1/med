GCC notes
# 0. Setup
add file:
/etc/sysctl.d/10-ptrace.conf
Contents:
kernel.yama.ptrace_scope = 0
Apply:
sudo sysctl --system
For temporary only:
sudo sysctl kernel.yama.ptrace_scope=0

 ---> or run app in window, find process number, and attach to that process in gdb

# 1. Compile for Debug
gcc -g -O0 -fno-omit-frame-pointer -Wall -Wextra -o prog prog.c

Optional sanitizers:
-fsanitize=address
-fsanitize=undefined

#invoke gdb
gdb -tui ./prog

in gdb:
layout src
set pagination off
set confirm off

# 3. Separate Program I/O (Inferior TTY)
In a second terminal (or tmux pane):
tty

Example result:
/dev/pts/5

In GDB:
set inferior-tty /dev/pts/5
run

Interrupt anytime with:
Ctrl-C

# 4. Essential Single-Key Debugging
Ensure source window has focus:
Ctrl-x o

Core keys:
| Key | Action            |
| --- | ----------------- |
| n   | next (step over)  |
| s   | step (into)       |
| c   | continue          |
| f   | finish (step out) |
| u   | until             |
| b   | set breakpoint    |
| r   | run               |

These operate immediately in TUI mode.

# 5. Minimal .gdbinit
Place in `~/.gdbinit`:

set pagination off
set confirm off
set print pretty on
set print elements 0
set disassemble-next-line on

layout src

# Quick aliases
alias nn = next
alias ss = step
alias cc = continue
alias ff = finish
alias bt = backtrace

# Clean restart
alias rr = run

Optional helper for manual TTY use:
define runtty
    if $argc == 1
        set inferior-tty $arg0
        run
    else
        echo Usage: runtty /dev/pts/X\n
    end
end

Usage:

runtty /dev/pts/5

# 6. Useful Inspection Commands
State:
info locals
info args
print var
print/x var
ptype var

Stack:
bt
frame 1
up
down

Memory:
x/16xb ptr
x/10i $pc

# 7. Recommended tmux Layout
Pane 1: nvim
Pane 2: gdb -tui ./prog
Pane 3: inferior tty

Workflow:
1. `tty` in Pane 3
2. `set inferior-tty` in GDB
3. `run`
4. Step in Pane 2
5. Interact in Pane 3

For unbuffered stdout during debug:
setvbuf(stdout, NULL, _IONBF, 0);

# 9. Optional
Break on main automatically:
start

Catch signals:
handle SIGSEGV stop print

Follow forks (if needed):
set follow-fork-mode child


