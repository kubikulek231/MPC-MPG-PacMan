struct EntityPosition {
    float x;
    float y;

    EntityPosition(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    void move(float dx, float dy) {
        x += dx;
        y += dy;
    }
};

class Entity {
protected:
    EntityPosition position;

public:
    Entity(float x = 0.0f, float y = 0.0f) : position(x, y) {}

    EntityPosition getPosition() const {
        return position;
    }

    void setPosition(float x, float y) {
        position.setPosition(x, y);
    }

    void move(float dx, float dy) {
        position.move(dx, dy);
    }
};