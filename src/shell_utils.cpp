#include "shell_utils.h"
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// ---- c-tor for struct Job -----
Job::Job(const std::string& cmd, int processId, bool running)
    : command(cmd), pid(processId), isRunning(running) {
}

std::vector<Job> backgroundJobs; // Vector to store background jobs

// ----- Function definitions -----
// Splits a string into a vector of tokens based on a delimiter
std::vector<std::string> split(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(input);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Executes a child process with the given command path and arguments
void executeChildProcess(const std::string& commandPath, const std::vector<std::string>& arguments) {
    std::vector<char*> args;
    args.push_back(const_cast<char*>(commandPath.c_str())); // First argument is the command itself

    // Prepare arguments for execvp
    for (const auto& arg : arguments) {
        args.push_back(const_cast<char*>(arg.c_str()));
    }
    args.push_back(nullptr); // Last argument must be nullptr

    // Execute the command
    if (execvp(commandPath.c_str(), args.data()) == -1) {
        std::cerr << "Failed to execute command." << std::endl;
        exit(EXIT_FAILURE);	// In case execvp fails
    }
}

// Executes a command with the given command path, arguments, and background execution flag
void executeCommand(const std::string& commandPath, const std::vector<std::string>& arguments, bool runInBackground) {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Fork failed." << std::endl;
        return;
    }
    if (pid == 0) {
        // Child process
        executeChildProcess(commandPath, arguments);
    }
    else {
        // Parent process
        if (!runInBackground) {
            int status;
            waitpid(pid, &status, 0); // Wait for the child process to complete
            // Update the job status based on the process exit status
            bool isRunning = !WIFEXITED(status) || (WEXITSTATUS(status) == 0);
            updateJobStatus(pid, isRunning);
        }
        else {
            // Add the child process details (pid, command, etc.) to a list of background jobs
            Job job(commandPath, pid, true);
            backgroundJobs.push_back(job);
        }
    }
}

// Update the job status based on the process ID
void updateJobStatus(int pid, bool isRunning) {
    for (Job& job : backgroundJobs) {
        if (job.pid == pid) {
            job.isRunning = isRunning;
            break;
        }
    }
}

// Prints the details of a job (command, PID, status)
void printCommandDetails(const Job& job) {
    std::cout << "Command: " << job.command << std::endl;
    std::cout << "PID: " << job.pid << std::endl;
    std::cout << "Status: " << (job.isRunning ? "Running" : "Finished") << std::endl;
    std::cout << std::endl;
}

// Executes the 'myjobs' command to display information about background jobs
void executeMyJobsCommand() {
    if (backgroundJobs.empty()) {
        std::cout << "No background jobs." << std::endl;
        return;
    }

    // Iterate over the background jobs and print their details
    for (const Job& job : backgroundJobs) {
        printCommandDetails(job);
    }
}

// Executes the 'cd' command to change the current working directory
void executeCdCommand(const std::vector<std::string>& tokens) {
    if (tokens.size() > 1) {
        if (chdir(tokens[1].c_str()) != 0) {
            std::cerr << "Failed to change directory." << std::endl;
        }
    }
    else {
        std::cerr << "Missing argument for cd command." << std::endl;
    }
}

// Searches the PATH environment variable for the given command and returns the full path if found
std::string searchPathForCommand(const std::string& commandPath, const char* pathEnv) {
    std::string pathString(pathEnv);
    std::vector<std::string> paths = split(pathString, ':');
    for (const std::string& path : paths) {
        std::string fullPath = path + '/' + commandPath;
        if (access(fullPath.c_str(), X_OK) == 0) {
            return fullPath;
        }
    }
    return commandPath; // Return the original commandPath if not found in any path
}

// Handles a shell command by executing it or performing a built-in shell command
void handleCommand(const std::string& command) {
    std::vector<std::string> tokens = split(command, ' ');
    if (tokens.empty()) {
        return;
    }
    // Handle built-in shell commands (exit, cd, myjobs)
    if (tokens[0] == "exit") {
        exit(EXIT_SUCCESS);
    }
    else if (tokens[0] == "cd") {
        executeCdCommand(tokens);
    }
    else if (tokens[0] == "myjobs") {
        executeMyJobsCommand();
    }
    else {
        bool runInBackground = false;
        if (!tokens.empty() && tokens.back() == "&") {
            runInBackground = true;
            tokens.pop_back(); // Remove the '&' character from the arguments
        }

        std::string commandPath = tokens[0];
        if (commandPath[0] != '/') {
            // Not a full path, search for the software in the paths listed in the PATH environment variable
            char* pathEnv = getenv("PATH");
            if (pathEnv != nullptr) {
                commandPath = searchPathForCommand(commandPath, pathEnv);
            }
        }

        // Pass the arguments vector to the executeCommand function
        std::vector<std::string> arguments;
        if (tokens.size() > 1) {
            arguments.assign(tokens.begin() + 1, tokens.end());
        }
        executeCommand(commandPath, arguments, runInBackground);

    }
}
