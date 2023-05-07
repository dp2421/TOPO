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
public:
    Tile(TileInfo info);
    ~Tile();

    TileInfo data;
    Collider collider;
};

