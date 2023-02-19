#define STRIP_PIN 9

#include <microLED.h>
#include <IRremote.h> //IR датчик
microLED<0, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;

#define LEDSCOUNT 999
#define MAXPOWER 255
int DELAY = 1;
int RAND = 1;
int FREQ = 1;
int currColor = 1;

int rr, gg, bb;

int colors[11][3] = {
  {248, 240, 255},
  {102, 153, 204},
  {235, 250, 215},
  {0, 255, 255},
  {255, 127, 212},
  {240, 255, 255},
  {245, 245, 220},
  {228, 255, 196},
  {43, 138, 226},
  {42, 165, 42},
  {42, 165, 42}
};
uint8_t fader[LEDSCOUNT];

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(7);
  for (int i = 0; i < LEDSCOUNT; i++) {
    fader[i] = 0;
  }
}

void loop() {

  IRControl();

  signed int r, g, b;

  //Если фэйдер от 1 до 128 - нарастает. Если больше - убывает. Если 255 - сбрасываем в 1
  int rnd = random(RAND);
  int ok = false;

  if (rnd == 0) {
    rnd = random(LEDSCOUNT); //Выбрали случайный диод из коллекции

    if (fader[rnd] != 0) { //Мы попали в светодиод, который сейчас уже зажжен. Ищем его ближайших незаженных соседей
      for (int j = rnd; j < LEDSCOUNT; j++) {
        if (fader[j] == 0) {
          ok = true;
          rnd = j;
          break;
        }
      }

      if (!ok) { //Мы сходили вниз, но ничего не нашли. Идем с верха!
        for (int j = 0; j < rnd; j++) {
          if (fader[j] == 0) {
            ok = true;
            rnd = j;
            break;
          }
        }

      }
    } else ok = true;

    if (ok) {
      //Приняли решение зажечь диод. Если flash == 1, то зажигаем его быстро
      int flash = random(10);
      int pos = 5;
      if (flash == 1) pos = 120;
      fader[rnd] = pos;
    }
  }


  strip.begin();

  for (int i = 0; i < LEDSCOUNT; i++) {

    if (fader[i] >= 250) {
      fader[i] = 0;

    }

    if (fader[i] > 0) {

      int fadeval = 0;

      fader[i]++;

      //Мы на пике света?
      if (fader[i] == 128) {

        //Рандом, от1  до 10 горим или таем
        int down = random(0, 1000);
        if (down > 0) {
          fader[i]--;
        }
      }


      if (fader[i] < 128) {
        fadeval = fader[i] * 2;

        if (r < 255 && g < 255 && b < 255)
        {
          r = colors[currColor][0] - 255 + fadeval; if (r < 0) r = 0;
          g = colors[currColor][1] - 255 + fadeval; if (g < 0) g = 0;
          b = colors[currColor][2] - 255 + fadeval; if (b < 0) b = 0;
        } else {
          r = colors[currColor][0];
          g = colors[currColor][1];
          b = colors[currColor][2];
        }


      } else {


        fadeval = 255 - ((fader[i] - 128) * 2);

        r = colors[currColor][0] - 255 + fadeval; if (r < 0) r = 0;
        g = colors[currColor][1] - 255 + fadeval; if (g < 0) g = 0;
        b = colors[currColor][2] - 255 + fadeval; if (b < 0) b = 0;

      }


    } else {

      r = 0; g = 0; b = 0;

    }



    strip.send(mRGB(r, g, b));

  }


  strip.end();

  delay(DELAY);

}





void IRControl()
{
  if (IrReceiver.decode()) {
    int command = IrReceiver.decodedIRData.command;

    //  Serial.println(command);

    switch (command) {
      case 24://ввехр
        if (DELAY > 1) DELAY = DELAY - 1;
        break;
      case 82://вниз
        DELAY = DELAY + 1;
        break;
      case 69://1
        currColor = 1;
        break;
      case 70://2
        currColor = 2;
        break;
      case 71://3
        currColor = 3;
        break;
      case 68://4
        currColor = 4;
        break;
      case 64://5
        currColor = 5;
        break;
      case 67://6
        currColor = 6;
        break;
      case 7://7
        currColor = 7;
        break;
      case 21://8
        currColor = 8;
        break;
      case 9://9
        currColor = 9;
        break;
      case 25://0
        currColor = 10;
        break;

      case 90://право
        if (RAND > 6) RAND = RAND - 5;
        break;
      case 8://лево
        RAND = RAND + 5;
        break;
      case 22://*
        if (FREQ > 2) FREQ--;
        break;
      case 13://#
        FREQ++;
        break;
    }
    // Serial.print(rr);
    //  Serial.print(" ");
    //  Serial.print(gg);
    //  Serial.print(" ");
    //  Serial.print(bb);
    //  Serial.println(" ");
    // delay (100);
    IrReceiver.resume();
  }
}
