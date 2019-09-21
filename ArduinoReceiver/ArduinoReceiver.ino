#include <TimerOne.h>
#include <Wire.h>

#define maxInputs 64
#define COMMAND_LENGTH 128

#define SLAVE_ADDRESS 0x04

int timer_count = 0;
int TIMER_NANOSECONDS = 500;

int inputsQty = 0;

bool inputConnState[maxInputs];
// Channels should be received from the RasPi and synced with other Arduino
int inputChannels[maxInputs];
// Ins
int inputs[maxInputs];
int testing_inputs[8] = {22, 24, 26, 28, 30, 32, 34, 36};

struct state_struct {
  int Low;
  int High;
  int LastLow;
  int LastHigh;
};
// States should match the amount of pins, configurable from te RasPi
state_struct inputStates[maxInputs];

bool qty_done = false;
bool pos_array_done = false;


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  // begin running as an I2C slave on the specified address
  Wire.begin(SLAVE_ADDRESS);
  // start serial for output
  Serial.begin(1000000);
  Serial.println("Program Started");
  // create event for receiving data
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

  // Init Timer
  Timer1.initialize(TIMER_NANOSECONDS);
  Timer1.attachInterrupt(timer1_callback);
}
void loop() {


}

void start_game() {
  // Testing HardCoded
  for (int i = 0; i < inputsQty; i++) {
    inputs[i] = testing_inputs[i];
    inputChannels[i] = (i + 1) * 10;
  }

  // Set Inputs as IN
  for (int i = 0; i < inputsQty; i++) {
    inputStates[i] = {0, 0, 0, 0};
    inputConnState[i] = false;
    int in = inputs[i];
    pinMode(in, INPUT_PULLUP);
  }

  Serial.println("Game Config:");
  Serial.print("Input Qty: "); Serial.println(inputsQty);
  Serial.print("Channels: ");
  for (int i = 0; i < inputsQty; i++) {
    Serial.print(inputChannels[i]); Serial.print(", ");
  }
  Serial.println("");
  Serial.print("Input Pins: ");
  for (int i = 0; i < inputsQty; i++) {
    Serial.print(inputs[i]); Serial.print(", ");
  }
  Serial.println("");

}


void sendData() {
  if (qty_done && pos_array_done) {
    char string[inputsQty];
    for (int i = 0; i < inputsQty; i++) {
      if (inputConnState[i]) {
        string[i] = '1';
      } else {
        string[i] = '0';
      }
    }
    string[inputsQty] = '\0';
    Wire.write(string);
  } else {
    Wire.write("Nope");
  }
}


void receiveData(int byteCount) {
  char command[COMMAND_LENGTH] = {'\0'};

  recvWithStartEndMarkers('$', ';', command);
  if (command[0] != '\0') {
    _process_received_command(command);
  }

}

void recvWithStartEndMarkers(char startMarker, char endMarker, char *receivedChars) {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char rc;
  boolean newData = false;
  int retry = 0;

  while (!newData && retry < 1000) {
    retry ++;
    if (Wire.available() > 0) {
      rc = Wire.read();

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

void _process_received_command(char *command) {
  int pos_array[maxInputs];

  int pos_index;

  char * strtokIndx; // this is used by strtok() as an index
  char directive[16];
  strtokIndx = strtok(command, ":");      // get the first part - the directive
  strcpy(directive, strtokIndx); // copy it to messageFromPC
  if (!strcmp(directive, "conn_qty")) {
    Serial.println("Received conn_qty command");
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    inputsQty = atoi(strtokIndx);
    qty_done = true;
  }
  else if (!strcmp(directive, "pos")) {
    Serial.println("Received pos command");
    strtokIndx = strtok(NULL, ",");
    pos_index = atoi(strtokIndx);
    pos_array_done = true;
  }
}

void check_connections() {
  bool result;
  for (int i = 0; i < inputsQty; i++) {
    result = _match_input(inputs[i], inputStates[i], inputChannels[i]);
    inputConnState[i] = result;
  }
}

bool _match_input(int pin, state_struct &state, int ticks) {
  bool result = false;
  int pinRead = digitalRead(pin);
  if (pinRead) {
    if (!state.High) {
      state.LastLow = state.Low;
      state.Low = 0;
    }
    state.High ++;
  } else {
    if (!state.Low) {
      state.LastHigh = state.High;
      state.High = 0;
    }
    state.Low ++;
  }

  if (ticks - 2 <= state.LastHigh && state.LastHigh <= ticks + 2 &&
      ticks - 2 <= state.LastLow && state.LastLow <= ticks + 2 &&
      state.High < ticks + 5 && state.Low < ticks + 5) {
    digitalWrite(LED_BUILTIN, HIGH);
    result = true;
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    result = false;
  }
  return result;
}


void send_status() {
  if (timer_count % 2000 == 0) {
    Serial.println("Status:");
    for (int i = 0; i < inputsQty; i++) {
      Serial.print("Input: "); Serial.print(inputs[i]);
      if (inputConnState[i]) {
        Serial.println(" is properly connected");
      } else {
        Serial.println(" is not connected");
      }
    }
  }
}

void timer1_callback() {


  if (!(qty_done && pos_array_done) && (timer_count % 400 == 0)) {
    Serial.print(qty_done);
    Serial.print(", ");
    Serial.println(pos_array_done);

    if (qty_done && pos_array_done) {
      Serial.println("Starting Game! Yay!");
      start_game();
    }
  }
  if (qty_done && pos_array_done && (timer_count % 60 == 0)) {
    check_connections();
    send_status();
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
