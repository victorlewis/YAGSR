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
    // into arrays of 62 floats, and then transfer each into a RichPoint since you can't read directly into a struct due to packing.
    // allocate a buffer to read into
    float* buffer = new float[splats->numSplats * 62];
    reader.read((char*)buffer, splats->numSplats * 62 * sizeof(float));
    //reader.read((char*)splats->splats.data(), splats->numSplats * sizeof(RichPoint<45>));
    if (reader.eof())
    {
        std::cerr << "Reader is EOF?" << std::endl;
        splats->valid = false;
        return std::move(splats);
    }
    // copy the buffer into the splats
    for (int i = 0; i < splats->numSplats; i++)
	{
		RichPoint<45> rp;
        rp.pos[0] = buffer[i * 62 + 0];
		rp.pos[1] = buffer[i * 62 + 1];
        rp.pos[2] = buffer[i * 62 + 2];

        rp.n[0] = buffer[i * 62 + 3];
        rp.n[1] = buffer[i * 62 + 4];
        rp.n[2] = buffer[i * 62 + 5];

        rp.dc[0] = buffer[i * 62 + 6];
        rp.dc[1] = buffer[i * 62 + 7];
        rp.dc[2] = buffer[i * 62 + 8];

        for (int j = 0; j < 45; j++) rp.shs[j] = buffer[i * 62 + 9 + j]; 

        rp.opacity = buffer[i * 62 + 54];

		rp.scale[0] = buffer[i * 62 + 55];
		rp.scale[1] = buffer[i * 62 + 56];
		rp.scale[2] = buffer[i * 62 + 57];

		rp.rot[0] = buffer[i * 62 + 58];
		rp.rot[1] = buffer[i * 62 + 59];
		rp.rot[2] = buffer[i * 62 + 60];
		rp.rot[3] = buffer[i * 62 + 61];

		splats->splats[i] = rp;
	}
    splats->valid = true;

    delete[] buffer;

    return std::move(splats);
}