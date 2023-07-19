#pragma once

enum LayerState : int
{
    //레이싱맵
    L2Part0 = 6,
    L2Part1,
    L2Part2,
    LJinggum,
    L2Part4,
    L2Part5,
    L2Part6,
    L2Part7,
    L2Part8,
    L1Water,
    L1Part1,
    L1Part2,
    L1Part3,
    L1Part4,
    L1Sujum,
    LCoin,

    //줄넘기맵
    LGrass,
    LWood,
    LAsphalt,

    //시상식 (25~)
    LAward1,
    LAward2,
    LAward3,
    LAwardGrs,
    LAwardBk,
    LAwardTr,

    // 타일 레이어 추가시 아래에 추가
};

enum MetorLayerState : int
{
    LMetorCenter = 6,
    LMetorStone,
    LMetorWater,
    LMetorGrass,
    LMetorWood,
};

struct TileInfo
{
    float xPos;
    float yPos;
    float zPos;
    float xScale;
    float yScale;
    float zScale;
    LayerState state;
};

struct MetorTile
{
    float xPos;
    float yPos;
    float zPos;
    float xScale;
    float yScale;
    float zScale;
    MetorLayerState state;
};
class Tile
{
private:
    TileInfo data;
    MetorTile Mdata;
public:
    Tile(TileInfo info);
    Tile(MetorTile info);
    ~Tile();

    float GetTileZPos() const { return data.zPos; }; //sort 비교용

    Vec3 GetTilePos() { return Vec3(data.xPos, data.yPos, data.zPos); };
    Vec3 GetMetorTilePos() { return Vec3(Mdata.xPos, Mdata.yPos, Mdata.zPos); }
    Vec3 GetTileScale() { return Vec3(data.xScale, data.yScale, data.zScale); };
    Vec3 GetTileScaleV2() { return Vec3(data.xScale, data.zScale, data.yScale); };

    wstring GetPathName(); //for load mdat file, convert enum->wstring
    wstring GetMetorPathName();
    LayerState GetState() { return data.state; };
};