#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(800, 600);
    glutCreateWindow("PacMan3D");

    // Singleton static instance
    Game& game = Game::getInstance();
    game.init();

    glutReshapeFunc(Game::reshape);
    glutDisplayFunc(Game::render);
    glutTimerFunc(16, Game::update, 0);
    glutMainLoop();
    return 0;
}