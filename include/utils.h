#pragma once

namespace Utils
{
    
#ifdef _WIN64
void DisplayError(const std::string& message);
#endif
void DisplayMessage(const std::string& message);
void DisplayHelp();

};
