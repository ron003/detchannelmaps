/**
 * @file run_channel_map_api.cxx
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <exception>

#include "detchannelmaps/TPCChannelMap.hpp"

int
main(int argc, char **argv)
{
  unsigned int crate = 0;
  unsigned int slot = 0;
  unsigned int stream = 0;
  unsigned int chan = 0;
  unsigned int off_chan = 0;
  std::string plugin;
  std::string channelmapfile;
  bool want_plane = false;
  bool has_crate = false;
  bool has_slot = false;
  bool has_stream = false;
  bool has_chan = false;
  bool has_off_chan = false;

  for (int i = 1; i < argc; ++i) {
    std::string a(argv[i]);
    if (a == "--crate" && i + 1 < argc) {
      crate = std::stoul(argv[++i]);
      has_crate = true;
    } else if (a == "--slot" && i + 1 < argc) {
      slot = std::stoul(argv[++i]);
      has_slot = true;
    } else if (a == "--stream" && i + 1 < argc) {
      stream = std::stoul(argv[++i]);
      has_stream = true;
    } else if (a == "--chan" && i + 1 < argc) {
      chan = std::stoul(argv[++i]);
      has_chan = true;
    } else if (a == "--off-chan" && i + 1 < argc) {
      off_chan = std::stoul(argv[++i]);
      has_off_chan = true;
    } else if (a == "--plugin" && i + 1 < argc) {
      plugin = argv[++i];
    } else if (a == "--channelmapfile" && i + 1 < argc) {
      channelmapfile = argv[++i];
    } else if (a == "--plane") {
      want_plane = true;
    } else if (a == "--help" || a == "-h") {
      std::cout << "Usage: run_channel_map_api [options]" << std::endl
                << "Options:" << std::endl
                << "  --crate <n>             crate number (default 0)" << std::endl
                << "  --slot <n>              slot number (default 0)" << std::endl
                << "  --stream <n>            stream number (default 0)" << std::endl
                << "  --chan <n>              channel number (default 0)" << std::endl
                << "  --off-chan <n>          offline channel for reverse lookup" << std::endl
                << "  --plugin <name>         plugin name (required)" << std::endl
                << "  --channelmapfile <file> channel map file (optional)" << std::endl
                << "  --plane                 also output plane (forward: \"offline_channel plane\", reverse: \"crate slot stream chan plane\")" << std::endl
                << "  --help, -h              show this help message" << std::endl;
      return 0;
    } else {
      std::cerr << "Unknown or incomplete option: " << a << std::endl;
      return 1;
    }
  }

  if (plugin.empty()) {
    std::cerr << "--plugin <plugin> is required" << std::endl;
    return 1;
  }

  if (has_off_chan && (has_crate || has_slot || has_stream || has_chan)) {
    std::cerr << "--off-chan is mutually exclusive with --crate/--slot/--stream/--chan" << std::endl;
    return 1;
  }

  try {
    auto map = dunedaq::detchannelmaps::make_map(plugin);
    (void)channelmapfile; // kept for CLI compatibility; plugins may read it implicitly

    if (has_off_chan) {
      auto coords = map->get_crate_slot_fiber_chan_from_offline_channel(off_chan);
      if (!coords.has_value()) {
        std::cerr << "No hardware coordinates found for offline channel " << off_chan << std::endl;
        return 2;
      }

      constexpr unsigned int n_chan_per_stream = 64;
      const unsigned int out_stream = ((coords->fiber & 0x1U) << 6) | ((coords->channel / n_chan_per_stream) & 0x3U);
      const unsigned int out_chan = coords->channel % n_chan_per_stream;

      if (want_plane) {
        unsigned int plane = map->get_plane_from_offline_channel(off_chan);
        std::cout << coords->crate << " " << coords->slot << " " << out_stream << " " << out_chan << " " << plane << std::endl;
      } else {
        std::cout << coords->crate << " " << coords->slot << " " << out_stream << " " << out_chan << std::endl;
      }
    } else {
      unsigned int off = map->get_offline_channel_from_crate_slot_stream_chan(crate, slot, stream, chan);
      if (want_plane) {
        unsigned int plane = map->get_plane_from_offline_channel(off);
        std::cout << off << " " << plane << std::endl;
      } else {
        std::cout << off << std::endl;
      }
    }
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 2;
  }

  return 0;
}
