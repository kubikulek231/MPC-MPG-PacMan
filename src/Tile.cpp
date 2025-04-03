#include "Tile.h"

Tile::Tile(TileType tileType, float xPosMin, float yPosMin, float xPosMax, float yPosMax) {
	this->tileType = tileType;
	this->xPosMin = xPosMin;
	this->yPosMin = yPosMin;
	this->xPosMax = xPosMax;
	this->yPosMax = yPosMax;
	this->xCenterPos = xPosMax - xPosMin;
	this->yCenterPos = yPosMax - yPosMin;
}

bool Tile::isWalkable() {
	if (tileType == TileType::PELLET || tileType == TileType::EMPTY) return true;
}

void Tile::setType(TileType tileType) {
	this->tileType = tileType;
}

float Tile::getCenterX() {
	return xCenterPos;
}

float Tile::getCenterY() {
	return yCenterPos;
}

TileType Tile::getTileType() {
	return tileType;
}