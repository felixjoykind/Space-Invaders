#define OLC_PGE_APPLICATION
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "Invader.h"
#include "Player.h"
#include "Bullet.h"
#include "Explosion.h"
#include "olcPixelGameEngine.h"

class Game : public olc::PixelGameEngine
{
public:
	Game()
	{
		sAppName = "Space Invaders";
	}

	bool OnUserCreate() override
	{
		Clear(olc::BLACK);

		invadersSpr = {
			new olc::Sprite("./sprites/invader1.png"),
			new olc::Sprite("./sprites/invader2.png"),
			new olc::Sprite("./sprites/invader3.png")
		};

		playerSpr = new olc::Sprite("./sprites/player.png");
		explosionSpr = new olc::Sprite("./sprites/explosion.png");

		player = new Player(ScreenWidth() / 2, ScreenHeight() - 10, 16, 8);

		invaders.clear();
		bullets.clear();

		// loading best score
		std::ifstream input("best_score.txt");
		input >> bestScore;
		std::cout << "LOADED HIGH SCORE: " << bestScore << std::endl;

		FillInvaders();

		score = 0;
		movingLeft = false;
		canInvadersShoot = true;
		running = true;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// getting user input
		if (IsFocused())
		{
			if (running)
			{
				if (GetKey(olc::Key::UP).bPressed)
					tryShoot();
				if (GetKey(olc::Key::RIGHT).bHeld)
				{
					if (player->GetX() + player->GetWidth() < ScreenWidth())
						player->Move(80.0f * fElapsedTime, 0);
				}
				if (GetKey(olc::Key::LEFT).bHeld)
				{
					if (player->GetX() > 0)
						player->Move(-80.0f * fElapsedTime, 0);
				}
			}
			else if (!running)
			{
				if (GetKey(olc::Key::SPACE).bPressed)
					OnUserCreate();
			}
		}

		if (!running) return true;

		// update imvaders position
		float step = movingLeft ? -3.25f : 3.25f;
		for (int y = 0; y < invaders.size(); y++)
			for (int x = 0; x < invaders[y].size(); x++)
			{
				auto& invader = invaders[y][x];
				if (invader.isAlive)
				{
					if ((invader.GetX() + invader.GetWidth() + 2 > ScreenWidth()) || (invader.GetX() <= 0))
						MoveDown();

					if (invader.GetY() + invader.GetHeight() > ScreenHeight() - 15)
						running = false;
				}

				invader.Move(step * fElapsedTime, 0);
				//std::cout << invader.GetX() << ", " << invader.GetY() << std::endl;
			}


		if (canInvadersShoot)
		{
			// choosing random enemy to shoot
			Invader invader(0, 0, 0, 0);
			do
			{
				int y = rand() % invaders.size();		// <--| getting random invader
				int x = rand() % invaders[y].size();	// <--|

				invader = invaders[y][x]; // random invader
			} while (!invader.isAlive);

			bullets.push_back(Bullet(invader.GetX() + invader.GetWidth() / 2, invader.GetY() + invader.GetHeight() / 2, 1, 5,
				Shooter::Invader, BulletColor{ 0, 255, 0 }));
			canInvadersShoot = false;
		}

		// collision detection
		for (int i = 0; i < bullets.size(); i++)
		{
			auto& bullet = bullets[i];
			for (int y = 0; y < invaders.size(); y++)
				for (int x = 0; x < invaders[y].size(); x++)
				{
					auto& invader = invaders[y][x];

					if (bullet.GetOwner() == Shooter::Player)
						if (invader.Intersect(bullet) && invader.isAlive)
						{
							invader.isAlive = false;
							bullets.erase(bullets.begin() + i);
							player->canShoot = true;
							invader.isAlive = false;

							explosions.push_back(Explosion(invader.GetX(), invader.GetY(), 0, 0));

							score += 100;
							std::cout << "HIT" << std::endl;
						}

					auto i = remove_if(bullets.begin(), bullets.end(),
						[&](Bullet b) { return bullet.GetOwner() == Shooter::Invader && player->Intersect(bullet); });
					if (i != bullets.end())
					{
						bullets.erase(i);
						player->TakeDamage(1);
						std::cout << "DAMAGED" << std::endl;
						canInvadersShoot = true;
					}
				}

			// checking for collision between bullets
			for (int j = 0; j < bullets.size(); j++)
			{
				auto& nextBullet = bullets[j];

				auto i = remove_if(bullets.begin(), bullets.end(),
					[&](Bullet b) { return b.GetOwner() != nextBullet.GetOwner() && b.Intersect(nextBullet); });

				if (i != bullets.end())
				{
					nextBullet.Move(ScreenWidth() + 100, 0);
					bullets.erase(i);
					player->canShoot = true;
					std::cout << "BULLETS HIT" << std::endl;
				}
			}
		}
		// update bullets position
		for (int i = 0; i < bullets.size(); i++)
		{
			auto& bullet = bullets[i];
			// moving bullet with speed that depends on bullet owner (player or invader)
			bullet.Move(0, (bullet.GetOwner() == Shooter::Player ? -150.0f : 125.0f) * fElapsedTime);

			if (bullet.GetY() < 0 || bullet.GetY() + bullet.GetHeight() > ScreenHeight() + 10)
			{
				Shooter owner = bullet.GetOwner();
				if (owner == Shooter::Player)
					player->canShoot = true;
				else if (owner == Shooter::Invader)
					canInvadersShoot = true;
				bullets.erase(bullets.begin() + i);
			}

			//std::cout << "BULLET: " << i << ", POSITION: " << bullet.GetX() << ", " << bullet.GetY() << std::endl;
		}

		// update explosions
		for (auto& explosion : explosions)
		{
			explosion.elapsed += fElapsedTime;

			auto i = remove_if(explosions.begin(), explosions.end(), 
				[&](Explosion e) { return explosion.elapsed >= 0.25f; });

			if (i != explosions.end())
				explosions.erase(i);
		}

		// clear screen
		Clear(olc::BLACK);
		// draw invaders
		for (int y = 0; y < invaders.size(); y++)
			for (int x = 0; x < invaders[y].size(); x++)
			{
				auto& invader = invaders[y][x];
				if (!invader.isAlive) continue;

				if (invader.isAlive)
				{
					// getting correct invader sprite based on position in vector
					olc::vf2d pos = { float(invader.GetX()), float(invader.GetY()) };
					olc::Sprite* sprite;
					switch (y)
					{
					case 1:
					case 2:
						sprite = invadersSpr[1];
						break;
					case 3:
					case 4:
						sprite = invadersSpr[2];
						break;
					default:
						sprite = invadersSpr[0];
						break;
					}
					DrawSprite(pos, sprite);
				}
			}
		// draw bullets
		for (auto bullet : bullets)
			FillRect(bullet.GetX(), bullet.GetY(), bullet.GetWidth(), bullet.GetHeight(),
				olc::Pixel(bullet.Color.r, bullet.Color.g, bullet.Color.b));
		// draw explosions
		for (auto explosion : explosions)
		{
			olc::vf2d pos = { float(explosion.GetX()), float(explosion.GetY()) };
			DrawSprite(pos, explosionSpr);
		}
		// draw player
		olc::vf2d pos = { float(player->GetX()), float(player->GetY()) };
		DrawSprite(pos, playerSpr);

		// draw player lives
		DrawString(0, 1, "<" + std::to_string(player->GetLives()) + ">");
		// draw player score
		DrawString(ScreenWidth() - 100, 1, "SCORE: " + std::to_string(score));
		DrawString(50, 1, "HI: " + std::to_string(bestScore));

		// check if game is over
		if (player->GetLives() <= 0)
			running = false;

		if (!running)
		{
			if (score > bestScore)
			{
				bestScore = score;
				std::ofstream file("highscore.txt");
				file << bestScore;
				file.close();
			}
			DrawString(ScreenWidth() / 2 - 35, ScreenHeight() / 2 - 20, "Game Over!");
			DrawString(ScreenWidth() / 2 - 90, ScreenHeight() / 2, "Press SPACE to restart");
		}

		if (GetInvadersCount() == 0)
		{
			FillInvaders();
		}

		return true;
	}

private:
	std::vector<std::vector<Invader>> invaders;
	std::vector<Bullet> bullets;
	std::vector<Explosion> explosions;

