#include <WiFi.h>
#include <SparkFunCCS811.h>
#include <Wire.h>
//code to connect sonar sensors and air quality sensor to the robot

const char* ssid     = "Harvard University";
const int trigPin = 5;
const int echoPin = 18;
// defines variables
long duration; //duration of sonar time
unsigned int distance; //distance detected by sonar
unsigned int eCOtwo; // CO2 values
unsigned int TVOC; // Total Volatile Organic Compound

WiFiServer server(80);
CCS811 air_sensor(0x5B); //

void read_sonar(){
  // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds  
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;
    
   
}
void read_air_sensor(){
      air_sensor.readAlgorithmResults();
      eCOtwo = air_sensor.getCO2();
      TVOC = air_sensor.getTVOC();
      Serial.print("CO2[");
      Serial.print(eCOtwo);
      Serial.print("] ppm tVOC[");
      Serial.print(TVOC);
      Serial.print("] ppb");
      Serial.println();
  }
void setup()
{
    Serial.begin(115200);
    //Initialize the pins used for the sensors
    pinMode(trigPin, OUTPUT); 
    pinMode(echoPin, INPUT);
    pinMode(SDA,INPUT_PULLUP);
    pinMode(SCL,INPUT_PULLUP);
    delay(10);

    // Now, connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //start the server
    server.begin();

    Wire.begin(21,22);//initialize the pins used for I2C with the CCS811 air quality senser
    CCS811Core::status returnCode = air_sensor.begin();
    if(returnCode != CCS811Core::SENSOR_SUCCESS){
        Serial.println(".begin() returned with an error.");
        Serial.println("continuing program without air quality detection.");
      }
}

int value = 0;

void loop(){

 
 WiFiClient client = server.available();   // listen for incoming clients
  
  if(air_sensor.dataAvailable()){
    read_air_sensor();
  }
    //delay(10);
  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            //The following is the HTML that is sent to the client and displayed for viewing on the local webpage
            client.print(F( //start F() macro
              "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
              "<HTML>"
              "<HEAD>"
              "<TITLE>Who even reads these titles...</TITLE>"
              "<meta http-equiv=\"refresh\" content=\".5\" >"  //Refresh the page to update with the newly collected data.
              "<!-- ChangeTheValue -->"
              "</HEAD>"
              "<BODY style=\"background-color:#B7AEBE\">"
              "<CENTER>"
              "<B style=\"font-size:300%;\">Robot Data Collection</B>"
              "<br>"
              "<br>"
              "<br>"
              "<B style=\"font-size:150%;\">Sonar Readings</B><br>"));
              read_sonar();
            client.print(distance);
            if(distance > 15) client.print("<br>PIPE INTEGRITY VIOLATION DETECTED<br>");
            client.print(F(
              "<br>"
              "<B style=\"font-size:150%;\">Air Quality Readings</B><br>"
              "Equivalent CO2 (eCO2): "
              ));
            client.print(eCOtwo);
            client.print(" parts per million (ppm) <br>");
            client.print("Total Volatile Organic Compound (TVOC): ");
            client.print(TVOC);
            client.print(" parts ber billion (ppb) <br>");
            client.print(F(
              "</CENTER>" 
              "</BODY>"
              "</HTML>"));
           
            // End of the HTML for the webpage
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
