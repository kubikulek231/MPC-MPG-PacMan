#include <GL/glut.h>
#include "Game.h"

Game game = Game(); // Global game instance

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Pacman 3D");

    game.init();
    glutReshapeFunc(Game::reshape);
    glutDisplayFunc(Game::render);
    glutTimerFunc(16, Game::update, 0);
    glutMainLoop();
    return 0;
}