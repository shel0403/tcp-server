#include "FdHandler.h"
#include <unistd.h>

FdHandler::~FdHandler()
{
    if (own_fd)
    {
        close(own_fd);
    }
}
