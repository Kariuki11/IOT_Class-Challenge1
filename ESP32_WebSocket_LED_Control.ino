#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// Define the GPIO pins for the RGB LED
#define RED_PIN 19
#define GREEN_PIN 33
#define BLUE_PIN 16

// Define the GPIO pins for the LEDs
#define LED1_PIN 2
#define LED2_PIN 4

// OLED display setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

bool led1State = false;
bool led2State = false;

// WiFi credentials
const char* ssid = "ALX The Piano 01";
const char* password = "Thepiano01";

// Function to update the OLED display
void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.println("ESP32 Web Server");

  // Use else if to conditionally display the LED states
  if (led1State && led2State) {
    display.println("LED1: ON, LED2: ON");
  }
  else if (led1State && !led2State) {
    display.println("LED1: ON, LED2: OFF");
  }
  else if (!led1State && led2State) {
    display.println("LED1: OFF, LED2: ON");
  }
  else {
    display.println("LED1: OFF, LED2: OFF");
  }

  display.display();
}

// Initialize WiFi connection
void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

// Toggle the LED state and update OLED
void toggleLED(int ledPin, bool &ledState) {
  ledState = !ledState;
  digitalWrite(ledPin, ledState ? HIGH : LOW);
  updateOLED();
}

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String msg = String((char*)payload);
    // Use else if to check the specific command
    if (msg == "toggleLED1") {
      toggleLED(LED1_PIN, led1State);
    } 
    else if (msg == "toggleLED2") {
      toggleLED(LED2_PIN, led2State);
    }
    // Send back the current LED states to the client
    webSocket.sendTXT(num, "LED1:" + String(led1State ? "ON" : "OFF") + ", LED2:" + String(led2State ? "ON" : "OFF"));
  }
}

// HTML content for the webpage
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head><title>ESP32 LED Control</title></head>
<body>
  <h1>ESP32 Web Server</h1>
  <button onclick="toggleLED('toggleLED1')">Toggle LED 1</button>
  <button onclick="toggleLED('toggleLED2')">Toggle LED 2</button>
  <script>
    var connection = new WebSocket('ws://' + location.hostname + ':81/');
    connection.onmessage = function(event) {
      console.log("WebSocket message received:", event.data);
    };
    function toggleLED(ledCommand) {
      connection.send(ledCommand);
    }
  </script>
</body>
</html>)rawliteral";

void setup() {
  Serial.begin(115200);

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    while(true);
  }
  display.display();
  delay(1000);
  display.clearDisplay();
  
  // Initialize LED pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  // Set initial brightness for the RGB LED
  analogWrite(RED_PIN, 255);    // Full brightness for red
  analogWrite(GREEN_PIN, 180);  // Medium brightness for green
  analogWrite(BLUE_PIN, 200);   // Medium brightness for blue

  setupWiFi();
  server.on("/", []() {
    server.send_P(200, "text/html", webpage);
  });
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  updateOLED();
}

void loop() {
  server.handleClient();
  webSocket.loop();
}



  
  // The code is well-commented and should be easy to understand. Here’s a brief overview of the code: 
  
  // Include the necessary libraries for WiFi, WebServer, WebSocketsServer, Adafruit_SSD1306, and Adafruit_GFX. 
  // Define the GPIO pins for the RGB LED, LEDs, and OLED display. 
  // Initialize the WebServer and WebSocketsServer objects. 
  // Define the LED states and initialize the WiFi connection. 
  // Implement the updateOLED function to update the OLED display with the current LED states. 
  // Implement the toggleLED function to toggle the state of an LED and update the OLED display. 
  // Implement the webSocketEvent function to handle WebSocket messages and toggle the LEDs accordingly. 
  // Define the HTML content for the webpage. 
  // Implement the setup function to initialize the OLED display, LED pins, RGB LED brightness, WiFi connection, WebServer, and WebSocketsServer. 
  // Implement the loop function to handle incoming client requests and WebSocket messages. 
  
  // Compile and Upload the Code 
  // After writing the code, compile and upload it to your ESP32 board. Make sure you have selected the correct board and port in the Arduino IDE before uploading the code. 
  // Once the code is uploaded, open the Serial Monitor to view the IP address of the ESP32. You will need this IP address to access the web server. 
  // Access the Web Server 
  // Open a web browser on your computer or smartphone and enter the IP address of the ESP32 in the address bar. You should see a webpage with two buttons labeled “Toggle LED 1” and “Toggle LED 2”. 
  // Clicking on these buttons will toggle the states of the LEDs connected to the ESP32. The OLED display will also update to show the current states of the LEDs. 
  // You can also open the Serial Monitor to view the WebSocket messages sent and received by the ESP32. 
  // That’s it! You have successfully created a web server to control LEDs using WebSocket communication on an ESP32. You can now customize the code to add more features or control other devices using the web server. 
  // Summary 
  // In this tutorial, you learned how to create a web server to control LEDs using WebSocket communication on an ESP32. You used the WebServer and WebSocketsServer libraries to handle HTTP requests and WebSocket messages. You also used an OLED display to show the current states of the LEDs. 
  // WebSocket communication allows for real-time bidirectional communication between the ESP32 and the web client,