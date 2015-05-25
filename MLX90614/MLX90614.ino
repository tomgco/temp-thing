#include <i2cmaster.h>

byte numbers[11][7] = {
  {1,1,1,1,1,1,0}, //zero
  {0,1,1,0,0,0,0}, //one
  {1,1,0,1,1,0,1}, //two
  {1,1,1,1,0,0,1}, //three
  {0,1,1,0,0,1,1}, //four
  {1,0,1,1,0,1,1}, //five
  {1,0,1,1,1,1,1}, //six
  {1,1,1,0,0,0,0}, //seven
  {1,1,1,1,1,1,1}, //eight
  {1,1,1,1,0,1,1}, //nine
  {0,0,0,0,0,0,0}  //off
};

byte pins[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13};


// Works from 0 - 99
void display(int number) {
  // Least Significant Digit
  for (int i = 0; i < 7; i++){
    digitalWrite(pins[i], !numbers[number % 10][i]);
  }

  // Most Significant Digit
  for (int i = 7; i < 14; i++){
    digitalWrite(pins[i], !numbers[number / 10][i - 7]);
  }
}

void setup(){
  i2c_init(); //Initialise the i2c bus
  PORTC = (1 << PORTC4) | (1 << PORTC5); // enable pullups

  for (int i = 0; i < 14; i++){
    pinMode(pins[i], OUTPUT);
  }
}

void loop(){
  int dev = 0x5A<<1;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  i2c_start_wait(dev+I2C_WRITE);
  // Instruct the sensor to Read sensor 1 at 0x07
  // Sensor 2 is at 0x07
  i2c_write(0x07);

  // read
  i2c_rep_start(dev+I2C_READ);
  data_low = i2c_readAck(); //Read 1 byte and then send ack
  data_high = i2c_readAck(); //Read 1 byte and then send ack
  pec = i2c_readNak();
  i2c_stop();

  //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
  double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)
  double tempData = 0x0000; // zero out the data

  // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  tempData = (tempData * tempFactor)-0.01;

  float celcius = tempData - 273.15;

  // float is casted to int, maybe floor this, but meh.
  display(celcius);
  delay(1000);
}