	bool movingLeft;
	bool running;
	bool canInvadersShoot;

	int score;
	int bestScore;

	std::vector<olc::Sprite*> invadersSpr;
	olc::Sprite* playerSpr;
	olc::Sprite* explosionSpr;

	Player* player;

	void MoveDown()
	{
		for (int y = 0; y < invaders.size(); y++)
			for (int x = 0; x < invaders[y].size(); x++)
				invaders[y][x].Move(movingLeft ? 2.0f : -2.0f, 8.0f);
		movingLeft = !movingLeft;
	}

	void tryShoot()
	{
		if (player->canShoot)
		{
			std::cout << "SHOOT" << std::endl;
			bullets.push_back(Bullet(player->GetX() + 7, player->GetY() - 3, 1, 5,
				Shooter::Player, BulletColor{ 255, 0, 0 }));
			player->canShoot = false;
		}
	}

	int GetInvadersCount()
	{
		int count = 0;
		for (auto& v : invaders)
			for (auto& i : v)
				if (i.isAlive) count++;
		return count;
	}

	void FillInvaders()
	{
		invaders.clear();
		const int GAP = 10;
		for (int y = 0; y < 5; y++) {
			std::vector<Invader> v;
			invaders.push_back(v);
			for (int x = 0; x < 11; x++) {
				// calcuate position for invader (literally found this on google)
				float invaderX = x * 5 + (GAP * x) + 50;
				float invaderY = y + (GAP * y) + 30;

				float width;
				float height;

				// setting width and height based on position in invaders vector
				switch (y)
				{
				case 1:
				case 2:
					width = 10;
					height = 8;
					break;
				case 3:
				case 4:
					width = 12;
					height = 8;
					break;
				default:
					width = 8;
					height = 8;
				}
				invaders[y].push_back(Invader(invaderX, invaderY, width, height));
			}
		}
		movingLeft = false;
		canInvadersShoot = true;
		bullets.clear();
	}
};

int main()
{
	Game game;
	if (game.Construct(250, 180, 4, 4))
		game.Start();

	return 0;
}