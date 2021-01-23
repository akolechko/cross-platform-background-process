#include "pch.h"

#include "utils.h"

#define EN_US 0x0409

void Utils::DisplayError(const std::string& message)
{
	LPSTR error = NULL;

	if (FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			EN_US,
			reinterpret_cast<LPSTR>(&error),
			0,
			NULL) == 0)
	{
		return;
	}

	std::cout << message << ". " << error;
	LocalFree(error);
}

void Utils::DisplayHelp()
{
	std::cout << "\nUsage:\n"
				 "Enter \"install\" to install and start the service.\n"
				 "Enter \"uninstall\" to stop and delete the service.\n"
				 "Enter \"help\" to show help."
			  << std::endl;
}

void Utils::DisplayMessage(const std::string& message)
{
	std::cout << message << ". " << std::endl;
}