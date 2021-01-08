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
#pragma once

#include <frozen/map.h>
#include <inttypes.h>

#include <map>
#include <set>

namespace openrs {
namespace game {
namespace player {

/**
 * Manages the types of prayer spells are available and which are active.
 */
class PrayerBook {
 public:
  /**
   * The types of prayer books available to the player.
   */
  enum struct BookType : uint8_t { kDefault, kAncient };

  /**
   * The list of possible prayer spells.
   */
  enum struct PrayerType : uint8_t {
    kSkin1 = 0,
    kSkin2 = 5,
    kSkin3 = 13,
  };

 private:
  static inline constexpr frozen::map<PrayerType, float, 3> type_drain_mapping_{
      {PrayerType::kSkin1, 1.2f},
      {PrayerType::kSkin2, 1.2f},
      {PrayerType::kSkin3, 1.2f},
  };

 private:
  std::map<BookType, std::set<PrayerType>> active_prayers_;

 public:
  PrayerBook()
      : active_prayers_({BookType::kDefault : {}, BookType::kAncient : {}}) {}

  /**
   * Checks if a specific prayer is active.
   *
   * @param kType The specific prayer spell to check.
   * @return True if active, false otherwise.
   */
  inline bool IsUsingPrayer(const PrayerType& kType) const {
    for (const auto& kBook : this->active_prayers_) {
      if (this->IsUsingPrayer(kBook.first, kType)) {
        return true;
      }
    }

    return false;
  }

  /**
   * Checks if a specific prayer is active within a specific book.
   *
   * @param kBook The book to check.
   * @param kType The specific prayer spell to check.
   * @return True if active, false otherwise.
   */
  inline bool IsUsingPrayer(const BookType& kBook,
                            const PrayerType& kType) const {
    return this->active_prayers_[kBook].count(kType) > 0;
  }

  inline const auto& active_prayers() const { return this->active_prayers_; }
};

}  // namespace player
}  // namespace game
}  // namespace openrs
