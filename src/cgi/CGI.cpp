#include "CGI.hpp"

/*================ BUILDERS ================*/

// Default Constructor
CGI::CGI() {
	// std::cout << "CGI default constructor called" << std::endl;
}

// Copy Constructor
CGI::CGI(const CGI& other) {
	// std::cout << "CGI copy constructor called" << std::endl;
	*this = other;
}

// Copy Assignment Operator
CGI& CGI::operator=(const CGI& other) {
	// std::cout << "CGI copy assignment operator called" << std::endl;
	if (this != &other) {
		// Copy attributes here
	}
	return *this;
}

// Destructor
CGI::~CGI() {
	// std::cout << "CGI destructor called" << std::endl;
}

void CGI::processInput(std::string input)
{
	// end by calling processInput()
}

void CGI::createSubprocess(const std::string& filepath, const std::string& input)
{
	// checks should be done upstream
	pid_t pid;

	pid = fork();
	if (pid == -1)
		throw std::runtime_error("Forking process failed.");

	if (pid == 0)
	{
		// Child process
		char *args[] = {const_cast<char *>(filepath.c_str()), const_cast<char *>(input.c_str()), NULL};

		execve(args[0], args, NULL); // envp useless?
		// still in child process, execve failed
		// TODO: properly handle error
		throw std::runtime_error("Execution of CGI script failed.");
	}
	else
	{
		// Parent process
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			std::cout << "CGI script exited with status: " << WEXITSTATUS(status) << std::endl;
		else
			std::cout << "CGI script did not exit normally." << std::endl;
	}
}
