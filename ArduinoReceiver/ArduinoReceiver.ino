#include <TimerOne.h>
#include <Wire.h>

#define maxInputs 32
#define COMMAND_LENGTH 128
#define SLAVE_ADDRESS 0x04

int timer_count = 0;
int TIMER_NANOSECONDS = 500;

int inputsQty;
bool inputConnState[maxInputs];
int inputChannels[maxInputs];
int inputs[maxInputs];
int testing_inputs[8] = {22, 24, 26, 28, 30, 32, 34, 36};

// Arduino Mega PinMap (Leftmost 32 pin cluster)
int inputArray[maxInputs] = {
    22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52,
    23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53
};

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
bool game_started = false;

bool healthCheck = false;

char stateString[maxInputs] = {'\0'};
int statusCounter = 0;

void setup() {
    Wire.begin(SLAVE_ADDRESS);
    Wire.setClock(400000);
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);

    // Init Timer
    Timer1.initialize(TIMER_NANOSECONDS);
    Timer1.attachInterrupt(timer1_callback);
}

void loop() {
    delay(100);
    if (game_started) {
        for (int i = 0; i < inputsQty; i++) {
            if (inputConnState[i]) {
                stateString[i] = '1';
            } else {
                stateString[i] = '0';
            }
        }
        stateString[inputsQty] = '\0';
    }
}

void start_game() {
    // Testing HardCoded
    for (int i = 0; i < inputsQty; i++) {
        // inputs[i] = testing_inputs[i];
        inputChannels[i] = (i * 10) + 100;
    }

    // Set Inputs as IN
    for (int i = 0; i < inputsQty; i++) {
        inputStates[i] = {0, 0, 0, 0};
        inputConnState[i] = false;
        int in = inputs[i];
        pinMode(in, INPUT_PULLUP);
    }

    game_started = true;
}

void sendData() {
    char hc = 'N';
    if (healthCheck) {
        if (game_started) {
            hc = 'Y';
        }
        Wire.write(hc);
        healthCheck = false;
    }
    else if (game_started) {
        Wire.write(stateString[statusCounter]);
        statusCounter++;
        if (statusCounter > inputsQty) {
            statusCounter = 0;
        }
    } else {
        Wire.write('E');
    }
}

void _process_received_command(char *command) {
    int pos_array[maxInputs];

    int pos_index;

    char * strtokIndx; // this is used by strtok() as an index
    char directive[16];
    strtokIndx = strtok(command, ":"); // get the first part - the directive
    strcpy(directive, strtokIndx); // copy it to messageFromPC

    if (!strcmp(directive, "status")) {
        statusCounter = 0;
    }
    else if (!strcmp(directive, "health_check")) {
        healthCheck = true;
    }
    else if (!strcmp(directive, "restart")) {
        qty_done = false;
        game_started = false;
        pos_array_done = false;
    }
    else if (!strcmp(directive, "conn_qty")) {
        strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
        inputsQty = atoi(strtokIndx);
        qty_done = true;
    }
    else if (!strcmp(directive, "pos")) {
        int ii = 0;
        while( strtokIndx != NULL && ii < maxInputs ) {
            strtokIndx = strtok(NULL, ",");
            inputs[ii] = inputArray[atoi(strtokIndx)];
            ii++;
        }
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
    if (state.High > 2000) {
        state.High = 0;
    }
    if (state.Low > 2000) {
        state.Low = 0;
    }
    if (ticks - 2 <= state.LastHigh && state.LastHigh <= ticks + 2 &&
            ticks - 2 <= state.LastLow && state.LastLow <= ticks + 2 &&
            state.High < ticks + 5 && state.Low < ticks + 5) {
        result = true;
    } else {
        result = false;
    }
    return result;
}

void timer1_callback() {

    if ((! game_started) && qty_done && pos_array_done) {
        start_game();
    }
    if (game_started) {
        check_connections();
    }

    // Timer Counter HealthCheck
    prevent_timer_overflow();
}

// Same in both... Maybe a lib?
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

void prevent_timer_overflow() {
    timer_count ++;
    // Prevent Timer Counter Overflow
    if (timer_count > 10000) {
        timer_count = 0;
    }
}
