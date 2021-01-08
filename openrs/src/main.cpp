/**
 * This file is part of OpenRS.
 *
 * OpenRS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * OpenRS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with OpenRS. If not, see <https://www.gnu.org/licenses/>.
 */
#include <openrs/database/model.h>
#include <sys/time.h>
#include <unistd.h>

#include <iostream>

#include "openrs/manager/cache/cachemanager.h"
#include "openrs/manager/configmanager.h"
#include "openrs/manager/databasemanager.h"
#include "openrs/manager/worldmanager.h"
#include "openrs/net/reactor.h"

int main() {
  struct timeval start, end;
  long mtime, seconds, useconds;
  bool quit = false;
  constexpr int kTickRate = 600;

  std::cout << "openrs Server" << std::endl;

  if (!openrs::manager::ConfigManager::get()->Init()) {
    std::cerr << "Failed to init config manager." << std::endl;
    return 1;
  }

  if (!openrs::manager::DatabaseManager::get()->Init()) {
    std::cerr << "Failed to init database manager." << std::endl;
    return 1;
  }

  if (!openrs::manager::cache::CacheManager::get()->Init()) {
    std::cerr << "Failed to init cache manager." << std::endl;
    return 1;
  }

  if (!openrs::manager::WorldManager::get()->Init()) {
    std::cerr << "Failed to init world manager." << std::endl;
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
