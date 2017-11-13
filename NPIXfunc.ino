/*
 * Autonomous NeoPixel Functions
*/
void pixOff() {
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0)); 
  }
  pixels.show();
}

void pixBalance(int bal) {
  switch (bal) {
    case 1: //full balance; all green
    {
      for(int i=1;i<NUMPIXELS;i++) {
        pixSingle_grn(i);
      }
      break;
    }
    case 2: //half balance; half green
    {
      pixOff();
      for(int i=1;i<NUMPIXELS;i++) {
        if (i < 4) {
          pixSingle_grn(i);  
        } else{
          pixSingle_off(i);
        }
      }
      break;
    }
    case 3: //low balance; half yellow
    {
      for(int i=1;i<NUMPIXELS;i++) {
        if (i < 4) {
          pixSingle_yel(i);  
        } else{
          pixSingle_off(i);
        }
      }
      break;
    }
    case 4: //extremely low or negative balance; all red
    {
      for(int i=1;i<NUMPIXELS;i++) {
        pixSingle_red(i);  
      }
      break;
    }
  }
}

/*
 * Single NeoPixel Functions
*/
void pixSingle_grn(int pixel) {
    pixels.setPixelColor(pixel, pixels.Color(223,0,0)); 
    pixels.show();
}

void pixSingle_blu(int pixel) {
    pixels.setPixelColor(pixel, pixels.Color(0,0,223)); 
    pixels.show();
}

void pixSingle_red(int pixel) {
    pixels.setPixelColor(pixel, pixels.Color(0,223,0)); 
    pixels.show();
}

void pixSingle_yel(int pixel) {
    pixels.setPixelColor(pixel, pixels.Color(223,223,0)); 
    pixels.show();
}

void pixSingle_off(int pixel) {
    pixels.setPixelColor(pixel, pixels.Color(0,0,0)); 
    pixels.show();
}

void pixSingle_rand(int pixel) {
  pixels.setPixelColor(pixel, pixels.Color(random(10, 200), random(10,200), random(10,200)));
  pixels.show();
}


