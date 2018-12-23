// BASED on the project of CHR https://www.instructables.com/id/Led-Cube-8x8x8/ Thank you for enhancing my understandig of electronics!

#include <SPI.h>
#include <avr/interrupt.h>
#include <string.h>
#define AXIS_X 1
#define AXIS_Y 2
#define AXIS_Z 3

volatile unsigned char cube[8][8];
volatile int current_layer = 0;

//const int clockPin = 13; // SPI
//const int dataPin = 11; // SPI
const int latchPin = 0x04; // // 0b00001000; // ports (15,14) 13 12 11 10 9 8 ( 0x04 == pin 10)
const int enablePin = 0x02; // 0b00000010; // ports (15,14) 13 12 11 10 9 8 ( 0x02 == pin 9)

void setup()
{
  for(int i=0; i<14; i++)
    pinMode(i, OUTPUT);

  // pinMode(A0, OUTPUT) as specified in the arduino reference didn't work. So I accessed the registers directly.
  DDRC = 0xff;
  PORTC = 0x00;
  PORTB = 0x00;
  PORTD = 0x00;

  // Reset any PWM configuration that the arduino may have set up automagically!
  TCCR2A = 0x00;
  TCCR2B = 0x00;

  TCCR2A |= (0x01 << WGM21); // CTC mode. clear counter on TCNT2 == OCR2A
  OCR2A = 10; // Interrupt every 25600th cpu cycle (256*100)
  TCNT2 = 0x00; // start counting at 0
  TCCR2B |= (0x01 << CS22) | (0x01 << CS21); // Start the clock with a 256 prescaler

  TIMSK2 |= (0x01 << OCIE2A);
  
  SPI.begin();
  SPI.setDataMode(SPI_MODE0); 
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
}


ISR (TIMER2_COMPA_vect)
{
  PORTB = enablePin; // shutdown output of 74hc595 chip 
  
  for (int i=0; i<8; i++)  SPI.transfer(cube[current_layer][i]);
  SPI.transfer((0x80 >> current_layer)); // start at the top shit to next layer
  
  PORTB = latchPin;
  if (++current_layer == 8)
    current_layer = 0;
}

void loop()
{  
   effect_loadbar(5000);
  
   effect_planboing(AXIS_Z, 400);
   effect_planboing(AXIS_Y, 400);
   effect_planboing(AXIS_X, 400);
       
   
   effect_blinky2();  
   effect_random_filler(75,1);
   effect_random_filler(75,0);
       
   effect_rain(100);
    
   effect_boxside_randsend_parallel (AXIS_X, 0, 150, 1);
   effect_boxside_randsend_parallel (AXIS_X, 1, 150, 1);
   effect_boxside_randsend_parallel (AXIS_Y, 0, 150, 1);
   effect_boxside_randsend_parallel (AXIS_Y, 1, 150, 1);
   effect_boxside_randsend_parallel (AXIS_Z, 0, 150, 1);
   effect_boxside_randsend_parallel (AXIS_Z, 1, 150, 1);
   
   effect_stringfly2(" FLOREN ");
}

void tester()
{
  for (int i=0; i < 8 ;i++) cube[i][i] = (0xFF << i);
}


// ==========================================================================================
//   Effect functions
// ==========================================================================================

void draw_positions_axis (char axis, unsigned char positions[64], int invert)
{
  int x, y, p;

  fill(0x00);

  for (x=0; x<8; x++)
  {
    for (y=0; y<8; y++)
    {
      if (invert)
      {
        p = (7-positions[(x*8)+y]);
      } 
      else
      {
        p = positions[(x*8)+y];
      }

      if (axis == AXIS_Z)
        setvoxel(x,y,p);

      if (axis == AXIS_Y)
        setvoxel(x,p,y);

      if (axis == AXIS_X)
        setvoxel(p,y,x);
    }
  }

}


