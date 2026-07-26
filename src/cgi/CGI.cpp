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

}
