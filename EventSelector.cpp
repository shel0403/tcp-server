#include "EventSelector.h"
#include <sys/select.h>
#include <errno.h>

EventSelector::~EventSelector()
{
    if (fd_array)
    {
        delete[] fd_array;
    }
}

void EventSelector::Add(FdHandler* h)
{
    int fd = h->GetFd();

    if (!fd_array)
    {
        fd_array_len = fd > 15 ? fd + 1 : 16;
        fd_array = new FdHandler*[fd_array_len];

        for (int i = 0; i < fd_array_len; i++)
        {
            fd_array[i] = 0;
        }

        max_fd = -1;
    }

    if (fd_array_len <= fd)
    {
        FdHandler** tmp = new FdHandler*[fd + 1];

        for (int i = 0; i <= fd; i++)
        {
            tmp[i] = i < fd_array_len ? fd_array[i] : 0;
        }

        fd_array_len = fd + 1;
        delete[] fd_array;
        fd_array = tmp;
    }

    if (fd > max_fd)
    {
        max_fd = fd;
    }

    fd_array[fd] = h;
}

bool EventSelector::Remove(FdHandler* h)
{
    int fd = h->GetFd();

    if (fd >= fd_array_len || fd_array[fd] != h)
    {
        return false;
    }

    fd_array[fd] = 0;

    if (fd == max_fd)
    {
        while (max_fd >= 0 && !fd_array[max_fd])
        {
            max_fd--;
        }
    }

    return true;
}

void EventSelector::Run()
{
    quit_flag = false;

    do
    {
        fd_set rds, wrs;
        FD_ZERO(&rds);
        FD_ZERO(&wrs);

        for (int i = 0; i <= max_fd; i++)
        {
            if (fd_array[i] && fd_array[i]->WantRead())
            {
                FD_SET(i, &rds);
            }
            else if (fd_array[i] && fd_array[i]->WantWrite())
            {
                FD_SET(i, &wrs);
            }
        }

        int res = select(max_fd + 1, &rds, &wrs, 0, 0);

        if (res < 0 && errno == EINTR)
        {
            continue;
        }
        else if (res < 0)
        {
            break;
        }

        if (res > 0)
        {
            for (int i = 0; i <= max_fd; i++)
            {
                if (!fd_array[i])
                {
                    continue;
                }

                bool r = FD_ISSET(i, &rds);
                bool w = FD_ISSET(i, &wrs);

                if (r || w)
                {
                    fd_array[i]->Handle(r, w);
                }
            }
        }
    } while (!quit_flag);
}