void effect_boxside_randsend_parallel (char axis, int origin, int delay, int mode)
{
  int i;
  int done;
  unsigned char cubepos[64];
  unsigned char pos[64];
  int notdone = 1;
  int notdone2 = 1;
  int sent = 0;

  for (i=0;i<64;i++)
  {
    pos[i] = 0;
  }

  while (notdone)
  {
    if (mode == 1)
    {
      notdone2 = 1;
      while (notdone2 && sent<64)
      {
        i = rand()%64;
        if (pos[i] == 0)
        {
          sent++;
          pos[i] += 1;
          notdone2 = 0;
        }
      }
    } 
    else if (mode == 2)
    {
      if (sent<64)
      {
        pos[sent] += 1;
        sent++;
      }
    }

    done = 0;
    for (i=0;i<64;i++)
    {
      if (pos[i] > 0 && pos[i] <7)
      {
        pos[i] += 1;
      }

      if (pos[i] == 7)
        done++;
    }

    if (done == 64)
      notdone = 0;

    for (i=0;i<64;i++)
    {
      if (origin == 0)
      {
        cubepos[i] = pos[i];
      } 
      else
      {
        cubepos[i] = (7-pos[i]);
      }
    }


    delay_ms(delay);
    draw_positions_axis(axis,cubepos,0);

  }

}


void effect_rain (int iterations)
{
  int i, ii;
  int rnd_x;
  int rnd_y;
  int rnd_num;

  for (ii=0;ii<iterations;ii++)
  {
    rnd_num = rand()%4;

    for (i=0; i < rnd_num;i++)
    {
      rnd_x = rand()%8;
      rnd_y = rand()%8;
      setvoxel(rnd_x,rnd_y,7);
    }

    delay_ms(1000);
    shift(AXIS_Z,-1);
  }
}

// Set or clear exactly 512 voxels in a random order.
void effect_random_filler (int delay, int state)
{
  int x,y,z;
  int loop = 0;


  if (state == 1)
  {
    fill(0x00);
  } 
  else
  {
    fill(0xff);
  }

  while (loop<511)
  {
    x = rand()%8;
    y = rand()%8;
    z = rand()%8;

    if ((state == 0 && getvoxel(x,y,z) == 0x01) || (state == 1 && getvoxel(x,y,z) == 0x00))
    {
      altervoxel(x,y,z,state);
      delay_ms(delay);
      loop++;
    }	
  }
}


void effect_blinky2()
{
  int i,r;
  fill(0x00);

  for (r=0;r<2;r++)
  {
    i = 750;
    while (i>0)
    {
      fill(0x00);
      delay_ms(i);

      fill(0xff);
      delay_ms(100);

      i = i - (15+(1000/(i/10)));
    }

    delay_ms(1000);

    i = 750;
    while (i>0)
    {
      fill(0x00);
      delay_ms(751-i);

      fill(0xff);
      delay_ms(100);

      i = i - (15+(1000/(i/10)));
    }
  }

}

// Draw a plane on one axis and send it back and forth once.
void effect_planboing (int plane, int speed)
{
  int i;
  for (i=0;i<8;i++)
  {
    fill(0x00);
    setplane(plane, i);
    delay_ms(speed);
  }

  for (i=7;i>=0;i--)
  {
    fill(0x00);
    setplane(plane,i);
    delay_ms(speed);
  }
}

// ==========================================================================================
//   Draw functions
// ==========================================================================================


// Set a single voxel to ON
void setvoxel(int x, int y, int z)
{
  if (inrange(x,y,z))
    cube[z][y] |= (1 << x);
}


// Set a single voxel to ON
void clrvoxel(int x, int y, int z)
{
  if (inrange(x,y,z))
    cube[z][y] &= ~(1 << x);
}



// This function validates that we are drawing inside the cube.
unsigned char inrange(int x, int y, int z)
{
  if (x >= 0 && x < 8 && y >= 0 && y < 8 && z >= 0 && z < 8)
  {
    return 0x01;
  } 
  else
  {
    // One of the coordinates was outside the cube.
    return 0x00;
  }
}

// Get the current status of a voxel
unsigned char getvoxel(int x, int y, int z)
{
  if (inrange(x,y,z))
  {
    if (cube[z][y] & (1 << x))
    {
      return 0x01;
    } 
    else
    {
      return 0x00;
    }
  } 
  else
  {
    return 0x00;
  }
}

