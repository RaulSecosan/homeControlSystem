const int STEP_TIME = 5; // step time in ms (2 is minimum for the 28BYJ-48 stepper I used)

class BiPolStepper {
  public:
    int state [4] = {LOW, LOW, LOW, LOW};
    bool isUpdated = false;

    BiPolStepper(int steps) {
      this->steps = steps;
      mls = millis();
    }

    void update() {
      if (canRotate())
        rotate();
    }

    bool isRunning() {
      return stepsToMake > 0;
    }

    void rotateFull(int rot, bool dir = true) {
      stepsToMake = (long)steps * (long)rot;
      this->dir = dir;
    }

    void setStepsToMake(long steps, bool dir) {
      stepsToMake = steps;
      this->dir = dir;
    }

    void clearUpdated() {
      this->isUpdated = false;
    }

  private:
    int cStep = 0;
    int steps;
    long mls;
    long stepsToMake = 0;
    bool dir = true;

    void rotate() {
      this->isUpdated = true;
      stepsToMake--;
      mls = millis();
      calcNextStep();
      updateStep();
    }

    void calcNextStep() {
      if (dir) {
        cStep = (cStep + 1) % 4;
      } else {
        if (cStep == 0)
          cStep = 4;
        cStep = (cStep - 1) % 4;
      }
    }

    // void updateStep() {
    //   for (int i = 0; i < 4; i++)
    //     state[i] = i == cStep ? HIGH : LOW;
    // }



// Full-Step Mode:
// Activează două bobine simultan pentru fiecare pas, ceea ce crește câmpul magnetic și cuplul motorului.
// În această configurație, schema de rotație a bobinelor este:
// Pasul 0: Bobinele 1 și 2 (state[0] și state[1]) sunt activate.
// Pasul 1: Bobinele 2 și 3 (state[1] și state[2]) sunt activate.
// Pasul 2: Bobinele 3 și 4 (state[2] și state[3]) sunt activate.
// Pasul 3: Bobinele 4 și 1 (state[3] și state[0]) sunt activate.

    void updateStep() {
    // Excitație completă: activează două bobine simultan
    switch (cStep) {
        case 0: state[0] = HIGH; state[1] = HIGH; state[2] = LOW;  state[3] = LOW;  break;
        case 1: state[0] = LOW;  state[1] = HIGH; state[2] = HIGH; state[3] = LOW;  break;
        case 2: state[0] = LOW;  state[1] = LOW;  state[2] = HIGH; state[3] = HIGH; break;
        case 3: state[0] = HIGH; state[1] = LOW;  state[2] = LOW;  state[3] = HIGH; break;
    }
}


    bool canRotate() {
      return (millis() - mls >= STEP_TIME) && isRunning();
    }
};
