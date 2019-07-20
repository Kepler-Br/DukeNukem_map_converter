#include "mapconverter.h"
#include <vector>
#include <set>

int16_t MapConverter::readint8(std::fstream &file)
{
    int8_t num = 0;
    file.read(reinterpret_cast<char*>(&num), sizeof(int8_t));
    return num;
}

int16_t MapConverter::readint16(std::fstream &file)
{
    int16_t num = 0;
    file.read(reinterpret_cast<char*>(&num), sizeof(int16_t));
    return num;
}

int32_t MapConverter::readint32(std::fstream &file)
{
    int32_t num = 0;
    file.read(reinterpret_cast<char*>(&num), sizeof(int32_t));
    return num;
}

uint8_t MapConverter::readuint8(std::fstream &file)
{
    uint8_t num = 0;
    file.read(reinterpret_cast<char*>(&num), sizeof(uint8_t));
    return num;
}

uint16_t MapConverter::readuint16(std::fstream &file)
{
    uint16_t num = 0;
    file.read(reinterpret_cast<char*>(&num), sizeof(uint16_t));
    return num;
}

uint32_t MapConverter::readuint32(std::fstream &file)
{
    uint32_t num = 0;
    file.read(reinterpret_cast<char*>(&num), sizeof(uint32_t));
    return num;
}

void MapConverter::checkFileFlags(std::fstream &file)
{
    using std::cout;

    if(!file.is_open())
    {
        cout << "Cannot open file.\n";
        return;
    }

    if(file.bad())
    {
        cout << "Cannot open file: bad.\n";
        return;
    }

    int32_t mapVersion = readint32(file);
    if(mapVersion != 7)
    {
        cout << "Map version is not 7.\n";
        return;
    }
}

void MapConverter::readPlayerAttributes(std::fstream &file)
{
    pl.posx  = readint32(file);
    pl.posy  = readint32(file);
    pl.posz  = readint32(file);
    pl.angle = readint16(file);
    pl.startSector = readint16(file);
}

void MapConverter::readSectors(std::fstream &file)
{
    using std::runtime_error;
    using std::ios_base;

    file.seekg(0x14, ios_base::beg);

    int16_t numSectors = readint16(file);
    if(numSectors <= 0)
        throw runtime_error("Negative number of sectors.");

    constexpr int maxSectors = 2<<12;
    if(numSectors > maxSectors)
        throw runtime_error("Maximum sectors count reached(8192).");
    sectors.reserve(numSectors);
    for(int i = 0; i < numSectors; i++)
    {
        sector sec;
        sec.startWall = readint16(file);
        sec.wallNum = readint16(file);
        sec.floorHeigth = readint32(file);
        sec.ceilingHeight = readint32(file);
        int16_t ceilingStat = readint16(file);
        int16_t floorStat = readint16(file);
        sec.ceilingTextureIndex = readint16(file);
        int16_t ceilingSlopeValue = readint16(file);
        int8_t ceilingShade = readint8(file);
        uint8_t ceilingPallete = readint8(file);
        uint8_t ceilingXPanning = readint8(file);
        uint8_t ceilingYPanning = readint8(file);
        sec.floorTextureIndex = readint16(file);
        int16_t floorSlopeValue = readint16(file);
        int8_t floorShade = readint8(file);
        uint8_t floorPallete = readuint8(file);
        uint8_t floorXPanning = readuint8(file);
        uint8_t floorYPanning = readuint8(file);
        uint8_t visibility = readuint8(file);
        readint8(file);
        readint16(file);
        readint16(file);
        readint16(file);
        sectors.push_back(sec);
        if(file.eof())
            throw runtime_error("Unexpected eof.");
    }
}

void MapConverter::readWalls(std::fstream &file)
{
    using std::runtime_error;
    using std::ios_base;

    int16_t numWalls = readint16(file);
    constexpr int maxWalls = 2<<16;
    if(numWalls <= 0)
        throw runtime_error("Negative number of walls.");
    if(numWalls > maxWalls)
        throw runtime_error("Max wall count reached(131072).");
    walls.reserve(numWalls);
    for(int i = 0; i < numWalls; i++)
    {
        wall wll;
        wll.x = readint32(file);
        wll.y = readint32(file);
        wll.point2 = readint16(file);
        int16_t nextWall = readint16(file);
        wll.nextSector = readint16(file);
        int16_t cstat = readint16(file);
        wll.textureIndex = readint16(file);
        int16_t overpicnum = readint16(file);
        int8_t shade = readint8(file);
        uint8_t pallete = readuint8(file);
        wll.repeatX = readuint8(file);
        wll.repeatY = readuint8(file);
        wll.panningX = readuint8(file);
        wll.panningY = readuint8(file);
        readuint16(file);
        readuint16(file);
        readuint16(file);
        walls.push_back(wll);
        if(file.eof())
            throw runtime_error("Unexpected eof.");
    }
}

