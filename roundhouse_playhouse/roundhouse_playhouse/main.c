
#include "gba.h"
#include "screenmode.h"
#include "keypad.h"
#include "gfx.h"
#include "assets\background.h"
#include "assets\titlescreen.h"
#include "assets\player_still.h"
//include headers that contain the sprites

//memory location definers
u16* FrontBuffer = (u16*)0x6000000;
u16* BackBuffer = (u16*)0x600A000;
u16* videoBuffer;
volatile u16* ScanlineCounter = (volatile u16*)0x4000006;

u16* paletteMem = (u16*)0x5000000;

enum FighterStates
{
	blocking,
	still,
	attacking,
	moving,
	dead,
	stunned
};

//drawing functions
void PlotPixel(int x, int y, unsigned short int c)
{
	//set video buffer to draw static images
	videoBuffer[(y)* 120 + (x)] = (c);
}

void PlotPixelTransparent(int x, int y, unsigned short int c)
{
	//set video buffer to draw sprites and remove white space
	unsigned short int temp;
	if ((c & 0x00FF) == 0)
	{
		if ((c & 0xFF00) == 0)
		{
			return;
		}
		temp = ((videoBuffer[(y)* 120 + (x)]) & 0x00FF);
		temp |= c;
		videoBuffer[(y)* 120 + (x)] = (temp);
	}
	else if ((c & 0xFF00) == 0)
	{
		temp = ((videoBuffer[(y)* 120 + (x)]) & 0xFF00);
		temp |= c;
		videoBuffer[(y)* 120 + (x)] = (temp);
	}
	else
	{
		videoBuffer[(y)* 120 + (x)];
	}
}

void WaitForStart(void)
{
	//waits for the user to start the game
	while (1)
	{
		if (!((*KEYS) & KEY_START))
		{
			return;
		}
	}
}

void Flip(void)
{
	//flips between the front and back buffer
	if (REG_DISPCNT & BACKBUFFER)
	{
		REG_DISPCNT &= ~BACKBUFFER;
		videoBuffer = BackBuffer;
	}
	else
	{
		REG_DISPCNT |= BACKBUFFER;
		videoBuffer = FrontBuffer;
	}
}

void WaitForVblank(void)
{
	//waits for the buffer to get fully drawn before flipping to next buffer
	while (*ScanlineCounter < 160)
	{
	}
}

void EraseScreen(void)
{
	//erase the screen
	int x, y;

	for (y = 0; y < 160; y++)
	{
		for (x = 0; x < 120; x++)
		{
			PlotPixel(x, y, 0x0000);
		}
	}

	WaitForVblank();
	Flip();
}

void DoIntro(void)
{
	//go through the game intro before starting
	int x, y;
	for (x = 0; x < 256; x++)
	{
		paletteMem[x] = titlescreenPalette[x];
	}

	for (y = 0; y < 160; y++)
	{
		for (x = 0; x < 120; x++)
		{
			PlotPixel(x, y, titlescreenData[y * 120 + x]);
		}
	}
	WaitForVblank();
	Flip();
	//Sleep(50);

	WaitForStart();
}

void DrawBackground(void)
{
	//draws the background image when the game is playing
	int x, y;

	for (y = 0; y < 160; y++)
	{
		for (x = 0; x < 120; x++)
		{
			PlotPixel(x, y, backgroundData[y * 120 + x]);
		}
	}
}

void ChangeBackPal(void)
{
	//sets the palette memory to the image palette <---------------------------------------
}

void DrawPlayerLife(int pLife)
{
	//draws the players life bar
	int x, y;
	for (x = 6; x < (pLife); x += 2)
	{
		for (y = 5; y < 14; y++)
		{
			PlotPixel(x / 2, y, 0xBCBC);
		}
	}
}

void DrawOpponentsLife(int oLife)
{
	//draws the opponents life bar
	int x, y;
	for (x = 235; x > (241 - oLife); x -= 2)
	{
		for (y = 5; y < 14; y++)
		{
			PlotPixel(x / 2, y, 0xBCBC);
		}
	}
}

void DrawPlayer(int xplayer, int yplayer, int direction)
{
	//draws the player
	int x, y;
	
	for (y = 0; y < player_still_HEIGHT; y++)
	{
		for (x = 0; x < (player_still_WIDTH/2); x++)
		{
			PlotPixel(x+xplayer, y+yplayer, player_stillData[y * (player_still_WIDTH / 2) + x]);
		}
	}
	
}

void DrawOpponent(int x, int y, int direciton)
{
	//draws the opponent
}

void DrawTime(int time)
{
	//draws the time
}

void DrawLost(void)
{
	//draws the screen when the player loses
}

void DrawWin(void)
{
	//draws the screen when the player wins
}



//main function that controls the game loop
int main(void)
{
	SetMode(MODE_4 | BG2_ENABLE);

	EraseScreen();
	DoIntro();

	while (1)
	{
		//stays in this loop until exiting game
		EraseScreen();
		//everything is setup at the start of the loop until going into the next loop which controls manages the game in play mode
		int pl_life; //life is 1 to 100

		int op_life; //life is 1 to 200

		int playx, playy; //x and y position of the player

		int oppx, oppy; //x an y position of the opponent

		int time;

		//controls what state the player and opponent
		enum FighterStates pl_state = still;
		enum FighterStates opp_state = still;

		playx = 60;
		playy = 155 - player_still_HEIGHT;
		pl_life = 0;
		op_life = 0;
		time = 60;

		ChangeBackPal();

		while (pl_life < 100)
		{

			DrawBackground();
			DrawPlayerLife(pl_life);
			DrawOpponentsLife(op_life);
			op_life++;
			pl_life++;

			WaitForVblank();
			Flip();
		}

		while ((pl_state != dead) || (opp_state != dead))
		{
			// prolly want to do a screen flip here for speed????
			// anyhow method is as follows
			// 1. draw the bg
			// 2. draw the bombs, life, tank
			// 3. flip it to the screen
			// check for user input
			// move the bombs
			// check for collision
			// subtract from life

			DrawBackground();
			DrawPlayerLife(pl_life);
			DrawOpponentsLife(op_life);
			//DrawTime(time);
			//DrawOpponent(oppx, oppy, 0);
			DrawPlayer(playx, playy, 0);
			WaitForVblank();
			Flip();

			//player movement and subsequent opponent response

			if (!((*KEYS) & KEY_RIGHT))
			{
				playx += 2;
			}
			if (!((*KEYS) & KEY_LEFT))
			{
				playx -= 2;
			}
			
			//boundarycheck for player and opponent

			if (playx < 0)
			{
				playx = 0;
			}
			//if (playx > ())

			//control deltatime
		}

		
		WaitForVblank();
		Flip();
		//draw, win or loss depending game result

	}

}

