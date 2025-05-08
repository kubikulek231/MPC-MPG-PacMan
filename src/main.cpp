#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"
#include "resource.h"
#include <windows.h>

// This gets called by the Windows loader
// Global variable for storing the application window handle
HWND hWnd;

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(800, 600);
    glutCreateWindow("PacMan3D");


#ifdef _WIN32
    // Set the window icon
    HWND hWnd = FindWindowA(NULL, "PacMan3D");
    if (hWnd) {
        // Load ICO resource
        HICON hIcon = static_cast<HICON>(LoadImageA(
            GetModuleHandle(NULL),
            MAKEINTRESOURCEA(IDI_PACMAN_ICON),
            IMAGE_ICON,
            0, 0,               // 0,0 tells Windows to pick the best size
            LR_DEFAULTSIZE | LR_SHARED
        ));
        if (hIcon) {
            // Replace both the large and small icons
            SetClassLongPtrA(hWnd, GCLP_HICON, (LONG_PTR)hIcon);
            SetClassLongPtrA(hWnd, GCLP_HICONSM, (LONG_PTR)hIcon);
        }
    }
#endif

    // Singleton static instance
    Game& game = Game::getInstance();

    glutReshapeFunc(Game::reshape);
    glutDisplayFunc(Game::render);
    glutTimerFunc(16, Game::update, 0);
    glutMainLoop();
    return 0;
}

// This gets called by the Windows loader
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmdLine, int cmdShow) {
    // Fallback into your real main()
    return main(__argc, __argv);
    return 0;
}