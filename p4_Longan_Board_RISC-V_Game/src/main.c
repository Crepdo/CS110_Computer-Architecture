#include "lcd/lcd.h"
#include <string.h>
#include "utils.h"
#include <stdlib.h>

// Gamepad
#define GP_BUTTON_UP (!gpio_input_bit_get(GPIOB, GPIO_PIN_8))    //上方向键（左手五向摇杆）
#define GP_BUTTON_DOWN (!gpio_input_bit_get(GPIOB, GPIO_PIN_10)) //下方向键（左手五向摇杆）
#define GP_BUTTON_RIGHT (!gpio_input_bit_get(GPIOB, GPIO_PIN_7)) //右方向键（左手五向摇杆）
#define GP_BUTTON_LEFT (!gpio_input_bit_get(GPIOB, GPIO_PIN_6))  //左方向键（左手五向摇杆）
#define GP_BUTTON_PUSH (!gpio_input_bit_get(GPIOB, GPIO_PIN_11)) //按下键（左手五向摇杆）
#define GP_BUTTON_A (!gpio_input_bit_get(GPIOA, GPIO_PIN_2))     //A按键（右手AB按键）
#define GP_BUTTON_B (!gpio_input_bit_get(GPIOA, GPIO_PIN_3))     //B按键（左手AB按键）

// Map width and map height
#define MAP_WIDTH 32
#define MAP_HEIGHT 16

// Color setting
#define COLOR_BACKGROUND BLACK
#define COLOR_FOOD GREEN
#define COLOR_SUBFOOD RED
#define COLOR_HEAD CYAN
#define COLOR_BODY BLUE

// Food type
#define ADD_FOOD 1
#define SUB_FOOD 2

// Update interval (ms)
#define UPGRATE_INTERVAL 200

// Goal score
#define GOAL_SCORE 10

// Init gamepad inputs and output
void Gamepad_init()
{
    // Init buttons
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    // Init viberation
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
}

// Init BOOL button input
void Inp_init(void)
{
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

// Init adc
void Adc_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);
    RCU_CFG0 |= (0b10 << 14) | (1 << 28);
    rcu_periph_clock_enable(RCU_ADC0);
    ADC_CTL1(ADC0) |= ADC_CTL1_ADCON;
}

typedef enum MoveDirection
{
    Up,
    Down,
    Left,
    Right
} MoveDirection;

/******************
 * 
 * IO
 * 
 ******************/
void IO_init(void)
{
    Gamepad_init(); // gamepad init
    Inp_init();     // inport init
    Adc_init();     // A/D init
    Lcd_Init();     // LCD init
}

/******************
 * 
 * UI
 * 
 ******************/
void UIShowWin();

/* UI */
void UIShowScore(int score);

/* UI */
void UIShowGameOver();

/* UI */
void UIUpdateMainMenu();

/* UI */
void UIShowText(u8 *text)
{
    LCD_Clear(COLOR_BACKGROUND);
    LCD_ShowString(0, 0, (u8 *)(text), WHITE);
}

/******************
 * 
 * Wall
 * 
 ******************/
void DrawWall();

/* Check if hit the wall */
bool SnakeCheckHitWall(int snakeFrontPosX, int snakeFrontPosY);

/******************
 * 
 * Snake
 * 
 ******************/
/* draw snake */
void SnakeDraw(int *snakePosX, int *snakePosY, int snakeLength);
/* init snake */
void SnakeInit(int *snakePosX, int *snakePosY, int *snakeLength);

