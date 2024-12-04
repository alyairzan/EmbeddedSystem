// Pin untuk sensor ultrasonik
const int trigPin = 12;   // Pin trigger
const int echoPin = 11;   // Pin echo

// Pin untuk sensor kelembaban
const int moisturePin = A4;   // Pin analog untuk sensor kelembaban tanah

// Pin untuk relay
const int relayPin = 7;  // Pin untuk mengontrol relay/pompa

// Pin untuk LED RGB
const int redLEDPin = 8;   // Pin untuk LED merah
const int greenLEDPin = 9; // Pin untuk LED hijau

// Pin untuk LED hijau indikator pompa
const int pumpIndicatorPin = 2; // Pin untuk LED hijau indikator pompa (pompa hidup/mati)

// Variabel untuk menyimpan hasil pembacaan
long duration;
int distance;
int moistureLevel;
int numReadings = 10;   // Jumlah pembacaan untuk rata-rata
int averageMoisture = 0; // Menyimpan nilai rata-rata kelembaban

// Batasan ketinggian air dan kelembaban
const int minWaterHeight = 15;  // Ketinggian air minimal (cm)
const int maxMoisture = 75;     // Kelembaban tanah maksimal (%) untuk aktifkan pompa

void setup() {
  // Inisialisasi pin untuk sensor ultrasonik
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Inisialisasi pin untuk relay
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);  // Awalnya matikan pompa (relay off)

  // Inisialisasi pin untuk LED RGB
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);

  // Inisialisasi pin untuk LED hijau indikator pompa
  pinMode(pumpIndicatorPin, OUTPUT);  // Menetapkan pin LED hijau sebagai output

  digitalWrite(pumpIndicatorPin, LOW);  // Memastikan LED indikator mati

  // Awalnya nyalakan LED merah sebagai state awal
  setColor(255, 0, 0);  // Merah
  Serial.begin(9600);  // Inisialisasi komunikasi Serial

  Serial.println("Sistem siap, LED merah menyala sebagai state awal...");
}

void loop() {
  // Mengirim sinyal trigger pada sensor ultrasonik
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Membaca sinyal echo dan menghitung durasi pantulnya
  duration = pulseIn(echoPin, HIGH);

  // Jika durasi valid (lebih besar dari 0), hitung jarak
  if (duration > 0) {
    // Menghitung jarak berdasarkan durasi sinyal echo (dalam cm)
    distance = duration * 0.034 / 2;
  } else {
    // Jika tidak ada sinyal echo, set jarak sebagai -1 (tidak valid)
    distance = -1;
  }

  // Menghitung rata-rata pembacaan kelembaban
  averageMoisture = 0;  // Reset nilai rata-rata setiap kali perulangan

  for (int i = 0; i < numReadings; i++) {
    averageMoisture += analogRead(moisturePin); // Menambahkan hasil pembacaan
    delay(0); // Mengurangi delay menjadi 100ms untuk mempercepat rata-rata
  }

  // Menyelesaikan rata-rata pembacaan kelembaban
  averageMoisture /= numReadings;  // Pembacaan rata-rata

  // Menampilkan nilai kelembaban tanah dalam bentuk persentase
  int moisturePercent = map(averageMoisture, 0, 1023, 100, 0);  // Konversi ke persentase

  // Menampilkan data dari kedua sensor secara bersamaan
  Serial.print("Jarak air: ");
  if (distance != -1) {
    Serial.print(distance);
    Serial.print(" cm, ");
  } else {
    Serial.print("Tidak terdeteksi, ");
  }

  Serial.print("Kelembaban: ");
  Serial.print(moisturePercent);
  Serial.println("%");

  // Kontrol LED berdasarkan jarak air
  if (distance < minWaterHeight) {
    setColor(0, 255, 0);  // Nyalakan LED hijau (air sedikit)
  } else if (distance >= minWaterHeight) {
    setColor(255, 0, 0);  // Nyalakan LED merah (air cukup)
  }

  // Mengaktifkan relay/pompa berdasarkan kondisi yang diinginkan
  if (distance < minWaterHeight && moisturePercent < maxMoisture) {  // Kondisi dibalik
    digitalWrite(relayPin, LOW);   // Pompa hidup
    setPumpIndicatorColor(HIGH);  // LED hijau indikator pompa hidup
    Serial.println("Pompa ON - Air cukup dan Tanah kering");
  } else {
    digitalWrite(relayPin, HIGH);    // Pompa mati
    setPumpIndicatorColor(LOW);   // LED hijau indikator pompa mati
    Serial.println("Pompa OFF - Tidak memenuhi kondisi");
  }

  // Jeda pembacaan selama 1 detik sebelum pembacaan ulang
  delay(1000);  // Memberikan waktu jeda 1 detik untuk pembacaan berikutnya
}

// Fungsi untuk mengatur warna LED RGB
void setColor(int red, int green, int blue) {
  analogWrite(redLEDPin, red);
  analogWrite(greenLEDPin, green);
}

// Fungsi untuk mengatur LED indikator pompa (pumpIndicator)
void setPumpIndicatorColor(int state) {
  if (state == HIGH) {
    // Nyalakan LED hijau indikator pompa
    analogWrite(pumpIndicatorPin, 255);  // LED hijau (pompa hidup)
  } else {
    // Matikan LED hijau indikator pompa
    analogWrite(pumpIndicatorPin, 0);  // LED mati (pompa mati)
  }
}
