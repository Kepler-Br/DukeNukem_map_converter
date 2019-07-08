#ifndef MAPCONVERTER_H
#define MAPCONVERTER_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>



class MapConverter
{
    struct sector
    {
        int16_t startWall;
        int16_t wallNum;
        int32_t ceilingHeight;
        int32_t floorHeigth;
        int16_t ceilingTextureIndex;
        int16_t floorTextureIndex;
    };

    struct wall
    {
        int32_t x, y;
        int16_t point2;
        int16_t nextSector;
        int16_t textureIndex;
        uint8_t repeatX, repeatY;
        uint8_t panningX, panningY;
    };

    struct player
    {
        int32_t posx, posy, posz;
        int16_t angle, startSector;
    };

    int16_t readint8(std::fstream &file);
    int16_t readint16(std::fstream &file);
    int32_t readint32(std::fstream &file);

    uint8_t readuint8(std::fstream &file);
    uint16_t readuint16(std::fstream &file);
    uint32_t readuint32(std::fstream &file);

    void checkFileFlags(std::fstream &file);
    void readPlayerAttributes(std::fstream &file);
    void readSectors(std::fstream &file);
    void readWalls(std::fstream &file);

    void writeHeader(std::fstream &file, float coordinateDivider);
    void writeSectors(std::fstream &file, float coordinateDivider);
    void writeWalls(std::fstream &file, float coordinateDivider);


    player pl;
    std::vector<sector> sectors;
    std::vector<wall> walls;

public:

    MapConverter()
    {

    }

    void read(const std::string &path);

    void convert(const std::string &path);
};

#endif // MAPCONVERTER_H
