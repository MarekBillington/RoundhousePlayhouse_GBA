
#include "gba.h"
#include "screenmode.h"
#include "keypad.h"
#include "gfx.h"
#include "assets\background.h"
#include "assets\titlescreen.h"
#include "assets\player_still.h"
#include "assets\opponent_still.h"
#include "assets\fight.h"
#include "assets\win.h"
#include "assets\lose.h"
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
	if ((c & 0xFFFF) == 0) // bottom is transparent
	{
		if ((c & 0xFFFF) == 0) // top is also transparent
			return;
		// so bottom transparent, top is not so....
		temp = ((videoBuffer[(y)* 120 + (x)]) & 0xFFFF);
		temp |= c; // or it with c (to get the highpart of c in
		videoBuffer[(y)* 120 + (x)] = (temp);
	}
	else
	if ((c & 0xFFFF) == 0) // only the top is transprent
	{
		temp = ((videoBuffer[(y)* 120 + (x)]) & 0xFFFF);
		temp |= c;
		videoBuffer[(y)* 120 + (x)] = (temp);
	}
	else
	{
		videoBuffer[(y)* 120 + (x)] = (c);
	}
}

//declare functions
void WaitForStart(void);
void Flip(void);
void WaitForVblank(void);
void EraseScreen(void);
void DoIntro(void);
void DrawBackground(void);
void DrawPlayerLife(int pLife);
void DrawOpponentsLife(int oLife);

void DrawPlayerStill(int xplayer, int yplayer, int direction);
void DrawPlayerBlock(int xplayer, int yplayer, int direction);
void DrawPlayerAttack(int xplayer, int yplayer, int direction, int frame);

void DrawOpponentStill(int x, int y, int direciton);
void DrawOpponentBlock(int x, int y, int direciton);
void DrawOpponentAttack(int x, int y, int direciton, int frame);

