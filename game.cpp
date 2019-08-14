#include "precomp.h" // include (only) this in every .cpp file

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	// Create renderer
	shared_ptr<Renderer> r(new Renderer);
	renderer = r;
	renderer->Init(screen);
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{
}

//static Sprite rotatingGun( new Surface( "assets/aagun.tga" ), 36 );
static int frame = 0;

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	// clear the graphics window
	screen->Clear( 0 );

	// Render screen
	renderer->Render();

	// print something to the text window
	//printf( "this goes to the console window.\n" );

	// draw a sprite
	//rotatingGun.SetFrame( frame );
	//rotatingGun.Draw( screen, 100, 100 );

	if (++frame == 36) frame = 0;
}

void Game::KeyDown(int key)
{
	renderer->HandleInput(key);
}