#ifndef WIPHYNLCONTROL_STATION_H_
#define WIPHYNLCONTROL_STATION_H_

#include <array>
#include <string>

#include "Entity.h"
#include "Interface.h"
#include "Property.h"

#define STATION_ATTR_NUM 17

namespace wiphynlcontrol {

class Station : public Entity {
 public:
  explicit Station(const uint32_t &id);

 public:
    Property<uint32_t> dev_index_; // Identifier.
 private:
  Property<nested_t> sta_info_;
    Property<nested_t> tx_bitrate_info_; // nested in sta_info_
    Property<nested_t> rx_bitrate_info_; // nested in sta_info_
    Property<nested_t> info_bss_param_; // nested in sta_info_

 public:
  Property<std::string> mac_addr_;
  // Properties nested in sta_info_.
  Property<uint32_t> inactive_time_;
  Property<uint32_t> rx_bytes_;
  Property<uint32_t> tx_bytes_;
  Property<uint32_t> rx_packets_;
  Property<uint32_t> tx_packets_;
  Property<uint64_t> beacon_rx_; // number of beacons received from this peer (u64)
  Property<int8_t>  info_signal_; // signal strength of last received PPDU (u8, dBm)
  Property<uint64_t> info_offset_; // timing offset with respect to this STA (s64)
  Property<uint32_t> tx_retries_;
  Property<uint32_t> tx_failed_;
  Property<uint32_t> beacon_loss_; // count of times beacon loss was detected (u32)
  // Properties nested in tx_bitrate_info_.
  Property<uint32_t> tx_bitrate_;
  // Properties nested in rx_bitrate_info_.
  Property<uint32_t> rx_bitrate_;
  // Properties nested in info_bss_param_.
  Property<bool>     short_preamble_;
  Property<bool>     short_slot_time_;
  Property<uint16_t> beacon_interval_;


 private:
  const std::array<struct Attribute *, STATION_ATTR_NUM> all_attrs;

 public:
  void get();
  // Returns identifier. Overloads abstract method from Entity.
  uint32_t get_identifier() const;
  // Overloads abstract method from Entity.
  void set_identifier(const uint32_t &id);
};

}  // namespace wiphynlcontrol
#endif