void DrawLost(void);
void DrawWin(void);
void DrawFight(void);

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

		int rnum;

		int op_frames, pl_frames;

		//controls what state the player and opponent
		enum FighterStates pl_state = still;
		enum FighterStates op_state = still;

		playx = 60;
		playy = 155 - player_still_HEIGHT;
		oppx = 30;
		oppy = 130 - opponent_still_HEIGHT;
		pl_life = 0;
		op_life = 0;

		//ChangeBackPal();

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

		DrawBackground();
		DrawPlayerLife(pl_life);
		DrawOpponentsLife(op_life);
		DrawFight();
		WaitForVblank();
		Flip();
		Sleep(300);

		while ((pl_life > 0) && (op_life > 0))
		{
			
			pl_state = still;

			

			//player movement and subsequent opponent response

			if ((!((*KEYS) & KEY_RIGHT)) && (pl_state != blocking) && (pl_state != attacking))
			{
				playx += 2;
				pl_state = moving;
			}
			else if (!((*KEYS) & KEY_LEFT))
			{
				playx -= 2;
				pl_state = moving;
			}
			else if ((!((*KEYS) & KEY_A)) && (pl_state != blocking) && (pl_state != moving))
			{
				pl_state = attacking;
				
			}
			else if ((!((*KEYS) & KEY_B)) && (pl_state != moving) && (pl_state != attacking))
			{
				pl_state = blocking;
			}
			
			
			//boundarycheck for player and opponent

			if (playx < 0)
			{
				playx = 0;
			}
			if (playx > (240 - player_still_WIDTH) / 2)
			{
				playx = (240 - player_still_WIDTH) / 2;
			}

			//control opponent
			rnum = RAND(3);
			if ((pl_state == still) || (pl_state == moving) || (((pl_state == blocking) || (pl_state == attacking)) && (oppx != playx)) && (rnum == 1))
			{
				rnum = RAND(2);
				if ((op_life >= pl_life) || (rnum == 1))
				{
					if (playx < oppx)
					{

						if (oppx < 0)
						{
							oppx = 0;
						}
						else if (oppx >(240 - opponent_still_WIDTH) / 2)
						{
							oppx = (240 - opponent_still_WIDTH) / 2;
						}
						else
						{
							oppx -= 2;
						}
					}
					if (playx > oppx) {

						if (oppx < 0)
						{
							oppx = 0;
						}
						else if (oppx >(240 - opponent_still_WIDTH) / 2)
						{
							oppx = (240 - opponent_still_WIDTH) / 2;
						}
						else
						{
							oppx += 2;
						}
					}
				}
				else 
				{
					rnum = RAND(2);
					if (rnum == 1)
					{
						if (playx < oppx)
						{

							if (oppx < 0)
							{
								oppx = 0;
							}
							else if (oppx >(240 - opponent_still_WIDTH) / 2)
							{
								oppx = (240 - opponent_still_WIDTH) / 2;
							}
							else
							{
								oppx += 2;
							}
						}
						if (playx > oppx) 
						{

							if (oppx < 0)
							{
								oppx = 0;
							}
							else if (oppx >(240 - opponent_still_WIDTH) / 2)
							{
								oppx = (240 - opponent_still_WIDTH) / 2;
							}
							else
							{
								oppx -= 2;
							}
						}
					}
				}
				op_state = still;
			}
			else if ((pl_state == attacking) && ((rnum == 2) || (rnum == 0)))
			{
				rnum = RAND(2);
				if (rnum == 1)
				{
					if (oppx == playx)
					{
						op_life -= 1;
						op_state = blocking;
					}
				}
				else 
				{
					if (oppx == playx)
					{
						op_life -= 8;
						op_state = still;
					}
				}
			}
			else if ((oppx == playx)) 
			{
				
				rnum = RAND(4);
				if ((rnum >= 0) && (rnum < 3))
				{
					op_state = attacking;
					if (pl_state == blocking)
					{
						pl_life -= 2;
					}
					else
					{
						pl_life -= 9;
					}
				}
			}
			
			//animations need to monitor if both are performing the same action or not
			if ((pl_state == attacking) || (op_state == attacking) || ((pl_state == attacking) && (op_state == attacking)))
			{
				

				pl_frames = 0;
				op_frames = 0;

				while (((pl_state == attacking) && (pl_frames < 5)) || ((op_state == attacking) && (op_frames < 5)))
				{
					DrawBackground();
					DrawPlayerLife(pl_life);
					DrawOpponentsLife(op_life);

					if ((op_state == attacking) && (op_frames < 5))
					{
						DrawOpponentAttack(oppx, oppy, 0, op_frames);

						op_frames++;

						//draw player dependant on the state they are in
						if ((pl_state == still) || (pl_state == moving))
						{

							DrawPlayerStill(playx, playy, 0);
						}
						else if (pl_state == blocking)
						{
							DrawPlayerBlock(playx, playy, 0);
						}
					}

					if ((pl_state == attacking) && (pl_frames < 5))
					{
						if ((op_state == still) || (op_state == moving))
						{
							DrawOpponentStill(oppx, oppy, 0);
						}
						else if (op_state == blocking)
						{
							DrawOpponentBlock(oppx, oppy, 0);
						}

						DrawPlayerAttack(playx, playy, 0, pl_frames);
						pl_frames++;
					}
				}
			}
			else 
			{
				DrawBackground();
				DrawPlayerLife(pl_life);
				DrawOpponentsLife(op_life);

				//draw opponent depending on state they are in
				if ((op_state == still) || (op_state == moving))
				{

					DrawOpponentStill(oppx, oppy, 0);
				}
				if (op_state == blocking)
				{

					DrawOpponentBlock(oppx, oppy, 0);
				}

				//draw player dependant on the state they are in
				if ((pl_state == still) || (pl_state == moving))
				{

					DrawPlayerStill(playx, playy, 0);
				}
				if (pl_state == blocking)
				{

					DrawPlayerBlock(playx, playy, 0);
				}
			}

			WaitForVblank();
			Flip();


		}//end of gameplay loop
		
		//draw, win or loss depending game result
		EraseScreen();
		//DoIntro();
		DrawBackground();
		DrawPlayerLife(pl_life);
		DrawOpponentsLife(op_life);

		if (op_life < 0)  {
			DrawWin();
		}
		else
		{
			DrawLost();
		}

		WaitForVblank();
		Flip();
		WaitForStart();

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

