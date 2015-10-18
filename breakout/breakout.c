//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));
    
    double xvelocity = drand48()*3;
    double yvelocity = drand48()*4;

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);
    
    //paddle width, height
    int pwidth = 45;
    int pheight = 15;
    
    // ball width
    int bwidth = 15;
    

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    { 
    
        waitForClick();
       
        while (true)
        {
            //check for mouse movement
            GEvent event = getNextEvent(MOUSE_EVENT);
            
            //move the ball, bounce off walls. 
            move(ball, xvelocity, yvelocity);
        
            if (getX(ball) <= 0)
            {
                xvelocity = -xvelocity;
            }
            else if (getX(ball)+bwidth > WIDTH)
            {
                xvelocity = -xvelocity;
            }
        
            if (getY(ball)<= 0)
            {
                yvelocity = -yvelocity;
            }
            
            GObject object = detectCollision(window, ball);
            
            // ball bounces off paddle
            if (object == paddle)
            {
                yvelocity = -yvelocity;
            }

            //if floor is hit, restart ball, lose life, and wait for click. 
            if (getY(ball) + bwidth > HEIGHT)
            {
                setLocation(ball, (WIDTH-bwidth)/2, (HEIGHT+bwidth)/2);
                lives = lives-1;
                printf("%d\n", lives);
                waitForClick();
                xvelocity = drand48()*3;
            } 

            //if a brick is hit, bounce and remove the brick.
            if (object != NULL && object != paddle && object != label)
            {
                yvelocity = -yvelocity;
                removeGWindow(window, object);
                bricks = bricks-1;
                points = points+1;
                printf("brick hit\n");
                updateScoreboard(window, label, points);
            }     


            //break forever loop if lives or bricks run out. 
            if (lives <1 | bricks < 1)
                break;           
                
            //move the paddle when the mouse moves.
            if (event != NULL)
            {       
                    if (getEventType(event) == MOUSE_MOVED)
                    {
                        double x = getX(event)-pwidth/2;
                        setLocation(paddle, x, HEIGHT-pwidth);
                    }
                    
            }
                
            //pause for ball visibility.
            pause(9);    
        }
    }
    
    printf("game over\n");
    
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{            
    //make 5 rows
    for (int rows = 0; rows < ROWS; rows++)
    { 
        int width = 35;
        int height = 10;
        GRect brick;

        //each row has 10 bricks
        for (int bnum = 0; bnum < COLS; bnum++)
        {
            GRect brick = newGRect((5+width)*bnum + 2, 3+(height+5)*rows, width, height);
            setFilled(brick, true);
            add (window, brick);
        
            //change color based on row
            switch (rows)
            {
                case 0:
                    setColor(brick, "RED");
                    break;
                case 1:
                    setColor(brick, "ORANGE"); 
                    break;   
                case 2:
                    setColor(brick, "YELLOW");
                    break;
                case 3:
                    setColor(brick, "GREEN");
                    break;
                case 4:
                    setColor(brick, "BLUE");
                    break;
            
            }
        } 
    }
    
}


/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    int bwidth = 15;
    
    GOval ball = newGOval(WIDTH/2-bwidth, (HEIGHT-bwidth)/2, bwidth, bwidth);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    
    add(window, ball);
    
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{   
    //instantiate paddle
    int pwidth = 45;
    int pheight = 15;
    
    GRect paddle = newGRect((WIDTH-pwidth)/2, (HEIGHT-pwidth), pwidth, pheight);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);
    
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    //create and center scoreboard
    GLabel label = newGLabel(" ");
    setFont(label, "SansSerif-36");
    
    double x = (getWidth(window)-getWidth(label))/2;
    double y = (getHeight(window)-getHeight(label))/2;
    setLocation(label, x, 200);
    
    char s[12];
    sprintf(s, "0");
    setLabel(label, s);
    add(window, label);
    
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", 0+points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
