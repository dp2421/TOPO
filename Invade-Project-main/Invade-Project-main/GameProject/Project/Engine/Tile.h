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
    LMetor1 = 6,
    LMetor2,
    LMetor3,
    LMetor4,
    LMetor5,
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

    float GetTileZPos() const { return data.zPos; }; //sort �񱳿�

    Vec3 GetTilePos() { return Vec3(data.xPos, data.yPos, data.zPos); };
    Vec3 GetTileScale() { return Vec3(data.xScale, data.yScale, data.zScale); };

    wstring GetPathName(); //for load mdat file, convert enum->wstring
    wstring GetMetorPathName();
    LayerState GetState() { return data.state; };
};