// In some effect we want to just take bool and write it to a voxel
// this function calls the apropriate voxel manipulation function.
void altervoxel(int x, int y, int z, int state)
{
  if (state == 1)
  {
    setvoxel(x,y,z);
  } 
  else
  {
    clrvoxel(x,y,z);
  }
}

// Flip the state of a voxel.
// If the voxel is 1, its turned into a 0, and vice versa.
void flpvoxel(int x, int y, int z)
{
  if (inrange(x, y, z))
    cube[z][y] ^= (1 << x);
}

// Makes sure x1 is alwas smaller than x2
// This is usefull for functions that uses for loops,
// to avoid infinite loops
void argorder(int ix1, int ix2, int *ox1, int *ox2)
{
  if (ix1>ix2)
  {
    int tmp;
    tmp = ix1;
    ix1= ix2;
    ix2 = tmp;
  }
  *ox1 = ix1;
  *ox2 = ix2;
}

// Sets all voxels along a X/Y plane at a given point
// on axis Z
void setplane_z (int z)
{
  int i;
  if (z>=0 && z<8)
  {
    for (i=0;i<8;i++)
      cube[z][i] = 0xff;
  }
}

// Clears voxels in the same manner as above
void clrplane_z (int z)
{
  int i;
  if (z>=0 && z<8)
  {
    for (i=0;i<8;i++)
      cube[z][i] = 0x00;
  }
}

void setplane_x (int x)
{
  int z;
  int y;
  if (x>=0 && x<8)
  {
    for (z=0;z<8;z++)
    {
      for (y=0;y<8;y++)
      {
        cube[z][y] |= (1 << x);
      }
    }
  }
}

void clrplane_x (int x)
{
  int z;
  int y;
  if (x>=0 && x<8)
  {
    for (z=0;z<8;z++)
    {
      for (y=0;y<8;y++)
      {
        cube[z][y] &= ~(1 << x);
      }
    }
  }
}

void setplane_y (int y)
{
  int z;
  if (y>=0 && y<8)
  {
    for (z=0;z<8;z++)
      cube[z][y] = 0xff;
  } 
}

void clrplane_y (int y)
{
  int z;
  if (y>=0 && y<8)
  {
    for (z=0;z<8;z++)
      cube[z][y] = 0x00; 
  }
}

void setplane (char axis, unsigned char i)
{
  switch (axis)
  {
  case AXIS_X:
    setplane_x(i);
    break;

  case AXIS_Y:
    setplane_y(i);
    break;

  case AXIS_Z:
    setplane_z(i);
    break;
  }
}

void clrplane (char axis, unsigned char i)
{
  switch (axis)
  {
  case AXIS_X:
    clrplane_x(i);
    break;

  case AXIS_Y:
    clrplane_y(i);
    break;

  case AXIS_Z:
    clrplane_z(i);
    break;
  }
}

// Fill a value into all 64 byts of the cube buffer
// Mostly used for clearing. fill(0x00)
// Off on Off 0b0101010 fill(0x55)
// or setting all on. fill(0xff)
void fill (unsigned char pattern)
{
  int z;
  int y;
  for (z=0;z<8;z++)
  {
    for (y=0;y<8;y++)
    {
      cube[z][y] = pattern;
    }
  }
}



// Draw a box with all walls drawn and all voxels inside set
void box_filled(int x1, int y1, int z1, int x2, int y2, int z2)
{
  int iy;
  int iz;

  argorder(x1, x2, &x1, &x2);
  argorder(y1, y2, &y1, &y2);
  argorder(z1, z2, &z1, &z2);

  for (iz=z1;iz<=z2;iz++)
  {
    for (iy=y1;iy<=y2;iy++)
    {
      cube[iz][iy] |= byteline(x1,x2);
    }
  }

}

