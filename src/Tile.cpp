#include "Tile.h"
#include "MapFactory.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "GameLighting.h"


Tile::Tile(TileType tileType, Point3D tileOrigin, BoundingBox3D tileBoundingBox, int tileRow, int tileCol) : Entity(tileOrigin, tileBoundingBox) {
	this->tileRow = tileRow;
	this->tileCol = tileCol;
	this->tileType = tileType;
}

bool Tile::collectPellet() {
	if (tileType == TileType::PELLET) {
		this->setTileType(TileType::EMPTY);
		return true;
	}
	return false;
}

bool Tile::isWalkable() const {
	if (tileType == TileType::DOOR_CLOSED || tileType == TileType::WALL || tileType == TileType::EMPTY_UNWALKABLE) return false;
	return true;
}

void Tile::setTileType(TileType tileType) {
	this->tileType = tileType;
}

TileType Tile::getTileType() const {
	return tileType;
}

bool Tile::isGhostHouseTile() const {
	if (tileType == TileType::GHOST_HOUSE ||
		tileType == TileType::SPAWN_CLYDE ||
		tileType == TileType::DOOR_CLOSED ||
		tileType == TileType::DOOR_OPEN) {
		return true;
	}
	return false;
}

Point3D Tile::getCenterPoint() const {
	const auto& absBB = getAbsoluteBoundingBox();
	const auto& min = absBB.min;
	const auto& max = absBB.max;

	float centerX = (min.x + max.x) / 2.0f;
	float centerZ = (min.z + max.z) / 2.0f;

	return Point3D(centerX, 0.0f, centerZ);  // No need for Y
}

float Tile::distanceToCenter(const Tile& other) const {
	Point3D a = getCenterPoint();
	Point3D b = other.getCenterPoint();

	float dx = a.x - b.x;
	float dz = a.z - b.z;

	return std::sqrt(dx * dx + dz * dz);
}

std::string Tile::getTileTypeString() {
	switch (tileType) {
	case TileType::EMPTY:         return "EMPTY";
	case TileType::WALL:          return "WALL";
	case TileType::PELLET:        return "PELLET";
	case TileType::GHOST_HOUSE:   return "GHOST_HOUSE";
	case TileType::DOOR_OPEN:     return "DOOR_OPEN";
	case TileType::DOOR_CLOSED:   return "DOOR_CLOSED";
	case TileType::TELEPORT:      return "TELEPORT";
	case TileType::SPAWN_PLAYER:  return "SPAWN_PLAYER";
	case TileType::SPAWN_INKY:    return "SPAWN_INKY";
	case TileType::SPAWN_BLINKY:  return "SPAWN_BLINKY";
	case TileType::SPAWN_PINKY:   return "SPAWN_PINKY";
	case TileType::SPAWN_CLYDE:   return "SPAWN_CLYDE";
	default:                      return "UNKNOWN";
	}
}


void Tile::renderHighlight() const {
	BoundingBox3D abb = this->getAbsoluteBoundingBox();

	// Set the color for highlighting
	glColor4f(highlightR, highlightG, highlightB, highlightA);

	// Render the plane just above the floor to prevent clipping
	glBegin(GL_QUADS);
		glVertex3f(abb.min.x, abb.min.y + 0.01f, abb.min.z); // Bottom-left
		glVertex3f(abb.max.x, abb.min.y + 0.01f, abb.min.z); // Bottom-right
		glVertex3f(abb.max.x, abb.min.y + 0.01f, abb.max.z); // Top-right
		glVertex3f(abb.min.x, abb.min.y + 0.01f, abb.max.z); // Top-left
	glEnd();
}

