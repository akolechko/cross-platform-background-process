#pragma once

class CommandLineHandler
{
public:
	CommandLineHandler(int argc, char** argv);
	bool Parse();

private:
	int m_argc;
	char** m_argv;
};
