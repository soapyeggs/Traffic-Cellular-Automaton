#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define VMAX 4
#define GMAX 100
#define SIZE 1000

struct Car {
   int id;
   int velocity;
   int position;
   int nextPosition;
   int journeyTime;
};


int generation = 0;
struct Car *cells[SIZE] = {NULL};
struct Car *nextGen[SIZE] = {NULL};
int results[SIZE][GMAX];
FILE *outputFile;
/*
** Return the smaller of 2 numbers.
*/
int min(int a, int b)
{
	int c = 0;

	if( a < b )
	{
		c = a;;
	}
	else
	{
		c = b;
	}

	return c;
}//end min

/*
** Function to return the size of the gap in front of
** the car at the index of "position" in the cells array.
*/
int findGap(int position)
{
    //start off with no gap
    int gap = 0;

    int i = 1;
    //check the cells that the car will reach at this velocity
    while(i <= cells[position]->velocity)
    {
        //increase gap if cell is empty
        if(cells[position + i] == NULL)
        {
            gap++;
        }
        //return the gap once a cell isn't null
        else
        {
            return gap;
        }
        i++;
    }
    return gap;
}//end findGap

/*
** Increases the speeds of each car in the cells towards VMAX.
** Doesn't actually move them, just changes the speeds & next positions.
*/
void accelerationRule()
{
    int i;
    //loop through cells array
    for (i = 0; i < SIZE; i ++)
    {
        if(cells[i] != NULL)
        {
            //accelerate car
            cells[i]->velocity = min(cells[i]->velocity + 1, VMAX);
        }
    }
}//end accelerationRule

/*
** Slow the car down if it's going to collide with the car in front
*/
void brakeRule()
{
    int i;
    //loop through cells array
    for(i = 0; i < SIZE; i++)
    {
        if(cells[i] != NULL)
        {
            int gap = findGap(cells[i]->position);
            cells[i]->velocity = min(gap, cells[i]->velocity);
        }
    }
}//end brakeRule

/*
** Introduce an element of randomness to the car's velocity.
** Sets the car's speed to a random number between 1 and VMAX.
*/
void randomisationRule()
{
    int i;
    //loop through cells array
    for(i = 0; i < SIZE; i++)
    {

        if(cells[i] != NULL)
        {
            //give probability of .2 for randomisation
            if(rand()%10 >= 9)
            {
                //set the speed of a car to be a random number less than VMAX
                int speed = (rand() % VMAX) + 1;
                cells[i]->velocity = speed;
                printf("random %d, car %d\n", cells[i]->velocity, cells[i]->id);
            }
        }
    }
}//end randomisationRule

/*
** Checks to see if every cell in the array is empty
*/
bool isArrayEmpty()
{
    bool isEmpty = true;
    int i;

    //loop through array
    for(i = 0; i < SIZE; i++)
    {
        //return once a non-empty cell is found
        if (cells[i] != NULL)
        {
            return isEmpty = false;
        }
    }
    return isEmpty;
}//end isArrayEmpty

/*
** Function to create a default car structure to be placed into a cell
*/
struct Car *createCar()
{
    struct Car *c = (struct Car*)malloc(sizeof(struct Car));
    c->id = generation;
    c->velocity = 0;
    c->position = 0;
    c->journeyTime = 0;
    printf("creating car %d\n", c->id);
    return c;
}//end createCar

/*
** Moves the cars forwards in the cells array using a second temporary array
*/
void moveCars()
{
    printf("Moving cars\n");
    int i;
    //loop through array
    for(i = 0; i < SIZE; i++)
    {
        if(cells[i] != NULL)
        {
            //set the car's nextPosition based off its speed
            cells[i]->nextPosition = cells[i]->position + cells[i]->velocity;
            //check that the nextPosition isn't outside the array
            if(!(cells[i]->nextPosition >= SIZE))
            {
                //change car's position field
                cells[i]->position = cells[i]->nextPosition;
                //put the car in its new cell in the temporary array
                nextGen[cells[i]->nextPosition] = cells[i];
            }
            //reset the old cell
            cells[i] = NULL;
        }//end if
    }//end for loop
    //loop through nextGen array and transfer cars back to cells
    for(i=0; i < SIZE; i++)
    {
        cells[i] = nextGen[i];
        nextGen[i] = NULL;
    }
}//end moveCars

int main ()
{
    int iteration = 0;
    srand(time(NULL));
    bool run = true;

    //clear the contents of the results file.
    outputFile = fopen("results.dat", "w");
    fclose(outputFile);
    //open results file for adding data
    outputFile = fopen("results.dat", "a");

    //initialise results array with default value
    int s;
    int gg;
    for(s = 0; s < SIZE; s++)
    {
        for(gg=0; gg<GMAX; gg++)
        {
            results[s][gg] = -1;
        }
    }

    while(run == true)
    {
        iteration++;
        //create cars until we meet the designated number
        if(generation < GMAX)
        {
            //check that no car is in the first cell
            if(cells[0] == NULL)
            {
                generation++;
                struct Car *car = createCar();
                cells[0] = car;
            }
        }
        //increase the speed of cars (no movement yet)
        accelerationRule();
        //introduce an element of randomness to the driving speeds
        randomisationRule();
        //slow cars down to avoid collisions
        brakeRule();
        //move the cars to their next positions
        moveCars();
        //end while loop if the cars have all passed through the cells
        if(isArrayEmpty())
        {
            run = false;
            continue;
        }
        else
        {
            //loop to display car details after each run
            int i;
            for(i = 0; i < SIZE; i++)
            {
                if(cells[i] != NULL)
                {
                    /*
                    output car's position & the iteration number, x and y values for GNUplot
                    */
                    int id = cells[i]->id - 1;
                    results[iteration - 1][id] = cells[i]->position;
                }
            }
        }//end else
    }//end while

	int g;
	int t;
	for(g = 0; g < GMAX; g++)
    {
        for(t = 0; t < SIZE; t++)
        {
            if(results[t][g] != -1)
            {

                int p = results[t][g];

                fprintf(outputFile, "%d %d\n", t, p);
            }
        }
        fprintf(outputFile, "\n");
    }
    printf("\nDone writing to file \"results.dat\", closing.");
    fclose(outputFile);
	return 0;
}//end main