// Darw a hollow box with side walls.
void box_walls(int x1, int y1, int z1, int x2, int y2, int z2)
{
  int iy;
  int iz;

  argorder(x1, x2, &x1, &x2);
  argorder(y1, y2, &y1, &y2);
  argorder(z1, z2, &z1, &z2);

  for (iz=z1;iz<=z2;iz++)
  {
    for (iy=y1;iy<=y2;iy++)
    {	
      if (iy == y1 || iy == y2 || iz == z1 || iz == z2)
      {
        cube[iz][iy] = byteline(x1,x2);
      } 
      else
      {
        cube[iz][iy] |= ((0x01 << x1) | (0x01 << x2));
      }
    }
  }

}

// Draw a wireframe box. This only draws the corners and edges,
// no walls.
void box_wireframe(int x1, int y1, int z1, int x2, int y2, int z2)
{
  int iy;
  int iz;

  argorder(x1, x2, &x1, &x2);
  argorder(y1, y2, &y1, &y2);
  argorder(z1, z2, &z1, &z2);

  // Lines along X axis
  cube[z1][y1] = byteline(x1,x2);
  cube[z1][y2] = byteline(x1,x2);
  cube[z2][y1] = byteline(x1,x2);
  cube[z2][y2] = byteline(x1,x2);

  // Lines along Y axis
  for (iy=y1;iy<=y2;iy++)
  {
    setvoxel(x1,iy,z1);
    setvoxel(x1,iy,z2);
    setvoxel(x2,iy,z1);
    setvoxel(x2,iy,z2);
  }

  // Lines along Z axis
  for (iz=z1;iz<=z2;iz++)
  {
    setvoxel(x1,y1,iz);
    setvoxel(x1,y2,iz);
    setvoxel(x2,y1,iz);
    setvoxel(x2,y2,iz);
  }

}

// Returns a byte with a row of 1's drawn in it.
// byteline(2,5) gives 0b00111100
char byteline (int start, int end)
{
  return ((0xff<<start) & ~(0xff<<(end+1)));
}

// Flips a byte 180 degrees.
// MSB becomes LSB, LSB becomes MSB.
char flipbyte (char byte)
{
  char flop = 0x00;

  flop = (flop & 0b11111110) | (0b00000001 & (byte >> 7));
  flop = (flop & 0b11111101) | (0b00000010 & (byte >> 5));
  flop = (flop & 0b11111011) | (0b00000100 & (byte >> 3));
  flop = (flop & 0b11110111) | (0b00001000 & (byte >> 1));
  flop = (flop & 0b11101111) | (0b00010000 & (byte << 1));
  flop = (flop & 0b11011111) | (0b00100000 & (byte << 3));
  flop = (flop & 0b10111111) | (0b01000000 & (byte << 5));
  flop = (flop & 0b01111111) | (0b10000000 & (byte << 7));
  return flop;
}

// Draw a line between any coordinates in 3d space.
// Uses integer values for input, so dont expect smooth animations.
void line(int x1, int y1, int z1, int x2, int y2, int z2)
{
  float xy;	// how many voxels do we move on the y axis for each step on the x axis
  float xz;	// how many voxels do we move on the y axis for each step on the x axis 
  unsigned char x,y,z;
  unsigned char lasty,lastz;

  // We always want to draw the line from x=0 to x=7.
  // If x1 is bigget than x2, we need to flip all the values.
  if (x1>x2)
  {
    int tmp;
    tmp = x2; 
    x2 = x1; 
    x1 = tmp;
    tmp = y2; 
    y2 = y1; 
    y1 = tmp;
    tmp = z2; 
    z2 = z1; 
    z1 = tmp;
  }


  if (y1>y2)
  {
    xy = (float)(y1-y2)/(float)(x2-x1);
    lasty = y2;
  } 
  else
  {
    xy = (float)(y2-y1)/(float)(x2-x1);
    lasty = y1;
  }

  if (z1>z2)
  {
    xz = (float)(z1-z2)/(float)(x2-x1);
    lastz = z2;
  } 
  else
  {
    xz = (float)(z2-z1)/(float)(x2-x1);
    lastz = z1;
  }



  // For each step of x, y increments by:
  for (x = x1; x<=x2;x++)
  {
    y = (xy*(x-x1))+y1;
    z = (xz*(x-x1))+z1;
    setvoxel(x,y,z);
  }

}

