// https://techtutorialsx.com/2016/10/03/esp8266-setting-a-simple-http-webserver/

#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"

// global vars
char* OPEN_POSITION = "100";
char* MIDDLE_POSITION = "50";
char* CLOSED_POSITION = "0";

// it’s best to avoid GPIO 0 (D3), 2 (D4) and 15 (D8) unless absolutely necessary
/*
  GPIO16 (D0): pin is high at BOOT
  GPIO5  (D1): normal
  GPIO4  (D2): normal
  GPIO0  (D3): boot failure if pulled LOW
  GPIO2  (D4): pin is high on BOOT, boot failure if pulled LOW
  GPIO14 (D5): normal
  GPIO12 (D6): normal
  GPIO13 (D7): normal
  GPIO15 (D8): boot failure if pulled HIGH
*/

// FAR = right door (from inside garage)
const int FAR_DOOR_PIN = D2;
const int FAR_CLOSED_SENSOR_PIN = D0;
const int FAR_OPEN_SENSOR_PIN = D1;
char* far_door_position = OPEN_POSITION;

// NEAR = left door (from inside garage)
const int NEAR_DOOR_PIN = D5;
const int NEAR_CLOSED_SENSOR_PIN = D6;
const int NEAR_OPEN_SENSOR_PIN = D7;
char* near_door_position = OPEN_POSITION;

ESP8266WebServer server(80);

void setup() {

  Serial.begin(115200);

  pinMode(FAR_DOOR_PIN, OUTPUT);
  digitalWrite(FAR_DOOR_PIN, LOW);
  pinMode(FAR_CLOSED_SENSOR_PIN, INPUT_PULLUP);
  pinMode(FAR_OPEN_SENSOR_PIN, INPUT_PULLUP);

  pinMode(NEAR_DOOR_PIN, OUTPUT);
  digitalWrite(NEAR_DOOR_PIN, LOW);
  pinMode(NEAR_CLOSED_SENSOR_PIN, INPUT_PULLUP);
  pinMode(NEAR_OPEN_SENSOR_PIN, INPUT_PULLUP);

  check_sensors();

  WiFi.begin("wireless access point", "lucidpuma");  //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) {  //Wait for connection

    delay(500);
    Serial.println("Waiting to connect…");

  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP

  server.on("/far/toggle", toggle_far_door);
  server.on("/far/open", open_far_door);
  server.on("/far/close", close_far_door);
  server.on("/far/position", get_far_door_position);

  server.on("/near/toggle", toggle_near_door);
  server.on("/near/open", open_near_door);
  server.on("/near/close", close_near_door);
  server.on("/near/position", get_near_door_position);

  server.on("/", handleRootPath);    //Associate the handler function to the path
  server.begin();                    //Start the server
  Serial.println("Server listening");

}

void loop() {
  check_sensors();
  server.handleClient();         //Handling of incoming requests
}

void handleRootPath() {            //Handler for the root path
  server.send(200, "text/plain", "Hello world");
}

void triggerNear() {
  digitalWrite(NEAR_DOOR_PIN, HIGH);
  delay(500);
  digitalWrite(NEAR_DOOR_PIN, LOW);
  server.send(200, "on");
}

void triggerFar() {
  digitalWrite(FAR_DOOR_PIN, HIGH);
  delay(500);
  digitalWrite(FAR_DOOR_PIN, LOW);
}

void toggle_far_door() {
  triggerFar();
  server.send(200, "toggled far");
}

void open_far_door() {
  // if already open, quit
  if (far_door_position == OPEN_POSITION) {
    server.send(200, "far open");
    return;
  }

  triggerFar();
  server.send(200, "opened far");
}

void close_far_door() {
  // if already closed, quit
  if (far_door_position == CLOSED_POSITION) {
    server.send(200, "far closed");
    return;
  }

  triggerFar();
  server.send(200, "closed far");
}

void toggle_near_door() {
  triggerNear();
  server.send(200, "toggled near");
}

void open_near_door() {
  // if already open, quit
  if (near_door_position == OPEN_POSITION) {
    server.send(200, "near open");
    return;
  }

  triggerNear();
  server.send(200, "opened near");
}

void close_near_door() {
  // if already closed, quit
  if (near_door_position == CLOSED_POSITION) {
    server.send(200, "near closed");
    return;
  }

  triggerNear();
  server.send(200, "closed near");
}

void check_sensors() {
  bool far_door_closed = digitalRead(FAR_CLOSED_SENSOR_PIN) == LOW;
  bool far_door_open = digitalRead(FAR_OPEN_SENSOR_PIN) == LOW;

  if (far_door_closed) {
    far_door_position = CLOSED_POSITION;
  } else if (far_door_open) {
    far_door_position = OPEN_POSITION;
  } else {
    far_door_position = MIDDLE_POSITION;
  }

  bool near_door_closed = digitalRead(NEAR_CLOSED_SENSOR_PIN) == LOW;
  bool near_door_open = digitalRead(NEAR_OPEN_SENSOR_PIN) == LOW;

  if (near_door_closed) {
    near_door_position = CLOSED_POSITION;
  } else if (near_door_open) {
    near_door_position = OPEN_POSITION;
  } else {
    near_door_position = MIDDLE_POSITION;
  }
}

void get_far_door_position() {
  server.send(200, far_door_position, far_door_position);
}

void get_near_door_position() {
  server.send(200, near_door_position, near_door_position);
}
