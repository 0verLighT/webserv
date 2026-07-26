#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>



class CGI
{
public:
	/*================ BUILDERS ================*/

	// Default Constructor
	CGI();
	// Copy Constructor
	CGI(const CGI& other);
	// Copy Assignment Operator
	CGI& operator=(const CGI& other);
	// Destructor
	~CGI();

	/*================ MEMBERS ================*/

	void	processInput(std::string input);
	void	createSubprocess(const std::string& filename, const std::string& args);
};

#endif
