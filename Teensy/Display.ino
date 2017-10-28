int ccwidth = 200;
int ccheight = 100;
int xcorner = 20;
int ycorner = 20;
int numpoints = 20;

int bounds[10][2] = {{ -10, 50}, {90000, 120000}, {10, 90}, {0, 1000}, {0, 10}, {0, 1000}, {0, 1000}, {0, 1000}, {0, 1000}, {0, 1000}};
//                   temp-0   pres-1         hum-2   co-3     ch4-4  co2-5    lpg-6    alc-7     voc-8     part-9
String names[10] = {"Temp", "Pres", "Hum", "CO", "CH4", "CO2", "LPG", "Alc", "VOC", "PART"};
boolean healthy[11] = {true, true, true, true, true, true, true, true, true, true, true}; //last value corresponds to control chart
double data[10] = {13, 100000, 14, 45, 3, 234, 56, 76, 900, 123};
double controlData[50];
int controlVar = 2;
//variable depends on user selection

#include <ILI9341_t3.h>
#include <SPI.h>

#define TFT_DC  9
#define TFT_CS 10
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);

unsigned int colors[8] = {ILI9341_BLACK, ILI9341_RED, ILI9341_GREEN, ILI9341_BLUE, ILI9341_CYAN, ILI9341_YELLOW, ILI9341_WHITE, ILI9341_PURPLE};

double mean(double A[], int len) {
  double sum = 0;
  for (int i = 0; i < len; i++) {
    sum += A[i];
  }

  return sum / ( (double) len);

}
void setState(boolean health) {
  //updates graph with health
  if (health == true) {
    tft.drawRect(0, 0, xcorner / 2, 320, ILI9341_GREEN);
    tft.drawRect(240 - xcorner / 2, 0, xcorner / 2, 320, ILI9341_GREEN);
  }
  else {
    tft.drawRect(0, 0, xcorner / 2, 320, ILI9341_RED);
    tft.drawRect(240 - xcorner / 2, 0, xcorner / 2, 320, ILI9341_RED);
  }

}
double stdev(double A[], double mean, int len) {
  double diffSum = 0;
  for (int i = 0; i < len; i++) {
    diffSum += (A[i] - mean) * (A[i] - mean);
  }
  double stdev_ret = sqrt(diffSum / ( (double) len - 1 ));
  return stdev_ret;
}
double modifiedMap(double x, double in_min, double in_max, double out_min, double out_max)
{
  double temp = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  temp = (int) (4 * temp + .5);
  return (double) temp / 4;
}
void graphPoints(double A[], int len) {
  //returns the x axis positions for which points should be graphed
  int spacing = ccwidth / len;
  double mean_out = mean(A, len);
  double stdev_out = stdev(A, mean_out, len);

  int positionsX[len];
  int positionsY[len];

  for (int i = 0; i < len; i++) {
    positionsX[i] = i * spacing + ycorner;
  }

  for (int i = 0; i < len; i++) {
    positionsY[i] = modifiedMap(A[i], mean_out - 3.0 * stdev_out, mean_out + 3 * stdev_out, 0.0, (double) ccheight) + (double) ycorner;
  }



  for (int i = 0; i < len; i++) {
    if (i != 0)
      tft.drawLine(positionsX[i - 1], positionsY[i - 1], positionsX[i], positionsY[i], ILI9341_CYAN);
  }

}
void drawAxes(char typeName[]) {
  //draws control chart axes
  int spacing = ccheight / 6;
  for (int i = 0; i < 7; i++) {
    tft.drawFastHLine(ycorner, spacing * i + xcorner, 200, ILI9341_PURPLE);
  }
  tft.drawFastHLine(ycorner, spacing * 3 + xcorner, 200, ILI9341_WHITE);
  
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(20, 5);
  tft.print(typeName);
  //tft.drawString("World!!",60,220,4,WHITE);
}
void clearControlData() {
  for (int i = 0; i < 50; i++) {
    controlData[i] = 0;
  }
}

boolean calcState() {
  for (int i = 0; i < 10; i++) {
    if ( (data[i] > bounds[i][0]) && (data[i] < bounds[i][1]) ) healthy[i] = true;
  }

  //healthy[11] = true;

  for (int i = 0; i < 11; i++) {
    if (healthy[i] == false) return false;
  }
  return true;
}

void fetchData() {
  // this function magically summons data from sensors, storing data from each sensor in the relevant positions of the data array.
  readBMP():
  
  float ppm = sampleSensor100() * (3.3 / 1024) * 0.172 - 0.0999;
}

void updateWeb() {
  // this function magically updates the web database with the contents of data
  for (int i = 0; i<10; i++) {
    Serial.print(i);
    Serial.println(data[i]);
  }
}

void updateBounds() {
  // this function magically updates bounds with data from the internet
}

void textFailures(String message) {
  // this function texts the user if an issue is found

}

String texts[10] = {"", "", "", "", "", "", "", "", "", ""};
char nameArr[5] = {'1', '1', '1', '1', '1'};

void drawText() {
  for (int i = 0; i < 10; i++)
    texts[i] = "";
  for (int i = 0; i < 10; i++) {
    int datapoint = (int) data[i];
    Serial.print("datapoint ");
    Serial.print(datapoint);
    texts[i] = String(datapoint).substring(0, 4);
    Serial.print(" data ");
    Serial.print(data[i]);
    Serial.print(" i:");
    Serial.print(i);
    Serial.print(" Text: ");
    Serial.println(texts[i]);
  }
  int drawn = 0;
  for (int i = 0; i < 5; i++)
    nameArr[i] = '1';
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (drawn == controlVar) {
        j -= 1;
      }
      texts[drawn].toCharArray(nameArr, 5);
      Serial.print(j);
      Serial.print(nameArr[0]);
      Serial.println(nameArr[1]);
      tft.setCursor(60 + 50 * j, 175 + 55 * i);
      tft.print(nameArr);
      drawn += 1;
    }
  }
  drawn = 0;
  for (int i = 0; i < 5; i++)
    nameArr[i] = '1';
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (drawn == controlVar) {
        j -= 1;
      }
      names[drawn].toCharArray(nameArr, 5);
      Serial.print(names[drawn]);
      Serial.print(nameArr[0]);
      Serial.println(nameArr[1]);
      tft.setCursor(60 + 50 * j, 155 + 55 * i);
      tft.setTextColor(ILI9341_WHITE);
      tft.print(nameArr);
      drawn += 1;
    }
  }
}

void updateGraph() {
  tft.fillRect(0, 0, 320, 320, ILI9341_BLACK);
  char nameArr[10];
  names[controlVar].toCharArray(nameArr, 10);
  drawAxes(nameArr);
  setState(calcState());

  for (int i = 0; i < 49; i++) {
    controlData[i] = controlData[i + 1];
  }
  controlData[49] = data[controlVar];

  graphPoints(controlData, 50);
  //drawText();
}

void setNewControlChartVar(int var) {
  //Sets a new variable as control chart subject
  clearControlData();
  controlVar = var;
  updateGraph();
}


void setup() {
  // put your setup code here, to run once:
  clearControlData();
  Serial.begin(9600);
  //TFT_BL_ON;                                  // turn on the background light
  tft.begin();                              //init TFT library
  drawText();
}

void loop() {
  // put your main code here, to run repeatedly:
  fetchData();
  updateWeb();
  updateGraph();
  drawText();

  if (calcState() == false) {
    String message = "";
    for (int i = 0; i < 11; i++) {
      if (healthy[i] == false)
        message += ("ERROR: " + names[i] + String(data[i]) + " ");
    }
    textFailures(message);
  }

  delay(1000);

}




