#include <TimerOne.h>
#include <Wire.h>

#define maxOutputs 64
#define COMMAND_LENGTH 128

int timer_count = 0;
int TIMER_NANOSECONDS = 500;

int outputsQty;

// State
bool outputStates[maxOutputs];
// Channels should be received from the RasPi and synced with other Arduino
int outputChannels[maxOutputs];
// OUTS
int outputs[maxOutputs];
int testing_outputs[8] = {22,24,26,28,30,32,34,36};

void setup() {
  Serial.begin(1000000);
  Serial.println("Program Started");
}

void start_game() {
  // Testing HardCoded
  for (int i = 0; i < outputsQty; i++) {
    outputs[i] = testing_outputs[i];
    outputChannels[i] = (i+1)*10;
  }

  // Set Outputs as OUT
  for (int i = 0; i < outputsQty; i++) {
    outputStates[i] = false;
    int out = outputs[i];
    pinMode(out, OUTPUT);
  }

  Serial.println("Game Config:");
  Serial.print("Output Qty: "); Serial.println(outputsQty);
  Serial.print("Channels: ");
  for (int i = 0; i < outputsQty; i++) {
    Serial.print(outputChannels[i]); Serial.print(", ");
  }
  Serial.println("");
  Serial.print("Output Pins: ");
  for (int i = 0; i < outputsQty; i++) {
    Serial.print(outputs[i]); Serial.print(", ");
  }
  Serial.println("");

  // Init Timer
  Timer1.initialize(TIMER_NANOSECONDS);
  Timer1.attachInterrupt(timer1_callback);
}

void loop() {
  bool game_started = false;
  while(!game_started) {
    char command[COMMAND_LENGTH] = {'\0'};
    recvWithStartEndMarkers('$', ';', command);
    if (command[0] != '\0') {
      game_started = _process_received_command(command);
    }
  }
  start_game();
}

void recvWithStartEndMarkers(char startMarker, char endMarker, char *receivedChars) {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char rc;
    boolean newData = false;

    while (!newData) {
    if (Serial.available() > 0) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= COMMAND_LENGTH) {
                    ndx = COMMAND_LENGTH - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
    }
}

bool _process_received_command(char *command) {
  bool qty_done = false;
  char * strtokIndx; // this is used by strtok() as an index
  char directive[16];
  strtokIndx = strtok(command, ":");      // get the first part - the directive
  strcpy(directive, strtokIndx); // copy it to messageFromPC
  if (!strcmp(directive, "conn_qty")) {
    Serial.println("Received conn_qty command");
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    outputsQty = atoi(strtokIndx);
    qty_done = true;
  }
  if (qty_done) {
    return true;
  }
  return false;
}

void timer1_callback() {
  // The sending of the signals should be in a loop, configurable from the RasPi
  for (int i = 0; i < outputsQty; i++) {
    if (timer_count % outputChannels[i] == 0) {
      digitalWrite(outputs[i], outputStates[i]);
      outputStates[i] = !outputStates[i];
    }
  }
  // Timer Counter HealthCheck
  prevent_timer_overflow();
}

void prevent_timer_overflow() {
  timer_count ++;
  // Prevent Timer Counter Overflow
  if (timer_count > 10000) {
    timer_count = 0;
  }
}
