#include <GL/glut.h>
#include "Game.h"

Game game; // Global game instance

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Pacman 3D");

    game.init();

    glutDisplayFunc(Game::display);
    glutReshapeFunc(Game::reshape);
    glutIdleFunc(Game::display); // Keeps updating

    glutMainLoop();
    return 0;
}