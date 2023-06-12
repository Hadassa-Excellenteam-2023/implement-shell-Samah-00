#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#include <string>
#include <vector>


struct Job {
    std::string command;
    int pid;
    bool isRunning;

    Job(const std::string& cmd, int processId, bool running);
};

std::vector<std::string> split(const std::string& input, char delimiter);
void executeChildProcess(const std::string& commandPath, const std::vector<std::string>& arguments,
    const std::string& inputFile, const std::string& outputFile);
void executeCommand(const std::string& commandPath, const std::vector<std::string>& arguments,
    const std::string& inputFile, const std::string& outputFile, bool runInBackground);
void updateJobStatus(int pid, bool isRunning);
void printCommandDetails(const Job& job);
void executeMyJobsCommand();
void executeCdCommand(const std::vector<std::string>& tokens);
std::string searchPathForCommand(const std::string& commandPath, const char* pathEnv);
void handleCommand(const std::string& command);

#endif