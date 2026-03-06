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
  bool want_plane = false;

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
    } else if (a == "--plane") {
      want_plane = true;
    } else if (a == "--help" || a == "-h") {
      std::cout << "Usage: run_channel_map_api [options]" << std::endl
                << "Options:" << std::endl
                << "  --crate <n>             crate number (default 0)" << std::endl
                << "  --slot <n>              slot number (default 0)" << std::endl
                << "  --stream <n>            stream number (default 0)" << std::endl
                << "  --chan <n>              channel number (default 0)" << std::endl
                << "  --plugin <name>         plugin name (required)" << std::endl
                << "  --channelmapfile <file> channel map file (optional)" << std::endl
                << "  --plane                 also output plane (prints: \"offline_channel plane\")" << std::endl
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

  try {
    auto map = dunedaq::detchannelmaps::make_map(plugin); // plugin is a string from cmdline --plugin, e.g. "ICEBERGChannelMap"
    //(void)channelmapfile; // kept for CLI compatibility; plugins may read it implicitly
    unsigned int off = map->get_offline_channel_from_crate_slot_stream_chan(crate, slot, stream, chan);
    if (want_plane) {
      unsigned int plane = map->get_plane_from_offline_channel(off);
      std::cout << off << " " << plane << std::endl;
    } else {
      std::cout << off << std::endl;
    }
  } catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 2;
  }

  return 0;
}
