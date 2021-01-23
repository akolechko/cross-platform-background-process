#include "pch.h"

#include "service.h"
#include "commandlinehandler.h"
#include "servicehandler.h"
#include "utils.h"

int main(int argc, char** argv)
{
    auto parser = std::make_unique<CommandLineHandler>(argc, argv);
    bool success = parser->Parse();
    const int return_code = success ? 0 : 1;

    return return_code;
}
