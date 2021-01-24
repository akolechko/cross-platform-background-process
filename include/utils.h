#pragma once

namespace Utils
{
    
#ifdef WIN32
void DisplayError(const std::string& message);
#endif
void DisplayMessage(const std::string& message);
void DisplayHelp();

};
