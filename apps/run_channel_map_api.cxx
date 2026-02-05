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
  std::string plugin;
  std::string channelmapfile;

  for (int i = 1; i < argc; ++i) {
    std::string a(argv[i]);
    if (a == "--crate" && i + 1 < argc) {
      crate = std::stoul(argv[++i]);
    } else if (a == "--slot" && i + 1 < argc) {
      slot = std::stoul(argv[++i]);
    } else if (a == "--stream" && i + 1 < argc) {
      stream = std::stoul(argv[++i]);
    } else if (a == "--chan" && i + 1 < argc) {
      chan = std::stoul(argv[++i]);
    } else if (a == "--plugin" && i + 1 < argc) {
      plugin = argv[++i];
    } else if (a == "--channelmapfile" && i + 1 < argc) {
      channelmapfile = argv[++i];
    } else {
      std::cerr << "Unknown or incomplete option: " << a << std::endl;
      return 1;
    }
  }

  if (plugin.empty()) {
    std::cerr << "--plugin <plugin> is required" << std::endl;
    return 1;
  }

  try {
    auto map = dunedaq::detchannelmaps::make_map(plugin);
    (void)channelmapfile; // kept for CLI compatibility; plugins may read it implicitly
    unsigned int off = map->get_offline_channel_from_crate_slot_stream_chan(crate, slot, stream, chan);
    std::cout << off << std::endl;
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 2;
  }

  return 0;
}
