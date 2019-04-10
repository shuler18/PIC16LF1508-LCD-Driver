/********************************** Game Of Life ******************************/

#include "gol.h"
#include "lcd_display.h"

unsigned long board = 0b0011100010000000;
unsigned long copy_board = 0b0011100010000000;
void evolve()
{
	short x, y, count;
	for(x = 0; x < 5; x++)
	{
		for(y = 0; y < 5; y++)
		{
			count = count_neighbors(x,y);
			if(count == 3 && get_cell_value(x,y) == 0)
			{
				copy_board |= (0x01UL<<(5*x+y)); //the conversion to UL is needed for values > 16
			}
			else if((count < 2 || count > 3) && get_cell_value(x,y) == 1)
			{
				copy_board &= ~(0x01UL<<(5*x+y));
			}
		}
	}
	board = copy_board;
}

short count_neighbors(short x, short y)
{
	short count = 0;
	count += get_cell_value(x-1,y-1);
	count += get_cell_value(x-1,y);
	count += get_cell_value(x-1,y+1);
	count += get_cell_value(x,y-1);
	count += get_cell_value(x,y+1);
	count += get_cell_value(x+1,y-1);
	count += get_cell_value(x+1,y);
	count += get_cell_value(x+1,y+1);
	return count;
}

short get_cell_value(short x, short y)
{
	//ensure valid coordinates
	if(x >= 0 && x <= 4 && y >= 0 && y <= 4)
	{
		return ((board & (0x01UL << (x*5+y))) >> (x*5+y));
	}
	else
	{
		return 0;
	}
}



void print_board()
{
    short i,j;
    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < 5; j++)
        {
            draw_cell(i,j, get_cell_value(i,j)); 
        }
    }
    
}


/* Function to draw a cell if it is alive
 * x and y are the indices of a living cell
 * there is a 22 pixel padding added to each side of the board, so the x coor
 * must be converted to account for this
 * 
 * A cell is 8x8 pixels, so the loop draws 8 filled cols
 */
void draw_cell(short x, short y, short val)
{
    short i;
    set_xy(21+(y*8),x); //this is wack, but the x and y values are actually flipped
    DC = 1; //sending data
    for(i = 0; i < 8; i++)
    {
        if(val == 1)
        {
            SPI_write_data(0xFF); //fill column
        }
        else
        {
            SPI_write_data(0); //fill column
        }
    }
}

