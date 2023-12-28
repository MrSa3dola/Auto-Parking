#include <WiFi.h>
#include <math.h>

const char* ssid = "Car1";
const char* password = "12345678";

struct Point {
  float x;
  float y;
};

struct StationaryRobot {
  const char* ssid;
  float distance;
  Point position;
} stationaryRobots[] = {
  { "Car2", 0, { 0, 4 } },
  { "Car3", 0, { 2, 1.5 } },
  { "Car4", 0, { 0, 0 } }
};

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

void loop() {
  Serial.println("Scanning for stationary robots...");
  int n = WiFi.scanNetworks();

  for (int i = 0; i < n; ++i) {
    for (StationaryRobot& robot : stationaryRobots) {
      if (WiFi.SSID(i) == robot.ssid) {
        int rssi = WiFi.RSSI(i);
        robot.distance = rssiToDistance(rssi);

        Serial.print("RSSI to ");
        Serial.print(robot.ssid);
        Serial.print(" ");
        Serial.print(rssi);
        Serial.print("\n");
        Serial.printf("Distance to %s: %.2f meters\n", robot.ssid, robot.distance);  // Convert to meters
      }
    }
  }

  // Calculate the position of the mobile robot
  Point mobileRobotPosition = calculatePosition();
  Serial.printf("Mobile Robot Position: (%.2f, %.2f)\n", mobileRobotPosition.x, mobileRobotPosition.y);

  WiFi.softAP(ssid, password);
  delay(5000);
}

float rssiToDistance(int rssi) {
  // Calibrate the RSSI to distance conversion based on empirical measurements
  const double A = -55;  // Calibrated value
  const double n = 3.6;  // Calibrated value
  float distance = pow(10.0, (A - rssi) / (10 * n));
  return distance;
}

float getAverageRSSI(const char* ssid, int samples, int delayTime) {
  int totalRSSI = 0;
  for (int i = 0; i < samples; ++i) {
    int n = WiFi.scanNetworks();
    for (int j = 0; j < n; ++j) {
      if (WiFi.SSID(j) == ssid) {
        totalRSSI += WiFi.RSSI(j);
        break;
      }
    }
    delay(delayTime);
  }
  return float(totalRSSI) / samples;  // Return the average RSSI
}

Point calculatePosition() {
  // Using trilateration to find the position of the mobile robot
  Point p1 = stationaryRobots[0].position;
  Point p2 = stationaryRobots[1].position;
  Point p3 = stationaryRobots[2].position;
  float r1 = stationaryRobots[0].distance;
  float r2 = stationaryRobots[1].distance;
  float r3 = stationaryRobots[2].distance;

  float A = 2 * (p2.x - p1.x);
  float B = 2 * (p2.y - p1.y);
  float C = r1 * r1 - r2 * r2 - p1.x * p1.x - p1.y * p1.y + p2.x * p2.x + p2.y * p2.y;
  float D = 2 * (p3.x - p2.x);
  float E = 2 * (p3.y - p2.y);
  float F = r2 * r2 - r3 * r3 - p2.x * p2.x - p2.y * p2.y + p3.x * p3.x + p3.y * p3.y;

  float x = (C * E - F * B) / (E * A - B * D);
  float y = (C * D - A * F) / (B * D - A * E);

  return { x, y };
}