// Delay loop.
// This is not calibrated to milliseconds,
// but we had allready made to many effects using this
// calibration when we figured it might be a good idea
// to calibrate it.
void delay_ms(uint16_t x)
{
  uint8_t y, z;
  for ( ; x > 0 ; x--){
    for ( y = 0 ; y < 90 ; y++){
      for ( z = 0 ; z < 6 ; z++){
        asm volatile ("nop");
      }
    }
  }
}



// Shift the entire contents of the cube along an axis
// This is great for effects where you want to draw something
// on one side of the cube and have it flow towards the other
// side. Like rain flowing down the Z axiz.
void shift (char axis, int direction)
{
  int i, x ,y;
  int ii, iii;
  int state;

  for (i = 0; i < 8; i++)
  {
    if (direction == -1)
    {
      ii = i;
    } 
    else
    {
      ii = (7-i);
    }	


    for (x = 0; x < 8; x++)
    {
      for (y = 0; y < 8; y++)
      {
        if (direction == -1)
        {
          iii = ii+1;
        } 
        else
        {
          iii = ii-1;
        }

        if (axis == AXIS_Z)
        {
          state = getvoxel(x,y,iii);
          altervoxel(x,y,ii,state);
        }

        if (axis == AXIS_Y)
        {
          state = getvoxel(x,iii,y);
          altervoxel(x,ii,y,state);
        }

        if (axis == AXIS_X)
        {
          state = getvoxel(iii,y,x);
          altervoxel(ii,y,x,state);
        }
      }
    }
  }

  if (direction == -1)
  {
    i = 7;
  } 
  else
  {
    i = 0;
  }	

  for (x = 0; x < 8; x++)
  {
    for (y = 0; y < 8; y++)
    {
      if (axis == AXIS_Z)
        clrvoxel(x,y,i);

      if (axis == AXIS_Y)
        clrvoxel(x,i,y);

      if (axis == AXIS_X)
        clrvoxel(i,y,x);
    }
  }
}

#define EEMEM
#define PROGMEM

