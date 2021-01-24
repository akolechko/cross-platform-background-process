#pragma once

#include <iostream>
#include <cassert>
#include <thread>

#ifdef _WIN64
#include <atlstr.h>
#endif

#ifdef linux
#include <cstring>
#include <csignal>

#include <iostream>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
