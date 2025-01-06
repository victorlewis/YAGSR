#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>


template<int D>
struct RichPoint
{
    float pos[3];
    float n[3];
    float dc[3];
    float shs[D];
    float opacity;
    float scale[3];
    float rot[4];
};

struct GSSplats
{
    bool valid; // Are the in-memory splats valid?
    int numSplats; // How many splats there are?
    std::vector<RichPoint<45>> splats;
};

std::unique_ptr<GSSplats> loadFromSplatsPly(const std::string& path)
{
    std::unique_ptr<GSSplats> splats = std::make_unique<GSSplats>();
    splats->numSplats = 0;
    splats->valid = false;

    std::ifstream reader(path, std::ios::binary);
    if (!reader.good())
    {
        std::cerr << "Bad PLY reader: " << path << "?" << std::endl;
        return std::move(splats);
    }

    // Get the headers out of the way
    std::string buf;
    std::getline(reader, buf);
    std::getline(reader, buf);
    std::getline(reader, buf);
    std::stringstream ss(buf);
    std::string dummy;

    // Read the number of splats and resize the `splats` array
    ss >> dummy >> dummy >> splats->numSplats;
    splats->splats.resize(splats->numSplats);
    std::cout << "Loading " << splats->numSplats << " splats.." << std::endl;

    while (std::getline(reader, dummy))
    {
        if (dummy.compare("end_header") == 0)
        {
            break;
        }
    }

    // Read the whole thing into memory. "The lot", as they say.
    reader.read((char*)splats->splats.data(), splats->numSplats * sizeof(RichPoint<45>));
    if (reader.eof())
    {
        std::cerr << "Reader is EOF?" << std::endl;
        splats->valid = false;
        return std::move(splats);
    }
    splats->valid = true;

    return std::move(splats);
}