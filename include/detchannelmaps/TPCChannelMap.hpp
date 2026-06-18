#ifndef DETCHANNELMAPS_INCLUDE_DETCHANNELMAPS_TPCCHANNELMAP_HPP_
#define DETCHANNELMAPS_INCLUDE_DETCHANNELMAPS_TPCCHANNELMAP_HPP_

#include <optional>
#include <stdexcept>
#include <string>
#include <memory>

#define DEFINE_DUNE_DET_CHANNEL_MAP(klass) /* no-op */

namespace dunedaq {
namespace detchannelmaps {

class TPCChannelMap
{
public:

  struct TPCCoords
  {
    uint crate;
    uint slot;
    uint fiber;
    uint channel;
  };

  /**
   * @brief      Gets the offline channel from detector elements.
   *
   * @param[in]  crate        The crate
   * @param[in]  slot         The slot
   * @param[in]  fiber        The fiber
   * @param[in]  fembchannel  The channel
   *
   * @return     The offline channel from detector elements.
   */
  virtual uint get_offline_channel_from_crate_slot_fiber_chan(uint crate, uint slot, uint fiber, uint channel) = 0;
  virtual uint get_offline_channel_from_crate_slot_stream_chan(uint crate, uint slot, uint stream, uint channel) {

    //if stream number looks wrong (not 0,1,2,3 or 64,65,66,67)
    if( (stream & 0xbc) ) throw std::runtime_error("Invalid stream number " + std::to_string(stream));
    
    constexpr uint n_chan_per_stream = 64;

    uint link = (stream >> 6) & 1;
    uint locstream = (stream & 0x3);
    uint ch = n_chan_per_stream*locstream+channel;
    return this->get_offline_channel_from_crate_slot_fiber_chan(crate, slot, link, ch);

  };
  virtual uint get_plane_from_offline_channel(uint offchannel) = 0;
  virtual std::string get_tpc_element_from_offline_channel(uint ) { return ""; }
  virtual std::optional<TPCCoords> get_crate_slot_fiber_chan_from_offline_channel(uint offchannel) = 0;
  /**
   * @brief TPCChannelMap destructor
   */
  virtual ~TPCChannelMap() noexcept = default;
    
protected:
   /*
   * @brief TPCChannelMap Constructor
   * @param name Name of the TPCChannelMap
   */
  explicit TPCChannelMap(){}
};

} // namespace detchannelmaps
} // namespace dunedaq

#endif // DETCHANNELMAPS_INCLUDE_DETCHANNELMAPS_TPCCHANNELMAP_HPP_
