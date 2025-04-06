#include "Tile.h"

Tile::Tile(TileType tileType, Point3D tileOrigin, BoundingBox3D tileBoundingBox) : Entity(tileOrigin, tileBoundingBox) {
	this->tileType = tileType;
}

bool Tile::isWalkable() const {
	if (tileType == TileType::PELLET || tileType == TileType::EMPTY) return true;
	return false;
}

void Tile::setType(TileType tileType) {
	this->tileType = tileType;
}

TileType Tile::getTileType() const {
	return tileType;
}

std::string Tile::getTileTypeString() {
	switch (tileType) {
	case TileType::EMPTY:  return "EMPTY";
	case TileType::WALL:   return "WALL";
	case TileType::PELLET: return "PELLET";
	default:               return "NONE";
	}
}