#include "openrs/manager/appearancemanager.h"

#include <openrs/common/io/buffer.h>

void openrs::manager::AppearanceManager::GetPlayerAppearance(
    const std::shared_ptr<openrs::game::Player>& kPlayer,
    openrs::common::io::Buffer<>* buffer) {
  uint8_t flag = 0;

  // Total length, empty for now.
  buffer->PutData<uint8_t>(0);
  uint8_t* length_ptr = buffer->data() + buffer->position() - 1;
  const auto kStartingOffset = buffer->position();

  buffer->PutData(flag);
  buffer->PutData<uint8_t>(kPlayer->HasSkull() ? 0 : -1);
  buffer->PutData<uint8_t>(-1);
  buffer->PutData<uint8_t>(0);

  // Hat.
  buffer->PutData<uint8_t>(0);
  // Cape.
  buffer->PutData<uint8_t>(0);
  // Amulet.
  buffer->PutData<uint8_t>(0);
  // Weapon.
  buffer->PutData<uint8_t>(0);

  // Chest.
  buffer->PutDataBE<uint16_t>(
      0x0100 + kPlayer->GetAppearanceLook(
                   openrs::game::player::Appearance::BodyPart::kTorso));
  // Shield.
  buffer->PutData<uint8_t>(0);

  // Arms.
  buffer->PutDataBE<uint16_t>(
      0x0100 + kPlayer->GetAppearanceLook(
                   openrs::game::player::Appearance::BodyPart::kArms));
  // Legs.
  buffer->PutDataBE<uint16_t>(
      0x0100 + kPlayer->GetAppearanceLook(
                   openrs::game::player::Appearance::BodyPart::kLegs));
  // Hands.
  buffer->PutDataBE<uint16_t>(
      0x0100 + kPlayer->GetAppearanceLook(
                   openrs::game::player::Appearance::BodyPart::kForearms));
  // Feet.
  buffer->PutDataBE<uint16_t>(
      0x0100 + kPlayer->GetAppearanceLook(
                   openrs::game::player::Appearance::BodyPart::kBoots));

  // Beard.
  buffer->PutDataBE<uint16_t>(
      0x0100 + kPlayer->GetAppearanceLook(
                   openrs::game::player::Appearance::BodyPart::kBeard));

  // Aura.
  buffer->PutData<uint8_t>(0);

  // Slot data length.
  buffer->PutDataBE<uint16_t>(0);

  // Hash.
  buffer->PutDataBE<uint16_t>(0);

  // Colours.
  buffer->PutData<uint8_t>(kPlayer->GetAppearanceColour(
      openrs::game::player::Appearance::BodyPart::kHair));
  buffer->PutData<uint8_t>(kPlayer->GetAppearanceColour(
      openrs::game::player::Appearance::BodyPart::kTorso));
  buffer->PutData<uint8_t>(kPlayer->GetAppearanceColour(
      openrs::game::player::Appearance::BodyPart::kLegs));
  buffer->PutData<uint8_t>(kPlayer->GetAppearanceColour(
      openrs::game::player::Appearance::BodyPart::kBoots));
  buffer->PutData<uint8_t>(kPlayer->GetAppearanceColour(
      openrs::game::player::Appearance::BodyPart::kSkin));
  buffer->PutData<uint8_t>(0);
  buffer->PutData<uint8_t>(0);
  buffer->PutData<uint8_t>(0);
  buffer->PutData<uint8_t>(0);
  buffer->PutData<uint8_t>(0);

  // Render emote.
  buffer->PutDataBE<uint16_t>(0);

  buffer->PutString(kPlayer->username);

  // Combat levels.
  buffer->PutData<uint8_t>(kPlayer->GetCombatLevel());
  buffer->PutData<uint8_t>(kPlayer->GetCombatLevel());

  buffer->PutData<uint8_t>(-1);
  buffer->PutData<uint8_t>(0);

  *length_ptr = static_cast<uint8_t>(buffer->position() - kStartingOffset);
}
