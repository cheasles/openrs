#include <sys/time.h>
#include <unistd.h>

#include <iostream>

#include "openrs/manager/cache/cachemanager.h"
#include "openrs/manager/configmanager.h"
#include "openrs/manager/databasemanager.h"
#include "openrs/net/reactor.h"

int main() {
  struct timeval start, end;
  long mtime, seconds, useconds;
  bool quit = false;
  constexpr int kTickRate = 600;

  std::cout << "openrs Server" << std::endl;

  if (!openrs::manager::ConfigManager::get().Init()) {
    std::cerr << "Failed to init config manager." << std::endl;
    return 1;
  }

  if (!openrs::manager::DatabaseManager::get().Init()) {
    std::cerr << "Failed to init database manager." << std::endl;
    return 1;
  }

  if (!openrs::manager::cache::CacheManager::get().Init()) {
    std::cerr << "Failed to init cache manager." << std::endl;
    return 1;
  }

  try {
    openrs::net::Reactor reactor;

    while (!quit) {
      gettimeofday(&start, NULL);

      reactor.Poll();

      // Sleep until the next cycle.
      gettimeofday(&end, NULL);
      seconds = end.tv_sec - start.tv_sec;
      useconds = end.tv_usec - start.tv_usec;
      mtime = ((seconds)*1000 + useconds / 1000.0) + 0.5;
      if (mtime <= kTickRate) {
        usleep((kTickRate - mtime) * 1000);
      }
    }
  } catch (const std::runtime_error& ex) {
    std::cerr << "Failed to bind to port:" << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
