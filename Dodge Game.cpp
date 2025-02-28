#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include "splashkit.h"

using namespace std;

typedef struct player_info
{
    bitmap bmp;
    float x, y;
    int lives;
} player_info;

typedef struct enemy_info
{
    bitmap bmp;
    float x, y;
    float dx, dy;
    int category; // Added category to differentiate between categories
} enemy_info;

typedef struct game_info
{
    player_info player;
    enemy_info enemies[10];
} game_info;

void load_resources();
void reset_to_centre(player_info *player);
void set_player(player_info *player);
void set_enemy(enemy_info *enemy);
void set_enemies(enemy_info enemies[]);
void start_game(game_info *game);
void draw_player(player_info player);
void draw_enemy(enemy_info enemy);
void draw_enemies(enemy_info enemies[]);
void draw_game(game_info game);
void destroy_player(player_info *player);
void update_enemy(enemy_info *enemy);
void handle_enemies(enemy_info enemies[], player_info *player);

int main()
{
    game_info new_game;
    load_resources();
    int PLAYER_SPEED = 1;
    open_window("Dodge Game", 800, 600);
    start_game(&new_game);
    while (not quit_requested())
    {
        process_events();
        if (key_down(LEFT_KEY))
            new_game.player.x -= PLAYER_SPEED;
        if (key_down(RIGHT_KEY))
            new_game.player.x += PLAYER_SPEED;
        if (key_down(UP_KEY))
            new_game.player.y -= PLAYER_SPEED;
        if (key_down(DOWN_KEY))
            new_game.player.y += PLAYER_SPEED;
        if ((new_game.player.x < -bitmap_width(new_game.player.bmp)) ||
            (new_game.player.y < -bitmap_height(new_game.player.bmp)) ||
            (new_game.player.x > screen_width()) ||
            (new_game.player.y > screen_height()))
            destroy_player(&new_game.player);
        handle_enemies(new_game.enemies, &new_game.player);
        draw_game(new_game);
        delay(50); // Adjust the delay to control game speed
    }
    return 0;
}

void load_resources()
{
    load_bitmap("spiderman", "spiderman.png");
    load_bitmap("batman", "batman.png");
    load_bitmap("superman", "superman.png");
    load_bitmap("donald_duck", "donald_duck.png");
    load_bitmap("tom", "tom.png");
    load_bitmap("jerry", "jerry.png");
    load_bitmap("mickey_mouse", "mickey_mouse.png");
}

void reset_to_centre(player_info *player)
{
    player->x = screen_width() / 2 - bitmap_width(player->bmp) / 2;
    player->y = screen_height() / 2 - bitmap_height(player->bmp) / 2;
}

void set_player(player_info *player)
{
    player->bmp = bitmap_named("spiderman");
    player->x = screen_width() / 2 - bitmap_width(player->bmp) / 2;
    player->y = screen_height() / 2 - bitmap_height(player->bmp) / 2;
    player->lives = 5;
}

void set_enemy(enemy_info *enemy)
{
    int select = 1 + rand() % 6;
    switch (select)
    {
    case 1:
        enemy->bmp = bitmap_named("batman");
        enemy->category = 1;
        break;
    case 2:
        enemy->bmp = bitmap_named("superman");
        enemy->category = 1;
        break;
    case 3:
        enemy->bmp = bitmap_named("donald_duck");
        enemy->category = 2;
        break;
    case 4:
        enemy->bmp = bitmap_named("tom");
        enemy->category = 2;
        break;
    case 5:
        enemy->bmp = bitmap_named("jerry");
        enemy->category = 2;
        break;
    case 6:
        enemy->bmp = bitmap_named("mickey_mouse");
        enemy->category = 2;
        break;
    }
    if (rand() % 2 == 1)
    {
        enemy->x = rand() % screen_width();
        enemy->dx = (rand() % 3 + 1) * 0.1; // Slower speed
    }
    else
    {
        enemy->x = screen_width() + rand() % screen_width();
        enemy->dx = -(rand() % 3 + 1) * 0.1; // Slower speed
    }

    if (rand() % 2 == 1)
    {
        enemy->y = -rand() % screen_height();
        enemy->dy = (rand() % 3 + 1) * 0.1; // Slower speed
    }
    else
    {
        enemy->y = screen_height() + rand() % screen_height();
        enemy->dy = -(rand() % 3 + 1) * 0.1; // Slower speed
    }
    enemy->dx = 0.1; // Fixed speed for all enemies
    enemy->dy = 0.1; // Fixed speed for all enemies

}

void set_enemies(enemy_info enemies[])
{
    for (int i = 0; i < 10; ++i)
    {
        set_enemy(&enemies[i]);
    }
}

void start_game(game_info *game)
{
    set_player(&(game->player));
    set_enemies(game->enemies);
}

void draw_player(player_info player)
{
    draw_bitmap(player.bmp, player.x, player.y);
    draw_text("Lives: " + to_string(player.lives), COLOR_BLACK, 10, 10); // Display lives
}

void draw_enemy(enemy_info enemy)
{
    draw_bitmap(enemy.bmp, enemy.x, enemy.y);
}

void draw_enemies(enemy_info enemies[])
{
    for (int i = 0; i < 10; ++i)
    {
        draw_enemy(enemies[i]);
    }
}

void draw_game(game_info game)
{
    clear_screen(COLOR_WHITE);
    draw_player(game.player);
    draw_enemies(game.enemies);
    refresh_screen();
}

void destroy_player(player_info *player)
{
    player->lives--;
    reset_to_centre(player);
}

void update_enemy(enemy_info *enemy)
{
    enemy->x += enemy->dx;
    enemy->y += enemy->dy;
    if (enemy->x < -bitmap_width(enemy->bmp) || enemy->x > screen_width() || enemy->y < -bitmap_height(enemy->bmp) || enemy->y > screen_height())
    {
        set_enemy(enemy); // Reset enemy position if it goes out of the window
    }
}

void handle_enemies(enemy_info enemies[], player_info *player)
{
    for (int i = 0; i < 10; ++i)
    {
        update_enemy(&enemies[i]);
        if (bitmap_collision(player->bmp, player->x, player->y, enemies[i].bmp, enemies[i].x, enemies[i].y))
        {
            if (enemies[i].category == 1)
            {
                // Player hits Category 1 enemy, lose one life
                player->lives--;
            }
            else
            {
                // Player hits Category 2 enemy, gain one extra life
                player->lives++;
            }
            // Reset enemy position
            set_enemy(&enemies[i]);
        }
    }

    // Check if player is out of the window
    if (player->x < 0 || player->x > screen_width() || player->y < 0 || player->y > screen_height())
    {
        player->lives--; // Lose one life if out of the window
        // Reset player position to the center of the screen
        reset_to_centre(player);
    }

    // Check win or lose condition
    if (player->lives <= 0)
    {
        draw_text("You Lost", COLOR_RED, screen_width() / 2 - 50, screen_height() / 2);
        refresh_screen();
        delay(10000); 
        exit(0); // End the game
    }
    else if (player->lives >= 10)
    {
        draw_text("You Won", COLOR_GREEN, screen_width() / 2 - 50, screen_height() / 2);
        refresh_screen();
        delay(10000); 
        exit(0); // End the game
    }
}


output: 
