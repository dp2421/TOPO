#pragma once

enum LayerState : int
{
    //���̸̽�
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

    //�ٳѱ��
    LGrass,
    LWood,
    LAsphalt,

    //�û�� (25~)
    LAward1,
    LAward2,
    LAward3,
    LAwardGrs,
    LAwardBk,
    LAwardTr,

    // Ÿ�� ���̾� �߰��� �Ʒ��� �߰�
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