void SnakeMove(int *snakePosX, int *snakePosY, int *snakeLength, MoveDirection moveDirection, int getFood)
{
    /* Check food type*/
    if (getFood != 0)
    {
        /*Lengthen food*/
        if (getFood == ADD_FOOD)
        {
            /* Add snake length */
            ++*snakeLength;
            /* Set new tail */
            snakePosX[*snakeLength - 1] = snakePosX[*snakeLength - 2];
            snakePosY[*snakeLength - 1] = snakePosY[*snakeLength - 2];

            /* Update body expect for new tail*/
            for (int i = *snakeLength - 2; i > 0; i--)
            {
                snakePosX[i] = snakePosX[i - 1];
                snakePosY[i] = snakePosY[i - 1];
            }
        }
        /*Shorten food*/
        if (getFood == SUB_FOOD)
        {
            /*original tail work*/
            LCD_Fill(snakePosX[*snakeLength - 1] * 5,
                     snakePosY[*snakeLength - 1] * 5,
                     (snakePosX[*snakeLength - 1] + 1) * 5 - 1,
                     (snakePosY[*snakeLength - 1] + 1) * 5 - 1,
                     COLOR_BACKGROUND);
            /* Sub snake length */
            --*snakeLength;
            /*new tail work*/
            LCD_Fill(snakePosX[*snakeLength - 1] * 5,
                     snakePosY[*snakeLength - 1] * 5,
                     (snakePosX[*snakeLength - 1] + 1) * 5 - 1,
                     (snakePosY[*snakeLength - 1] + 1) * 5 - 1,
                     COLOR_BACKGROUND);
            /* Update body expect for original tail*/
            for (int i = *snakeLength - 1; i > 0; i--)
            {
                snakePosX[i] = snakePosX[i - 1];
                snakePosY[i] = snakePosY[i - 1];
            }
        }
    }
    else
    {
        /* Clear tail */
        LCD_Fill(snakePosX[*snakeLength - 1] * 5,
                 snakePosY[*snakeLength - 1] * 5,
                 (snakePosX[*snakeLength - 1] + 1) * 5 - 1,
                 (snakePosY[*snakeLength - 1] + 1) * 5 - 1,
                 COLOR_BACKGROUND);

        /* Update body */
        for (int i = *snakeLength - 1; i > 0; i--)
        {
            snakePosX[i] = snakePosX[i - 1];
            snakePosY[i] = snakePosY[i - 1];
        }
    }

    /* Update head */
    switch (moveDirection)
    {
    case Up:
        snakePosY[0] = snakePosY[0] - 1;
        break;
    case Down:
        snakePosY[0] = snakePosY[0] + 1;
        break;
    case Left:
        snakePosX[0] = snakePosX[0] - 1;
        break;
    case Right:
        snakePosX[0] = snakePosX[0] + 1;
        break;
    }

    /* Make sure inside map */
    for (int i = 0; i < *snakeLength; i++)
    {
        /* Check X pos */
        if (snakePosX[i] < 0)
            snakePosX[i] += MAP_WIDTH;
        else if (snakePosX[i] > MAP_WIDTH - 1)
            snakePosX[i] -= MAP_WIDTH;

        /* Check Y pos */
        if (snakePosY[i] < 0)
            snakePosY[i] += MAP_HEIGHT;
        else if (snakePosY[i] > MAP_HEIGHT - 1)
            snakePosY[i] -= MAP_HEIGHT;
    }
}

void SnakeUpdateFrontPos(int *snakePosX, int *snakePosY, MoveDirection moveDirection, int *snakeFrontPosX, int *snakeFrontPosY);

bool SnakeCheckHitSelf(int *snakePosX, int *snakePosY, int snakeLength, int snakeFrontPosX, int snakeFrontPosY)
{
    /* Check hit self */
    for (int i = 0; i < snakeLength; i++)
    {
        if (snakeFrontPosX == snakePosX[i] && snakeFrontPosY == snakePosY[i])
            return TRUE;
    }

    /* Not hitting self */
    return FALSE;
}

/******************
 * 
 * Food
 * 
 ******************/

/*Draw both type of food*/
void FoodDraw(int foodPosX, int foodPosY, int food_color)
{
    LCD_Fill(foodPosX * 5, foodPosY * 5, (foodPosX + 1) * 5 - 1, (foodPosY + 1) * 5 - 1, food_color);
    LCD_Fill(foodPosX * 5 + 1, foodPosY * 5 + 1, (foodPosX + 1) * 5 - 2, (foodPosY + 1) * 5 - 2, WHITE);
}

