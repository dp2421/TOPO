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
public:
    Tile(TileInfo info);
    Tile(MetorTile info);
    ~Tile();

    TileInfo data;
    MetorTile Mdata;

    Collider collider;
};