void MapConverter::writeHeader(std::fstream &file, float coordinateDivider)
{
    file << "// secnum stands for sector number. It defines total number of sectors in the map.\n";
    file << "secnum " << sectors.size() << "\n\n";
    file << "// wallnum stands for wall number. It defines total number of wals in the map.\n";
    file << "wallnum " << walls.size() << "\n\n";
    file << "// pstart stands for player start. It defines where player will be spawn in the level.\n";
    file << "// pstart X Y Z angle start_sector\n";
    constexpr float maxIntAngle = 2047.0f;
    file << "pstart " << static_cast<float>(pl.posx)/coordinateDivider << " " << static_cast<float>(pl.posy)/coordinateDivider << " "
         << static_cast<float>(pl.posz)/coordinateDivider << " " << static_cast<float>(pl.angle)/maxIntAngle*360.0f << " " << pl.startSector << "\n";
    file << "\n";
}

void MapConverter::writeTexturePaths(std::fstream &file)
{
    std::set<uint> textureIndexes;
    for(const auto& wall: walls)
        textureIndexes.insert(wall.textureIndex);
    for(const auto& sector: sectors)
    {
        textureIndexes.insert(sector.floorTextureIndex);
        textureIndexes.insert(sector.ceilingTextureIndex);
    }
    file << "//t stands for texture. It defines level texture paths.\n";
    file << "//t texture_index texture_path.\n";
    const std::string defaultTextureFolder = "./textures/";
    for(const uint textureIndex:textureIndexes)
    {
        file << "t " << textureIndex << " " << defaultTextureFolder << textureIndex << ".bmp" << "\n";
    }
    file << "\n";
}

void MapConverter::writeSectors(std::fstream &file, float coordinateDivider)
{
    file << "// s stands for sector. It defines one sector. Format:\n"
            "//s start_wall_index wall_number floor_height ceiling_height ceiling_texture_index floor_texture_index\n";
    for(const auto & sec : sectors)
    {
        // Because sectors would be really high.
        constexpr float sectorHeigthDivider = 8.0f;
        file << "s "<< sec.startWall << " " << sec.wallNum << " "
             << static_cast<float>(sec.ceilingHeight)/(coordinateDivider*sectorHeigthDivider) << " "
             << static_cast<float>(sec.floorHeigth)/(coordinateDivider*sectorHeigthDivider) << " "
             << sec.ceilingTextureIndex << " " << sec.floorTextureIndex << "\n";
    }
}

void MapConverter::writeWalls(std::fstream &file, float coordinateDivider)
{
    file << "\n// w stands for wall. It defines one wall. Format:\n"
            "//w point_x point_y second_wall_point next_sector(in case if portal > -1)\n"
            "//  texture_index repeatX repeatY panningX panningY\n";

    for(const auto & wll : walls)
    {
        file << "w "<< static_cast<float>(wll.x)/coordinateDivider << " "
             << static_cast<float>(wll.y)/coordinateDivider
             << " " << wll.point2 << " " << wll.nextSector  << " "
             << wll.textureIndex << " " << static_cast<int>(wll.repeatX) << " " << static_cast<int>(wll.repeatY)
             << " " << static_cast<int>(wll.panningX) << " " << static_cast<int>(wll.panningY)  << "\n";
    }
}

void MapConverter::read(const std::string &path)
{
    using std::fstream;
    using std::ios_base;

    std::fstream file;
    file.open(path, std::ios_base::binary | std::ios_base::in);

    checkFileFlags(file);
    readPlayerAttributes(file);
    readSectors(file);
    readWalls(file);
    file.close();
}

void MapConverter::convert(const std::string &path, const float coordinateDivider)
{
    using std::fstream;
    using std::ios_base;

    fstream file(path, ios_base::out);

    writeHeader(file, coordinateDivider);
    writeTexturePaths(file);
    writeSectors(file, coordinateDivider);
    writeWalls(file, coordinateDivider);

    file.close();
}
