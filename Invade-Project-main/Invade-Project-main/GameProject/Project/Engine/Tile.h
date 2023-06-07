#pragma once

enum LayerState : int
{
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
    L1Part4
    // 타일 레이어 추가시 아래에 추가
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

class Tile
{
private:
    TileInfo data;
public:
    Tile(TileInfo info);
    ~Tile();

    float GetTileZPos() const { return data.zPos; }; //sort 비교용
    Vec3 GetTilePos() { return Vec3(data.xPos, data.yPos, data.zPos); };
    wstring GetLayerName(); //for load mdat file, convert enum->wstring
    int GetState() { return data.state; };
};