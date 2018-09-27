#include <sys/time.h>
#include <unistd.h>

#include <iostream>

#include "OpenRS/net/reactor.h"

int main()
{
    struct timeval start, end;
    long mtime, seconds, useconds;
    bool quit = false;
    constexpr int kTickRate = 600;

    std::cout << "Hello, world!" << std::endl;

    openrs::net::Reactor reactor;

    while (!quit)
    {
        gettimeofday(&start, NULL);

        reactor.Poll();

        // Sleep until the next cycle.
		gettimeofday(&end, NULL);
		seconds = end.tv_sec - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;
		mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;
        if (mtime <= kTickRate)
        {
            usleep((kTickRate - mtime) * 1000);
        }
    }

    return 0;
}