volatile const unsigned char font[455] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x5f,0x5f,0x00,0x00,	//  !
	0x00,0x03,0x00,0x03,0x00,0x14,0x7f,0x14,0x7f,0x14,	// "#
	0x24,0x2a,0x7f,0x2a,0x12,0x23,0x13,0x08,0x64,0x62,	// $%
	0x36,0x49,0x55,0x22,0x50,0x00,0x05,0x03,0x00,0x00,	// &'
	0x00,0x1c,0x22,0x41,0x00,0x00,0x41,0x22,0x1c,0x00,	// ()
	0x14,0x08,0x3e,0x08,0x14,0x08,0x08,0x3e,0x08,0x08,	// *+
	0x00,0x50,0x30,0x00,0x00,0x08,0x08,0x08,0x08,0x08,	// ,-
	0x00,0x60,0x60,0x00,0x00,0x20,0x10,0x08,0x04,0x02,	// ./
	0x3e,0x51,0x49,0x45,0x3e,0x00,0x42,0x7f,0x40,0x00,	// 01
	0x42,0x61,0x51,0x49,0x46,0x21,0x41,0x45,0x4b,0x31,	// 23
	0x18,0x14,0x12,0x7f,0x10,0x27,0x45,0x45,0x45,0x39,	// 45
	0x3c,0x4a,0x49,0x49,0x30,0x01,0x71,0x09,0x05,0x03,	// 67
	0x36,0x49,0x49,0x49,0x36,0x06,0x49,0x49,0x29,0x1e,	// 89
	0x00,0x36,0x36,0x00,0x00,0x00,0x56,0x36,0x00,0x00,	// :;
	0x08,0x14,0x22,0x41,0x00,0x14,0x14,0x14,0x14,0x14,	// <=
	0x00,0x41,0x22,0x14,0x08,0x02,0x01,0x51,0x09,0x06,	// >?
	0x32,0x49,0x79,0x41,0x3e,0x7e,0x11,0x11,0x11,0x7e,	// @A
	0x7f,0x49,0x49,0x49,0x36,0x3e,0x41,0x41,0x41,0x22,	// BC
	0x7f,0x41,0x41,0x22,0x1c,0x7f,0x49,0x49,0x49,0x41,	// DE
	0x7f,0x09,0x09,0x09,0x01,0x3e,0x41,0x49,0x49,0x7a,	// FG
	0x7f,0x08,0x08,0x08,0x7f,0x00,0x41,0x7f,0x41,0x00,	// HI
	0x20,0x40,0x41,0x3f,0x01,0x7f,0x08,0x14,0x22,0x41,	// JK
	0x7f,0x40,0x40,0x40,0x40,0x7f,0x02,0x0c,0x02,0x7f,	// LM
	0x7f,0x04,0x08,0x10,0x7f,0x3e,0x41,0x41,0x41,0x3e,	// NO
	0x7f,0x09,0x09,0x09,0x06,0x3e,0x41,0x51,0x21,0x5e,	// PQ
	0x7f,0x09,0x19,0x29,0x46,0x46,0x49,0x49,0x49,0x31,	// RS
	0x01,0x01,0x7f,0x01,0x01,0x3f,0x40,0x40,0x40,0x3f,	// TU
	0x1f,0x20,0x40,0x20,0x1f,0x3f,0x40,0x38,0x40,0x3f,	// VW
	0x63,0x14,0x08,0x14,0x63,0x07,0x08,0x70,0x08,0x07,	// XY
	0x61,0x51,0x49,0x45,0x43,0x00,0x7f,0x41,0x41,0x00,	// Z[
	0x02,0x04,0x08,0x10,0x20,0x00,0x41,0x41,0x7f,0x00,	// \]
	0x04,0x02,0x01,0x02,0x04,0x40,0x40,0x40,0x40,0x40,	// ^_
	0x00,0x01,0x02,0x04,0x00,0x20,0x54,0x54,0x54,0x78,	// `a
	0x7f,0x48,0x44,0x44,0x38,0x38,0x44,0x44,0x44,0x20,	// bc
	0x38,0x44,0x44,0x48,0x7f,0x38,0x54,0x54,0x54,0x18,	// de
	0x08,0x7e,0x09,0x01,0x02,0x0c,0x52,0x52,0x52,0x3e,	// fg
	0x7f,0x08,0x04,0x04,0x78,0x00,0x44,0x7d,0x40,0x00,	// hi
	0x20,0x40,0x44,0x3d,0x00,0x7f,0x10,0x28,0x44,0x00,	// jk
	0x00,0x41,0x7f,0x40,0x00,0x7c,0x04,0x18,0x04,0x78,	// lm
	0x7c,0x08,0x04,0x04,0x78,0x38,0x44,0x44,0x44,0x38,	// no
	0x7c,0x14,0x14,0x14,0x08,0x08,0x14,0x14,0x18,0x7c,	// pq
	0x7c,0x08,0x04,0x04,0x08,0x48,0x54,0x54,0x54,0x20,	// rs
	0x04,0x3f,0x44,0x40,0x20,0x3c,0x40,0x40,0x20,0x7c,	// tu
	0x1c,0x20,0x40,0x20,0x1c,0x3c,0x40,0x30,0x40,0x3c,	// vw
	0x44,0x28,0x10,0x28,0x44,0x0c,0x50,0x50,0x50,0x3c,	// xy
	0x44,0x64,0x54,0x4c,0x44				// z
};


volatile const unsigned char bitmaps[6][8] EEMEM = {
	{0xc3,0xc3,0x00,0x18,0x18,0x81,0xff,0x7e}, // smiley 3 small
	{0x3c,0x42,0x81,0x81,0xc3,0x24,0xa5,0xe7}, // Omega
	{0x00,0x04,0x06,0xff,0xff,0x06,0x04,0x00},  // Arrow
	{0x81,0x42,0x24,0x18,0x18,0x24,0x42,0x81}, // X
	{0xBD,0xA1,0xA1,0xB9,0xA1,0xA1,0xA1,0x00}, // ifi
	{0xEF,0x48,0x4B,0x49,0x4F,0x00,0x00,0x00} // TG
};