/*Generator for both type of food*/
/*foodG: generated food2:not generated*/
void FoodGenerate(int *foodG_PosX, int *foodG_PosY, int *food2PosX, int *food2PosY, int *snakePosX, int *snakePosY, int snakeLength, int food_color)
{
    bool isInsideOther = FALSE;
    /*Clear the old food*/
    LCD_Fill(*foodG_PosX * 5, *foodG_PosY * 5, (*foodG_PosX + 1) * 5 - 1, (*foodG_PosY + 1) * 5 - 1, COLOR_BACKGROUND);
    do
    {
        /* Update food pos randomly */
        *foodG_PosX = rand() % MAP_WIDTH;
        *foodG_PosY = rand() % MAP_HEIGHT;
        /* Temp flag for checking food pos inside snake */
        isInsideOther = FALSE;

        /* Check if new pos is inside snake */
        for (int i = 0; i < snakeLength; i++)
        {
            /* Check pos */
            if (snakePosX[i] == *foodG_PosX || snakePosY[i] == *foodG_PosY)
                isInsideOther = TRUE;
        }

        /*Check pos of not drawed foods*/
        if (*foodG_PosX == *food2PosX && *foodG_PosY == *food2PosY)
            isInsideOther = TRUE;
    } while (isInsideOther == TRUE);
    /* Draw the needed food in new pos */
    FoodDraw(*foodG_PosX, *foodG_PosY, food_color);
}

/******************
 * 
 * Game manager
 * 
 ******************/
void CheckDifficulty(int *diff)
{
    while (1)
    {
        /* Start button pressed */
        if (GP_BUTTON_A)
        {
            LCD_Clear(COLOR_BACKGROUND);
            *diff = 2;
            break;
        }

        /* Start button pressed */
        if (GP_BUTTON_B)
        {
            LCD_Clear(COLOR_BACKGROUND);
            *diff = 3;
            break;
        }
    }
}

void GameInit(int *score, int *snakePosX, int *snakePosY, int *snakeLength, int *snakeFrontPosX, int *snakeFrontPosY, int *foodPosX, int *foodPosY, int *subfoodPosX, int *subfoodPosY, MoveDirection *moveDirection, int *diff)
{
    /* Clear main menu */
    delay_1ms(100);
    LCD_Clear(COLOR_BACKGROUND);

    Introduction();
    CheckDifficulty(diff);

    /* Clear introduction */
    delay_1ms(100);
    LCD_Clear(COLOR_BACKGROUND);
    /* Draw the wall*/
    DrawWall();
    /* Init move direction */
    *moveDirection = Right;
    /* Init score */
    *score = 0;
    /* Init snake */
    SnakeInit(snakePosX, snakePosY, snakeLength);

    /* Draw initial 2 food */
    FoodGenerate(foodPosX, foodPosY, subfoodPosX, subfoodPosY, snakePosX, snakePosY, *snakeLength, COLOR_FOOD);
    if (*snakeLength > 6)
        FoodGenerate(subfoodPosX, subfoodPosY, foodPosX, foodPosY, snakePosX, snakePosY, *snakeLength, COLOR_SUBFOOD);
}

void GameWin(int score)
{
    /* UI */
    UIShowWin();
    UIShowScore(score);

    /* Viberate */
    ViberateSet(1);
    delay_1ms(800);
    ViberateSet(0);
}

void GameOver(int score)
{
    /* UI */
    UIShowGameOver();
    UIShowScore(score);

    /* Viberate */
    ViberateSet(1);
    delay_1ms(800);
    ViberateSet(0);
}

/******************
 * 
 * Main
 * 
 ******************/