//draw for each state in which the player can be in
void DrawPlayerStill(int xplayer, int yplayer, int direction)
{
	//draws the player
	int x, y;

	for (y = 0; y < player_still_HEIGHT; y++)
	{
		for (x = 0; x < (player_still_WIDTH / 2); x++)
		{
			PlotPixel(x + xplayer, y + yplayer, player_stillData[y * (player_still_WIDTH / 2) + x]);
		}
	}

}

void DrawPlayerBlock(int xplayer, int yplayer, int direction)
{
	//draws the player
	int x, y;

	for (y = 0; y < player_still_HEIGHT; y++)
	{
		for (x = 0; x < (player_still_WIDTH / 2); x++)
		{
			PlotPixel(x + xplayer, y + yplayer, player_stillData[y * (player_still_WIDTH / 2) + x]);
		}
	}

}

void DrawPlayerAttack(int xplayer, int yplayer, int direction, int frame)
{
	//draws the player
	int x, y;

	for (y = 0; y < player_still_HEIGHT; y++)
	{
		for (x = 0; x < (player_still_WIDTH / 2); x++)
		{
			PlotPixel(x + xplayer, y + yplayer, player_stillData[y * (player_still_WIDTH / 2) + x]);
		}
	}

}


//draw for each different state in which the opponent can be in
void DrawOpponentStill(int xopp, int yopp, int direciton)
{
	//draws the opponent
	int x, y;

	for (y = 0; y < opponent_still_HEIGHT; y++)
	{
		for (x = 0; x < (opponent_still_WIDTH / 2); x++)
		{
			PlotPixel(x + xopp, y + yopp, opponent_stillData[y * (opponent_still_WIDTH / 2) + x]);
		}
	}
}

void DrawOpponentBlock(int xopp, int yopp, int direciton)
{
	//draws the opponent
	int x, y;

	for (y = 0; y < opponent_still_HEIGHT; y++)
	{
		for (x = 0; x < (opponent_still_WIDTH / 2); x++)
		{
			PlotPixel(x + xopp, y + yopp, opponent_stillData[y * (opponent_still_WIDTH / 2) + x]);
		}
	}
}

void DrawOpponentAttack(int xopp, int yopp, int direciton, int frames)
{
	//draws the opponent
	int x, y;

	for (y = 0; y < opponent_still_HEIGHT; y++)
	{
		for (x = 0; x < (opponent_still_WIDTH / 2); x++)
		{
			PlotPixel(x + xopp, y + yopp, opponent_stillData[y * (opponent_still_WIDTH / 2) + x]);
		}
	}
}

void DrawFight()
{
	//draws the time
	//draws the background image when the game is playing
	int x, y;

	for (y = 0; y < fight_HEIGHT; y++)
	{
		for (x = 0; x < (fight_WIDTH / 2); x++)
		{
			PlotPixel(x + 36, y + 20, fightData[y * (fight_WIDTH / 2) + x]);
		}
	}
}

void DrawLost(void)
{
	//draws the screen when the player loses
	int x, y;

	for (y = 0; y < lose_HEIGHT; y++)
	{
		for (x = 0; x < (win_WIDTH / 2); x++)
		{
			PlotPixel(x + 10, y + 30, loseData[y * (lose_WIDTH / 2) + x]);
		}
	}
}

void DrawWin(void)
{
	//draws the screen when the player wins
	int x, y;

	for (y = 0; y < win_HEIGHT; y++)
	{
		for (x = 0; x < (win_WIDTH / 2); x++)
		{
			PlotPixel(x + 10, y + 30, winData[y * (win_WIDTH / 2) + x]);
		}
	}
}

