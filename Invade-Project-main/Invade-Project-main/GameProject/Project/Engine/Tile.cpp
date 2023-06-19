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

Tile::~Tile()
{

}

wstring Tile::GetLayerName()
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
        L"MeshData\\L1Part4.mdat"  
        //Ÿ�� ���̾� �߰��� �Ʒ��� �ݵ�� �߰�
	};

    return layerStrings[data.state-6]; //enum LayerState�� 6���� �����ϹǷ� 6 ���ֱ�
}