int main(void)
{
    // LCD init
    IO_init();
    LCD_Clear(COLOR_BACKGROUND);
    BACK_COLOR = COLOR_BACKGROUND;

    // Timer for update speed control (ms)
    uint64_t frameStartTime = 0;

    // Record move direction in next frame
    MoveDirection moveDirection = Right;

    // Snake
    int snakePosX[MAP_HEIGHT * MAP_HEIGHT];
    int snakePosY[MAP_HEIGHT * MAP_HEIGHT];
    int snakeLength;

    // Snake front pos
    int snakeFrontPosX = -1;
    int snakeFrontPosY = -1;
    MoveDirection lastMoveDirection = Right;

    //Add food
    int foodPosX = 0;
    int foodPosY = 0;

    //Sub food
    int subfoodPosX = 0;
    int subfoodPosY = 0;

    int subfoodPosX1[30] = {0};
    int subfoodPosY1[30] = {0};

    //difficulty
    int diff = 1;
    //score: how many food eated (both type)
    int score = 0;
    // Main loop
    while (1)
    {
        /* Clear display */
        LCD_Clear(COLOR_BACKGROUND);

        // Main menu loop
        while (1)
        {
            /* Main menu animation */
            UIUpdateMainMenu();

            /* Start button pressed */
            if (GP_BUTTON_A)
                break;
        }
        LCD_Clear(COLOR_BACKGROUND);
        /* Init game */
        GameInit(&score, snakePosX, snakePosY, &snakeLength, &snakeFrontPosX, &snakeFrontPosY, &foodPosX, &foodPosY, &subfoodPosX, &subfoodPosY, &moveDirection, &diff);

        /* Game loop */
        while (1)
        {
            /* Update front pos */
            SnakeUpdateFrontPos(snakePosX, snakePosY, moveDirection, &snakeFrontPosX, &snakeFrontPosY);
            int flag_t = 0;
            for (int i = 0; i < (snakeLength - 3) * diff; i += 1)
            {
                if (snakeFrontPosX == subfoodPosX1[i] && snakeFrontPosY == subfoodPosY1[i])
                    flag_t = 1;
            }
            /* Check what is in front of the snake */
            if ((snakeFrontPosX == foodPosX && snakeFrontPosY == foodPosY) || (snakeFrontPosX == subfoodPosX && snakeFrontPosY == subfoodPosY) || flag_t == 1) // Hit food
            {
                score++;
                if ((snakeLength - 3) == GOAL_SCORE) // Snake length == goal length 3:initial length
                {
                    /* Win */
                    GameWin(snakeLength - 3);
                    break;
                }
                else
                {
                    /*Temp foodtype for move function (this should before generate)*/
                    int foodtype = 0;
                    if (snakeFrontPosX == foodPosX && snakeFrontPosY == foodPosY)
                        foodtype = ADD_FOOD;
                    if (snakeFrontPosX == subfoodPosX && snakeFrontPosY == subfoodPosY)
                        foodtype = SUB_FOOD;
                    for (int i = 0; i < (snakeLength - 3) * diff; i += 1)
                    {
                        if (snakeFrontPosX == subfoodPosX1[i] && snakeFrontPosY == subfoodPosY1[i])
                            foodtype = SUB_FOOD;
                    }

                    /* Generate 2 type new food */
                    FoodGenerate(&foodPosX, &foodPosY, &subfoodPosX, &subfoodPosY, snakePosX, snakePosY, snakeLength, COLOR_FOOD);

                    /* difficulty level */
                    if (snakeLength > 4)
                    {
                        for (int i = 0; i < 5; i += 1)
                        {
                            // generate food
                            FoodGenerate(&subfoodPosX1[i], &subfoodPosY1[i], &foodPosX, &foodPosY, snakePosX, snakePosY, snakeLength, COLOR_SUBFOOD);
                        }
                    }

                    /* Move forward */
                    SnakeMove(snakePosX, snakePosY, &snakeLength, moveDirection, foodtype);
                }
            }
            else if (SnakeCheckHitSelf(snakePosX, snakePosY, snakeLength, snakeFrontPosX, snakeFrontPosY) || SnakeCheckHitWall(snakeFrontPosX, snakeFrontPosY)) // Hit self
            {
                /* Gameover */
                GameOver(snakeLength - 3);
                break;
            }
            else
            {
                /* Move forward */
                SnakeMove(snakePosX, snakePosY, &snakeLength, moveDirection, 0);
            }

            /* Draw wall */
            DrawWall();

            /* Draw snake */
            SnakeDraw(snakePosX, snakePosY, snakeLength);

            /* Get last move direction */
            lastMoveDirection = moveDirection;

            /* Get input loop */
            while (1)
            {
                /* Get the player input and Decide direction of movement in next frame */
                if (GP_BUTTON_UP && lastMoveDirection != Down)
                    moveDirection = Up;
                else if (GP_BUTTON_DOWN && lastMoveDirection != Up)
                    moveDirection = Down;
                else if (GP_BUTTON_LEFT && lastMoveDirection != Right)
                    moveDirection = Left;
                else if (GP_BUTTON_RIGHT && lastMoveDirection != Left)
                    moveDirection = Right;

                /* Reach one frame time limit */
                if (get_timer_value() - frameStartTime > SystemCoreClock / 4000.0 * UPGRATE_INTERVAL)
                {
                    /* Go to next frame */
                    frameStartTime = get_timer_value();
                    break;
                }
            }
        }
    }
}