const unsigned char paths[44] PROGMEM = {0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x67,0x57,0x47,0x37,0x27,0x17,
0x04,0x03,0x12,0x21,0x30,0x40,0x51,0x62,0x73,0x74,0x65,0x56,0x47,0x37,0x26,0x15}; // circle, len 16, offset 28

void font_getpath (unsigned char path, unsigned char *destination, int length)
{
	int i;
	int offset = 0;
	
	if (path == 1)
		offset=28;
	
	//for (i = 0; i < length; i++)
	//	destination[i] = pgm_read_byte(&paths[i+offset]);
}

void font_getchar (char chr, unsigned char dst[5])
{
	int i;
	chr -= 32;

    unsigned char font[455] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x5f,0x5f,0x00,0x00,	//  !
	0x00,0x03,0x00,0x03,0x00,0x14,0x7f,0x14,0x7f,0x14,	// "#
	0x24,0x2a,0x7f,0x2a,0x12,0x23,0x13,0x08,0x64,0x62,	// $%
	0x36,0x49,0x55,0x22,0x50,0x00,0x05,0x03,0x00,0x00,	// &'
	0x00,0x1c,0x22,0x41,0x00,0x00,0x41,0x22,0x1c,0x00,	// ()
	0x14,0x08,0x3e,0x08,0x14,0x08,0x08,0x3e,0x08,0x08,	// *+
	0x00,0x50,0x30,0x00,0x00,0x08,0x08,0x08,0x08,0x08,	// ,-
	0x00,0x60,0x60,0x00,0x00,0x20,0x10,0x08,0x04,0x02,	// ./
	0x3e,0x51,0x49,0x45,0x3e,0x00,0x42,0x7f,0x40,0x00,	// 01
	0x42,0x61,0x51,0x49,0x46,0x21,0x41,0x45,0x4b,0x31,	// 23
	0x18,0x14,0x12,0x7f,0x10,0x27,0x45,0x45,0x45,0x39,	// 45
	0x3c,0x4a,0x49,0x49,0x30,0x01,0x71,0x09,0x05,0x03,	// 67
	0x36,0x49,0x49,0x49,0x36,0x06,0x49,0x49,0x29,0x1e,	// 89
	0x00,0x36,0x36,0x00,0x00,0x00,0x56,0x36,0x00,0x00,	// :;
	0x08,0x14,0x22,0x41,0x00,0x14,0x14,0x14,0x14,0x14,	// <=
	0x00,0x41,0x22,0x14,0x08,0x02,0x01,0x51,0x09,0x06,	// >?
	0x32,0x49,0x79,0x41,0x3e,0x7e,0x11,0x11,0x11,0x7e,	// @A
	0x7f,0x49,0x49,0x49,0x36,0x3e,0x41,0x41,0x41,0x22,	// BC
	0x7f,0x41,0x41,0x22,0x1c,0x7f,0x49,0x49,0x49,0x41,	// DE
	0x7f,0x09,0x09,0x09,0x01,0x3e,0x41,0x49,0x49,0x7a,	// FG
	0x7f,0x08,0x08,0x08,0x7f,0x00,0x41,0x7f,0x41,0x00,	// HI
	0x20,0x40,0x41,0x3f,0x01,0x7f,0x08,0x14,0x22,0x41,	// JK
	0x7f,0x40,0x40,0x40,0x40,0x7f,0x02,0x0c,0x02,0x7f,	// LM
	0x7f,0x04,0x08,0x10,0x7f,0x3e,0x41,0x41,0x41,0x3e,	// NO
	0x7f,0x09,0x09,0x09,0x06,0x3e,0x41,0x51,0x21,0x5e,	// PQ
	0x7f,0x09,0x19,0x29,0x46,0x46,0x49,0x49,0x49,0x31,	// RS
	0x01,0x01,0x7f,0x01,0x01,0x3f,0x40,0x40,0x40,0x3f,	// TU
	0x1f,0x20,0x40,0x20,0x1f,0x3f,0x40,0x38,0x40,0x3f,	// VW
	0x63,0x14,0x08,0x14,0x63,0x07,0x08,0x70,0x08,0x07,	// XY
	0x61,0x51,0x49,0x45,0x43,0x00,0x7f,0x41,0x41,0x00,	// Z[
	0x02,0x04,0x08,0x10,0x20,0x00,0x41,0x41,0x7f,0x00,	// \]
	0x04,0x02,0x01,0x02,0x04,0x40,0x40,0x40,0x40,0x40,	// ^_
	0x00,0x01,0x02,0x04,0x00,0x20,0x54,0x54,0x54,0x78,	// `a
	0x7f,0x48,0x44,0x44,0x38,0x38,0x44,0x44,0x44,0x20,	// bc
	0x38,0x44,0x44,0x48,0x7f,0x38,0x54,0x54,0x54,0x18,	// de
	0x08,0x7e,0x09,0x01,0x02,0x0c,0x52,0x52,0x52,0x3e,	// fg
	0x7f,0x08,0x04,0x04,0x78,0x00,0x44,0x7d,0x40,0x00,	// hi
	0x20,0x40,0x44,0x3d,0x00,0x7f,0x10,0x28,0x44,0x00,	// jk
	0x00,0x41,0x7f,0x40,0x00,0x7c,0x04,0x18,0x04,0x78,	// lm
	0x7c,0x08,0x04,0x04,0x78,0x38,0x44,0x44,0x44,0x38,	// no
	0x7c,0x14,0x14,0x14,0x08,0x08,0x14,0x14,0x18,0x7c,	// pq
	0x7c,0x08,0x04,0x04,0x08,0x48,0x54,0x54,0x54,0x20,	// rs
	0x04,0x3f,0x44,0x40,0x20,0x3c,0x40,0x40,0x20,0x7c,	// tu
	0x1c,0x20,0x40,0x20,0x1c,0x3c,0x40,0x30,0x40,0x3c,	// vw
	0x44,0x28,0x10,0x28,0x44,0x0c,0x50,0x50,0x50,0x3c,	// xy
	0x44,0x64,0x54,0x4c,0x44				// z
    };
    for (i = 0; i < 5; i++)
    {
		dst[i] = font[(chr*5)+i];
	}
	
}

