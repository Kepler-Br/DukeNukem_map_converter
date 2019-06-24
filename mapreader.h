#ifndef MAPREADER_H
#define MAPREADER_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>

struct sector
{
    int16_t startWall;
    int16_t wallNum;
    int32_t ceilingHeight;
    int32_t floorHeigth;
};

struct wall
{
    int32_t x, y;
    int16_t point2;
    int16_t nextWall;
    int16_t nextSector;
};

class MapReader
{
    bool isFileOpened = false;
    std::fstream mapFile;

    int16_t readint16();
    int32_t readint32();

    std::vector<sector> sectors;
    std::vector<wall> walls;

public:

    MapReader()
    {

    }

    void open(const std::string &path);

    void convert(const std::string &path);
};

#endif // MAPREADER_H
