#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>

//Chân GPIO0/pin D3 nối với chân data của DHT11
#define DHTPIN 0
#define BUTTON_PIN 5

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Tạo liên kết tới firebase
#define FIREBASE_HOST "https://iomtproject-default-rtdb.asia-southeast1.firebasedatabase.app/" // Link tới Realtime database
#define FIREBASE_AUTH "AIzaSyAAjpj33sBpAmqCsDJJx1Ukyg6c2K8g-eA" //Web API Key
#define Led 2 //GPIO2/pin D4
int fireStatus = 0;

String root = "ICU";
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

bool taskCompleted = false;
// Kết nối wifi
#define WIFI_SSID "IT"
#define WIFI_PASSWORD "05102019"

float hum;
float temp;
float f;

void setup() {
  
  Serial.begin(9600);
  pinMode(Led, OUTPUT); 
  digitalWrite(Led, 0);  //trạng thái ban đầu là tắt
  WiFi.begin (WIFI_SSID, WIFI_PASSWORD); //khởi tạo wifi
  delay(2000);

  Serial.print("Connect to wifi!");
  Serial.println(WIFI_SSID);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
      
  Serial.println ("");
  Serial.println ("WiFi connected");
  Serial.println("IP address:");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setInt(fbdo, "/LED_STATUS", 0);

  Serial.println(F("DHTxx test!"));
  dht.begin();
  if (isnan(hum) || isnan(temp)){
  Serial.println(F("Fail!")); 
  delay(1000); 
  return;
  }
}

void loop() {
  if(Firebase.getInt(fbdo,"/LED_STATUS"))
  fireStatus = fbdo.intData();
  if (fireStatus ==1){ // Led sáng thì /LED_STATUS trên Realtime Database hiện ON và ngược lại
    Serial.println("Led turned ON"); // Hiển thị trên serial monitor
    digitalWrite(Led, 1);
  }
  else if (fireStatus ==0){ //Led tắt thì /LED_STATUS trên Realtime Database hiện OFF và ngược lại
    Serial.println("Led turned OFF"); // Hiển thị trên serial monitor
    digitalWrite(Led, 0);
  }
  else{
    Serial.println("Command error!");
  }
  // Đọc tín hiệu từ DHT11
  temp = random(0, 100); 
  hum = random(0, 100); 
  f = random(0, 100);
  // In ra serial monitor
  Serial.print("Hum: ");
  Serial.print(hum);
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println("");
  Serial.print("Temp in F: ");
  Serial.print(f);
  Serial.println("");
  
  //Đặt giá trị thu được lên root ở Realtime database 
  Firebase.setInt(fbdo,root+"/Temp",temp);
  Firebase.setInt(fbdo,root+"/Hum",hum);
  //Đẩy giá trị thu được lên root ở Realtime database
  Firebase.pushInt(fbdo, "/test/push/data/temp", temp);
  Firebase.pushInt(fbdo, "/test/push/data/hum", hum);
  Firebase.pushInt(fbdo, "/test/push/data/temp in f", f); //Đổi từ độ C sang F
}
