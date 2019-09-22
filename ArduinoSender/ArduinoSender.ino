#include <TimerOne.h>
#include <Wire.h>

#define maxOutputs 32
#define COMMAND_LENGTH 128
#define SLAVE_ADDRESS 0x05

int timer_count = 0;
int TIMER_NANOSECONDS = 500;

int outputsQty;
bool outputStates[maxOutputs];
int outputChannels[maxOutputs];
int outputs[maxOutputs];
int testing_outputs[8] = {22,24,26,28,30,32,34,36};

// Arduino Mega PinMap (Leftmost 32 pin cluster)
int outputArray[maxOutputs] = {
  22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52,
  23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53
};

bool qty_done = false;
bool pos_array_done = false;
bool game_started = false;

void setup() {
    Wire.begin(SLAVE_ADDRESS);
    Wire.setClock(400000);
    Wire.onReceive(receiveData);

    // Init Timer
    Timer1.initialize(TIMER_NANOSECONDS);
    Timer1.attachInterrupt(timer1_callback);
}

void loop() {
    delay(100);
}

void start_game() {
    // Testing HardCoded
    for (int i = 0; i < outputsQty; i++) {
        // outputs[i] = testing_outputs[i];
        outputChannels[i] = (i * 10) + 100;
    }

    // Set Outputs as OUT
    for (int i = 0; i < outputsQty; i++) {
        outputStates[i] = false;
        int out = outputs[i];
        pinMode(out, OUTPUT);
    }
    game_started = true;
}

bool _process_received_command(char *command) {
    char * strtokIndx; // this is used by strtok() as an index
    char directive[16];
    strtokIndx = strtok(command, ":"); // get the first part - the directive
    strcpy(directive, strtokIndx); // copy it to messageFromPC
    if (!strcmp(directive, "conn_qty")) {
        strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
        outputsQty = atoi(strtokIndx);
        qty_done = true;
    }
    else if (!strcmp(directive, "restart")) {
        qty_done = false;
        game_started = false;
        pos_array_done = false;
    }
    else if (!strcmp(directive, "pos")) {
        int ii = 0;
        while( strtokIndx != NULL && ii < maxOutputs ) {
          strtokIndx = strtok(NULL, ",");
          outputs[ii] = outputArray[atoi(strtokIndx)];
          ii++;
        }
        pos_array_done = true;
    }
}

void make_connections() {
    for (int i = 0; i < outputsQty; i++) {
        if (timer_count % outputChannels[i] == 0) {
            digitalWrite(outputs[i], outputStates[i]);
            outputStates[i] = !outputStates[i];
        }
    }
}

void timer1_callback() {
    if ((! game_started) && qty_done && pos_array_done) {
        start_game();
    }
    make_connections();
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
