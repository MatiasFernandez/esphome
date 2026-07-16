#pragma once

#include "esphome/components/climate_ir/climate_ir.h"

#include <cinttypes>

namespace esphome::climate_ir_lg {

// Temperature
const uint8_t TEMP_MIN = 18;  // Celsius
const uint8_t TEMP_MAX = 30;  // Celsius

class LgIrClimate final : public climate_ir::ClimateIR {
 public:
  LgIrClimate()
      : climate_ir::ClimateIR(TEMP_MIN, TEMP_MAX, 1.0f, true, true,
                              {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
                               climate::CLIMATE_FAN_HIGH},
                              {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL}) {}

  /// Override control to change settings of the climate device.
  void control(const climate::ClimateCall &call) override {
    this->send_swing_cmd_ = call.get_swing_mode().has_value();
    // Older units forget swing when powered off (swing returns to off on the next power-on),
    // so reset_swing_when_off (default true) mirrors that. Newer units remember swing across
    // power cycles: set it to false to keep our swing_mode in sync with the hardware.
    auto mode = call.get_mode();
    if (this->reset_swing_when_off_ && mode.has_value() && *mode == climate::CLIMATE_MODE_OFF)
      this->swing_mode = climate::CLIMATE_SWING_OFF;
    climate_ir::ClimateIR::control(call);
  }
  void set_header_high(uint32_t header_high) { this->header_high_ = header_high; }
  void set_header_low(uint32_t header_low) { this->header_low_ = header_low; }
  void set_bit_high(uint32_t bit_high) { this->bit_high_ = bit_high; }
  void set_bit_one_low(uint32_t bit_one_low) { this->bit_one_low_ = bit_one_low; }
  void set_bit_zero_low(uint32_t bit_zero_low) { this->bit_zero_low_ = bit_zero_low; }
  void set_reset_swing_when_off(bool reset_swing_when_off) { this->reset_swing_when_off_ = reset_swing_when_off; }

 protected:
  /// Transmit via IR the state of this climate controller.
  void transmit_state() override;
  /// Handle received IR Buffer
  bool on_receive(remote_base::RemoteReceiveData data) override;

  bool send_swing_cmd_{false};
  bool reset_swing_when_off_{true};

  void calc_checksum_(uint32_t &value);
  void transmit_(uint32_t value);

  uint32_t header_high_;
  uint32_t header_low_;
  uint32_t bit_high_;
  uint32_t bit_one_low_;
  uint32_t bit_zero_low_;

  climate::ClimateMode mode_before_{climate::CLIMATE_MODE_OFF};
};

}  // namespace esphome::climate_ir_lg