void font_getbitmap (char bitmap, unsigned char dst[8])
{
	int i;
	
        //for (i = 0; i < 8; i++)
	//	dst[i] = eeprom_read_byte(&bitmaps[bitmap][i]);
	
}

unsigned char font_getbitmappixel ( char bitmap, char x, char y)
{
	//unsigned char tmp = eeprom_read_byte(&bitmaps[bitmap][x]);
	//return (tmp >> y) & 0x01;
}

void effect_loadbar(int delay)
{
	fill(0x00);
	
	int z,y;
	
	for (z=0;z<8;z++)
	{
		for (y=0;y<8;y++)
			cube[z][y] = 0xff;
			
		delay_ms(delay);
	}
	
	delay_ms(delay*3);
	
	for (z=0;z<8;z++)
	{
		for (y=0;y<8;y++)
			cube[z][y] = 0x00;
			
		delay_ms(delay);
	}
}

void effect_stringfly2(char * str)
{
	int x,y,i;
	unsigned char chr[5];
	
	while (*str)
	{
		font_getchar(*str++, chr);
		
        // Put a character on the back of the cube
		for (x = 0; x < 5; x++)
		{
			for (y = 0; y < 8; y++)
			{
				if ((chr[x] & (0x80>>y)))
				{
					setvoxel(7,x+2,y);
				}
			}
		}
		
        // Shift the entire contents of the cube forward by 6 steps
        // before placing the next character
		for (i = 0; i<6; i++)
		{
			delay_ms(1000);
            shift(AXIS_X,-1);
		}
	}
    // Shift the last character out of the cube.
	for (i = 0; i<8; i++)
	{
		delay_ms(1000);
        shift(AXIS_X,-1);
	}
	
}
