#include "pch.h"
#include "Tile.h"
#include "SimpleMath.h"

Tile::Tile(TileInfo info)
{
	data.xPos = info.xPos;
	data.yPos = info.yPos;
	data.zPos = info.zPos;
	data.xScale = info.xScale;
	data.yScale = info.yScale;
	data.zScale = info.zScale;
	data.state = info.state;
}

Tile::Tile(MetorTile info)
{
    Mdata.xPos = info.xPos;
    Mdata.yPos = info.yPos;
    Mdata.zPos = info.zPos;
    Mdata.xScale = info.xScale;
    Mdata.yScale = info.yScale;
    Mdata.zScale = info.zScale;
    Mdata.state = info.state;
}


Tile::~Tile()
{

}

wstring Tile::GetPathName()
{
	static const wstring layerStrings[] = {
        L"MeshData\\L2Part0.mdat",
        L"MeshData\\L2Part1.mdat",
        L"MeshData\\L2Part2.mdat",
        L"MeshData\\LJinggum.mdat",
        L"MeshData\\L2Part4.mdat",
        L"MeshData\\L2Part5.mdat",
        L"MeshData\\L2Part6.mdat",
        L"MeshData\\L2Part7.mdat",
        L"MeshData\\L2Part8.mdat",
        L"MeshData\\L1Water.mdat",
        L"MeshData\\L1Part1.mdat",
        L"MeshData\\L1Part2.mdat",
        L"MeshData\\L1Part3.mdat",
        L"MeshData\\L1Part4.mdat",
        L"MeshData\\L1Sujum.mdat",
        L"MeshData\\LCoin.mdat",
        L"MeshData\\LGrass.mdat",
        L"MeshData\\LWood.mdat",
        L"MeshData\\LAsphalt.mdat",
        L"MeshData\\LAward1.mdat",
        L"MeshData\\LAward2.mdat",
        L"MeshData\\LAward3.mdat",
        L"MeshData\\LAwardGrs.mdat",
        L"MeshData\\LAwardBk.mdat",
        L"MeshData\\LAwardTr.mdat",

        //Ÿ�� ���̾� �߰��� �Ʒ��� �߰� (���̾� ���۹�ȣ �ٸ��� ���� �����ص���
	};

    return layerStrings[data.state-6]; //enum LayerState�� 6���� �����ϹǷ� 6 ���ֱ�
}

wstring Tile::GetMetorPathName()
{
    static const wstring layerStrings[] = {
        L"MeshData\\LMetorCenter.mdat",
        L"MeshData\\LMetorStone.mdat",
        L"MeshData\\LMetorWater.mdat",
        L"MeshData\\LMetorGrass.mdat",
        L"MeshData\\LMetorWood.mdat",

        //Ÿ�� ���̾� �߰��� �Ʒ��� �߰� (���̾� ���۹�ȣ �ٸ��� ���� �����ص���
    };

    return layerStrings[Mdata.state - 6]; //enum LayerState�� 6���� �����ϹǷ� 6 ���ֱ�
}

