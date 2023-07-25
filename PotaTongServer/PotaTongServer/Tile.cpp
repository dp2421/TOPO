#include "pch.h"
#include "Tile.h"

Tile::Tile(TileInfo info)
{
	data.xPos = roundf(info.xPos * 10) / 10;
	data.yPos = info.yPos;
	data.zPos = roundf(info.zPos * 10) / 10;
	data.xScale = info.xScale;
	data.yScale = info.yScale;
	data.zScale = info.zScale;
	data.state = info.state;

	collider.position = new Vector3(data.xPos, data.yPos, data.zPos);
	collider.offset = TileOffset;
	collider.size = TileCollider * Vector3(data.xScale, data.yScale, data.zScale);
}

Tile::Tile(MetorTile info)
{
}

Tile::~Tile()
{

}