void Tile::renderEmpty() const {
	BoundingBox3D abb = this->getAbsoluteBoundingBox();
	float COLOR[3] = { 0.0f, 0.0f, 0.0f };
	GLfloat LIGHT_AMBIENT[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat LIGHT_DIFFUSE[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
	GLfloat LIGHT_SPECULAR[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat LIGHT_EMISSION[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat LIGHT_SHININESS = 0.0f;

	GameLighting::setMaterial(GL_FRONT, LIGHT_AMBIENT, LIGHT_DIFFUSE, LIGHT_SPECULAR, LIGHT_EMISSION, LIGHT_SHININESS);

	glBegin(GL_QUADS);
		glNormal3f(0, 1, 0); // Up-facing surface
		glVertex3f(abb.min.x, abb.min.y, abb.min.z);
		glVertex3f(abb.max.x, abb.min.y, abb.min.z);
		glVertex3f(abb.max.x, abb.min.y, abb.max.z);
		glVertex3f(abb.min.x, abb.min.y, abb.max.z);
	glEnd();

	GameLighting::resetMaterial(GL_FRONT);
}

void Tile::renderPellet() const {
	BoundingBox3D abb = this->getAbsoluteBoundingBox();
	GLfloat LIGHT_AMBIENT[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat LIGHT_DIFFUSE[4] = { 0.8f, 0.3f, 0.0f, 1.0f };
	GLfloat LIGHT_SPECULAR[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat LIGHT_EMISSION[4] = { 0.3f, 0.05f, 0.0f, 1.0f };
	GLfloat LIGHT_SHININESS = 32.0f;

	float centerX = (abb.min.x + abb.max.x) / 2.0f;
	float centerY = abb.min.y + MapFactory::TILE_SIZE / 2.0f;
	float centerZ = (abb.min.z + abb.max.z) / 2.0f;

	GameLighting::setMaterial(GL_FRONT_AND_BACK, LIGHT_AMBIENT, LIGHT_DIFFUSE, LIGHT_SPECULAR, LIGHT_EMISSION, LIGHT_SHININESS);

	glPushMatrix();
	glTranslatef(centerX, centerY, centerZ);
	glutSolidSphere(MapFactory::TILE_SIZE / 8.0, 16, 16);  // Has normals by default
	glPopMatrix();

	GameLighting::resetMaterial(GL_FRONT_AND_BACK);
}

void Tile::renderDoorOpen() const {
	BoundingBox3D abb = this->getAbsoluteBoundingBox();

	// Dark brown material with no shine
	GLfloat LIGHT_AMBIENT[4] = { 0.1f, 0.05f, 0.025f, 1.0f };
	GLfloat LIGHT_DIFFUSE[4] = { 0.3f, 0.15f, 0.05f, 1.0f }; // Dark brown
	GLfloat LIGHT_SPECULAR[4] = { 0.0f, 0.0f, 0.0f, 1.0f };   // No specular
	GLfloat LIGHT_EMISSION[4] = { 0.0f, 0.0f, 0.0f, 1.0f };   // No glow
	GLfloat LIGHT_SHININESS = 0.0f;

	GameLighting::setMaterial(GL_FRONT_AND_BACK, LIGHT_AMBIENT, LIGHT_DIFFUSE, LIGHT_SPECULAR, LIGHT_EMISSION, LIGHT_SHININESS);

	float centerX = (abb.min.x + abb.max.x) / 2.0f;
	float centerY = (abb.min.y + abb.max.y) / 2.0f;
	float centerZ = (abb.min.z + abb.max.z) / 2.0f + MapFactory::TILE_SIZE * 0.25f;

	float width = MapFactory::TILE_SIZE;
	float height = MapFactory::TILE_SIZE;
	float depth = MapFactory::TILE_SIZE / 8;

	glPushMatrix();
	glTranslatef(centerX, centerY, centerZ);
	glScalef(width, height, depth); // scale unit cube into a door block
	glutSolidCube(1.0f);
	glPopMatrix();

	GameLighting::resetMaterial(GL_FRONT_AND_BACK);
}


void Tile::render() const {
	if (highlight) {
		renderHighlight();
	}

	switch (tileType) {
	case TileType::PELLET:
		renderEmpty();
		renderPellet();
		break;
	case TileType::EMPTY:
	case TileType::GHOST_HOUSE:
		renderEmpty();
		break;
	case TileType::DOOR_OPEN:
		renderDoorOpen();
		renderEmpty();
		break;
	case TileType::DOOR_CLOSED:
	case TileType::TELEPORT:
	case TileType::SPAWN_PLAYER:
	case TileType::SPAWN_INKY:
	case TileType::SPAWN_BLINKY:
	case TileType::SPAWN_PINKY:
	case TileType::SPAWN_CLYDE:
		renderEmpty();
		break;
	default:
		break;
	}
}


// Getter and Setter for neighboring tiles
Tile* Tile::getTileUp() const {
	return tileUp;
}

Tile* Tile::getTileDown() const {
	return tileDown;
}

Tile* Tile::getTileLeft() const {
	return tileLeft;
}

Tile* Tile::getTileRight() const {
	return tileRight;
}

int Tile::getTileRow() const {
	return tileRow;
}

int Tile::getTileCol() const {
	return tileCol;
}

void Tile::setTileUp(Tile* tile) {
	tileUp = tile;
}

void Tile::setTileDown(Tile* tile) {
	tileDown = tile;
}

void Tile::setTileLeft(Tile* tile) {
	tileLeft = tile;
}

void Tile::setTileRight(Tile* tile) {
	tileRight = tile;
}

void Tile::setHighlight(bool value) {
	this->highlight = value;
}

Tile* Tile::getTileInMoveDir(MoveDir moveDir) const {
	if (moveDir == MoveDir::BWD) { return tileDown; }
	if (moveDir == MoveDir::FWD) { return tileUp; }
	if (moveDir == MoveDir::LEFT) { return tileLeft; }
	if (moveDir == MoveDir::RIGHT) { return tileRight; }
	return nullptr;
}

void Tile::setHighlightColor(float r, float g, float b, float a) {
	this->highlightR = r;
	this->highlightG = g;
	this->highlightB = b;
	this->highlightA = a;
}



std::string Tile::toString() {
	BoundingBox3D abb = this->getAbsoluteBoundingBox();

	// Calculate the center of the tile
	float textX = (abb.min.x + abb.max.x) / 2.0f;
	float textY = abb.min.y;
	float textZ = (abb.min.z + abb.max.z) / 2.0f;

	// Prepare the coordinate string for output
	std::ostringstream oss;
	oss << "("
		<< std::fixed << std::setprecision(2) << abb.min.x << ","
		<< std::fixed << std::setprecision(2) << abb.min.z
		<< ")";

	std::string coordStr = oss.str();

	// Create the final string with tile coordinates and tile type
	std::ostringstream result;
	result << "Tile at (" << textX << ", " << textY << ", " << textZ
		<< ") - Coordinates: " << coordStr
		<< " - Type: " << this->getTileTypeString();  // Add tileType here

	// Return the generated string
	return result.str();
}

bool Tile::isEqual(const Tile* other) const {
	if (other == nullptr) { return false; }
	if (this->getTileCol() == other->getTileCol() && this->getTileRow() == other->getTileRow()) {
		return true;
	}
	return false;
}

bool Tile::isNeighbor(const Tile* other) const {
	if (other == nullptr) { return false; }
	if (this->isEqual(other)) { return false; }
	if (this->isEqual(other->getTileUp())) { return true; }
	if (this->isEqual(other->getTileDown())) { return true; }
	if (this->isEqual(other->getTileLeft())) { return true; }
	if (this->isEqual(other->getTileRight())) { return true; }
	return false;
}