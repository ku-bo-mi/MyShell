# My Shell

My Shell is a custom Unix-like shell implemented in C, designed to demonstrate my system programming skills, including process management, fork, and system calls. This project provides users with a basic yet functional command-line interface.

## Features

- **Full Shell Experience**: My Shell supports a range of standard shell features, making it a useful tool for everyday tasks.
- **Command Execution**: It creates child processes to execute user-entered commands, following standard Unix shell behavior.
- **Redirection**: Users can redirect standard input and output using `<` and `>` symbols, enhancing the shell's capabilities.
- **Pipes**: My Shell supports piping (`|`), enabling users to chain multiple commands together, just like in Unix shells.

## Getting Started

1. Clone this repository to your local machine:
   ```bash
   git clone https://github.com/yourusername/my-shell.git
   ```
2. Compile the code:
   ```
   make
   ```
3. Run the shell:
   ```
   ./my-shell
   ```

## Usage
1. Start the shell by running `./my-shell`.

2. Enter commands just like you would in a regular Unix shell.

3. Use `<` to redirect standard input from a file:
   ```
   cat < input.txt
   ```
4. Use `>` to redirect standard output to a file:
   ```
   ls > output.txt
   ```
5. Use `|` to pipe the output of one command into another:
   ```
   ls | grep txt
   ```
6. Use all to run a complex pipeline:
   ```
   cat < input.txt | cat > output.txt // copy the contents of input.txt to output.txt
   ```
6. Press `Ctrl + C` to interrupt a running command.

7. Use `Ctrl + D` to exit the shell.

## Skills

This project serves as a showcase of my system programming skills and proficiency in various areas, including:

- **Process Management**: I utilize the `fork()` system call to create child processes, allowing for concurrent execution of commands.

- **Command Execution**: The project demonstrates my ability to execute external programs with arguments using the `execvp()` system call, ensuring seamless command execution.

- **Child Process Handling**: I handle child process termination efficiently with the `wait()` system call, ensuring proper resource management and process synchronization.

- **Inter-Process Communication (IPC)**: My Shell incorporates IPC through the use of pipes created with the `pipe()` system call, facilitating data transfer between processes.

- **File Descriptor Management**: I manage file descriptors effectively, employing functions like `dup2()`, `read()`, and `write()` to control input and output redirection.

- **File I/O**: The project showcases my proficiency in file input and output, leveraging `open()` and `close()` system calls for file operations.

These skills collectively demonstrate my competence in system programming and my ability to build a functional Unix-like shell with advanced features.


## Author

- [Kubomi](https://github.com/ku-bo-mi/)

Feel free to reach out to me for questions, suggestions, or collaborations related to this project. You can also explore my GitHub profile for more of my work and projects.
