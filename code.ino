#include <ESP8266WiFi.h>                // Library esp8266 wifi
#include <ESP8266WebServer.h>           // Library esp8266 web server
#include "DHT.h"                        // Library dht (dht11, dht21, dht22)

// Uncomment salah satu baris kode berikut sesuai dengan jenis sensor DHT yang digunakan!
#define DHTTYPE DHT11                   // DHT 11
//#define DHTTYPE DHT21                 // DHT 21 (AM2301)
//#define DHTTYPE DHT22                 // DHT 22 (AM2302), AM2321

// Jaringan kredensial Wi-Fi
const char* ssid = "ANOS";        // Masukkan SSID disini
const char* password = "alfarino13";     	// Masukkan Password Wi-Fi disini

ESP8266WebServer server(80);            // Mengaktifkan server dengan port 80

// DHT Sensor
uint8_t DHTPin = D1;                    // Gunakan pin D1 pada modul WeMos D1 Mini untuk dikoneksikan ke sensor DHT11

// Inisialisasi sensor DHT
DHT dht(DHTPin, DHTTYPE);               // Mengaktifkan pin D1
float Temperature;                      // Mendefinisikan tipe data variabel Temperature sebagai float
float Humidity;                         // Mendefinisikan tipe data variabel Humidity sebagai float

void setup() {
  Serial.begin(115200);                           // Baudrate 115200
  delay(100);                                     // Delay 0.1 detik
  pinMode(DHTPin, INPUT);                         // Mendeklarasikan DHTPin (yaitu D1) sebagai INPUT
  dht.begin();                                    // Sensor DHT mulai beroperasi
  Serial.println("Connecting to ");               // Cetak "Connecting to " dengan baris baru
  Serial.println(ssid);                           // Cetak SSID (Nama Wi-Fi) dengan baris baru
  
  WiFi.begin(ssid, password);                     // Mulai mengkoneksikan ke jaringan Wi-Fi

  // Mengecek modul Wi-Fi apakah terkoneksi ke jaringan Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {         // While wi-fi is not connected,
  delay(1000);                                    // Delay 1 second
  Serial.print(".");                              // Keep print "." with delay 1 second until WiFi.status() = WL_CONNECTED
  }

  // Setelah terkoneksi ke jaringan Wi-Fi
  Serial.println("");                                             // Cetak spasi kosong dengan baris baru
  Serial.println("WiFi connected..!");                            // Cetak "WiFi connected..!" dengan baris baru
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());      // Cetak "Got IP: IP-address
  server.on("/", handle_OnConnect);                               // Server ON
  server.onNotFound(handle_NotFound);                             // Server tidak dapat ditemukan (404 Not Found)
  server.begin();                                                 // Server 'mulai jalan'
  Serial.println("HTTP server started");                          // Cetak "HTTP server started" dengan baris baru
}

void loop() {
  server.handleClient();                                          // Memproces 'permintaan (request)' dari klien
}

void handle_OnConnect() {                                         // Saat handle_OnConnect
  Temperature = dht.readTemperature();                            // Dapatkan nilai suhu (temperature)
  Humidity = dht.readHumidity();                                  // Dapatkan nilai kelembaban (humidity)
  server.send(200, "text/html", SendHTML(Temperature,Humidity));  // Mengirim respons HTTP ke klien yang telah membuat permintaan kepada server sebelumnya
  /* Mari kita lihat elemen-elemen dari pernyataan tersebut:
   * 200         : Ini adalah kode status HTTP yang menunjukkan bahwa permintaan klien berhasil diproses. Kode status 200 umumnya digunakan untuk respons OK.
   * "text/html" : Ini adalah jenis konten (content type) yang dikirim dalam respons. Dalam contoh ini, tipe konten adalah HTML,
   *               yang menunjukkan bahwa respons akan berisi dokumen HTML.
   * SendHTML(Temperature, Humidity): Ini adalah panggilan fungsi SendHTML dengan parameter Temperature dan Humidity.
   * Fungsi ini mungkin digunakan untuk menghasilkan atau memformat dokumen HTML yang akan dikirimkan sebagai respons.
   * Nilai Temperature dan Humidity kemungkinan diambil dari suatu sensor atau variabel lain dalam program Arduino.
   */
}

void handle_NotFound(){                                           // Saat handle_NotFound
  server.send(404, "text/plain", "Not found");                    // Server mengirim 404 Web Not Found
}

String SendHTML(float Temperaturestat,float Humiditystat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 with DHT11 Sensor</title>\n";
ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
ptr +="body{margin-top: 50px; text-align: center;} h1 {color: #444444;margin: 50px auto 30px; text-align: center;}\n";
ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px; text-align: center;}\n";
ptr +="</style>\n";

  // Tambahkan script JavaScript untuk mengambil data dari server secara berkala
  ptr += "<script>\n";
  ptr += "setInterval(function() {\n";
  ptr += "  var xhttp = new XMLHttpRequest();\n";
  ptr += "  xhttp.onreadystatechange = function() {\n";
  ptr += "    if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "      document.getElementById('webpage').innerHTML = this.responseText;\n";
  ptr += "    }\n";
  ptr += "  };\n";
  ptr += "  xhttp.open('GET', '/sensor', true);\n";
  ptr += "  xhttp.send();\n";
  ptr += "}, 5000); // Update setiap 5 detik\n";
  ptr += "</script>\n";

  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h2>WeMos D1 Mini</h2>\n";
  ptr +="<h2>Dengan Modul Sensor DHT11</h2>\n";
  ptr +="<h3>-------------</h3>\n";
  ptr +="<p>Suhu Ruangan: ";
  ptr +=(int)Temperaturestat;
  ptr +=" &degC</p>";
  ptr +="<p>Kelembaban: ";
  ptr +=(int)Humiditystat;
  ptr +="%</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
