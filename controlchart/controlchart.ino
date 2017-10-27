int ccwidth = 200;
int ccheight = 100;
int xcorner = 200;
int ycorner = 20;
int numpoints = 20;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

double mean(double A[], int len) {
  double sum = 0;
  for (int i = 0; i < len; i++) {
    sum += A[i];
  }
  return sum;
}

double stdev(double A[], double mean, int len) {
  double diffSum = 0;
  for (int i = 0; i < len; i++) {
    diffSum += (A[i] - mean) * (A[i] - mean);
  }
  double stdev_ret = sqrt(diffSum / ( (double) len - 1 ));
  return stdev_ret;
}

void graphPoints(double A[], int len) {
  //returns the x axis positions for which points should be graphed
  int spacing = ccwidth / numpoints;
  double mean_out = mean(A, len);
  double stdev_out = stdev(A, mean_out, len);

  int positionsX[len];
  int positionsY[len];

  for (int i = 0; i < len; i++) {
    positionsX[i] = i * spacing;
  }

  for (int i = 0; i < len; i++) {
    positionsY[len] = map(A[i], mean_out-3.0*stdev_out, mean_out+3*stdev_out,0, ccwidth);
  }

  //need to insert graphing logic here
  
}



