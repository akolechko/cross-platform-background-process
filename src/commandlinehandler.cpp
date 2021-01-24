#include "pch.h"

#include "commandlinehandler.h"

#include "servicehandler.h"
#include "utils.h"

CommandLineHandler::CommandLineHandler(int argc, char** argv)
  : m_argc(argc),
    m_argv(argv)
{ }

bool CommandLineHandler::Parse()
{
#ifdef _WIN64
    ServiceParameters parameters;
    auto service = std::make_unique<Service>(parameters);
    auto service_handler = std::make_unique<ServiceHandler>(std::move(service));

    bool success = true;

    do
    {
        if (m_argc == 1)
        {
            success = service_handler->Run();
            break;
        }

	    if (m_argc == 2)
        {
            if (strcmp(m_argv[1], "install") == 0)
            {
                success = service_handler->Install();
                break;
            }

            if (strcmp(m_argv[1], "uninstall") == 0)
            {
                success = service_handler->Uninstall();
                break;
            }

            if (strcmp(m_argv[1], "help") == 0)
            {
                Utils::DisplayHelp();
                break;
            }
        }
        
        Utils::DisplayMessage("Invalid parameters");
        Utils::DisplayHelp();
    }
    while (false);

    return success;
#elif linux
    auto service = std::make_unique<Service>();
    bool success = true;

    do
    {
        if (m_argc == 1)
        {
            success = service->Run();
            break;
        }

        if (m_argc == 2)
        {
            if (strcmp(m_argv[1], "help") == 0)
            {
                Utils::DisplayHelp();
                break;
            }
        }
        
        Utils::DisplayMessage("Invalid parameters");
        Utils::DisplayHelp();
    }
    while (false);

    return success;
#endif
}
