// Kütüphanelerin eklenmesi
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Yeni bir objenin düşme olasılığı
#define OBJECT_DROP_CHANCE 10 // Yüzde 10 şans
// Objelerin özellikleri
#define OBJECT_WIDTH 5
#define OBJECT_HEIGHT 5
#define OBJECT_SPEED 1
int objectX;
int objectY;
bool objectActive = false;
// 7 segment display'de kullanılan pinlerin tanımlanması
#define SEG_A 7
#define SEG_B 6
#define SEG_C 5
#define SEG_D 4
#define SEG_E 3
#define SEG_F 2
#define SEG_G 1
#define SEG_1 A2
#define SEG_2 A3
// OLED ekranın pin tanımlamaları
#define OLED_RESET    -1
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
////Arayüz için tuşların tanımlanması
#define BUTTON_UP_PIN     10
#define BUTTON_DOWN_PIN   9
#define BUTTON_SELECT_PIN 8
// Potansiyometre pin tanımlaması
#define POT_PIN A0
// LED'lerin pin tanımlamaları
#define GREEN_LED_PIN 13
#define YELLOW_LED_PIN 12
#define RED_LED_PIN 11
// Paletin genişliği ve başlangıç konumu
#define PALET_WIDTH 30
#define PALET_HEIGHT 3
#define PALET_SPEED 2
int paletX = (SCREEN_WIDTH - PALET_WIDTH) / 2;
int paletY = SCREEN_HEIGHT - 10;
// Topun boyutu ve başlangıç konumu
#define BALL_SIZE 3
#define BALL_SPEED_X 2
#define BALL_SPEED_Y 2
int ballX = paletX + (PALET_WIDTH / 2);
int ballY = paletY - BALL_SIZE;
int ballSpeedX = BALL_SPEED_X;
int ballSpeedY = BALL_SPEED_Y;
// Skor
int score = 0;
// Can sayısı
int lives = 3;
// Tuğla özellikleri
#define BRICK_WIDTH 10
#define BRICK_HEIGHT 6
#define BRICK_GAP 2 // Palet ile tuğlalar arasındaki boşluk
// Tuğla sayısı ve düzeni
#define NUM_BRICKS_X 10
#define NUM_BRICKS_Y 3
int bricks[NUM_BRICKS_X][NUM_BRICKS_Y]; // Tuğlaların konumlarını tutmak için 2D dizi

void setup() {
  // OLED ekranın başlatılması
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();
  pinMode(BUTTON_UP_PIN, INPUT); //Butonların ayarlanması
  pinMode(BUTTON_DOWN_PIN, INPUT);
  pinMode(BUTTON_SELECT_PIN, INPUT);
  // Potansiyometrenin giriş olarak tanımlanması
  pinMode(POT_PIN, INPUT);
  // LED'lerin çıkış olarak tanımlanması
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  // 7 segment pinlerinin çıkış olarak ayarlanması
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);
  pinMode(SEG_1, OUTPUT);
  pinMode(SEG_2, OUTPUT);
  // Tuğla dizisini başlat
  initBricks();
  displayScore(score);
}

void loop() {

  // Paletin kontrolü
  int potValue = analogRead(POT_PIN);
  paletX = map(potValue, 0, 1023, 0, SCREEN_WIDTH - PALET_WIDTH);
  // Topun hareketi ve çarpışma kontrolü
  moveBall();
  // Topun duvarlarla ve paletle ve tuğlalarla çarpma kontrolü
  checkWallCollision();
  checkPaddleCollision();
  // Skoru 7 segment display'de gösterme
  displayScore(score);
  // OLED ekranı temizleme
  display.clearDisplay();
  // Tuğlaları çiz
  drawBricks();
  //Tuğlayla çarpışma kontrolü
  checkCollisionWithBricks();
  // Objeyi çiz
  drawObject();
  // Objeyi hareket ettir
  moveObject();
  // Skorun ekrana yazdırılması
  display.setTextSize(1); // Metin boyutunu küçült
  display.setCursor(0, 0);
  display.print("Score: ");
  display.println(score);
  // Canların ekrana yazdırılması
  display.setCursor(80, 0);
  display.print("Lives: ");
  display.println(lives);
  // Paletin ve topun ekrana çizilmesi
  display.fillRect(paletX, paletY, PALET_WIDTH, PALET_HEIGHT, SSD1306_WHITE);
  display.fillCircle(ballX, ballY, BALL_SIZE, SSD1306_WHITE);
  // LED'lerin kontrolü
  updateLEDs();
  
  // Gecikme eklenerek oyunun hızının kontrol edilmesi
  delay(10);
  //// Tüm tuğlalar yok edildiyse
  //if (checkVictory()) // Seviye geçiş fonksiyonunu çağır
  //  increaseLevel();
  //// Ekranın güncellenmesi
  display.display();
}

//// Bölüm geçiş durumunu kontrol eden fonksiyon
//bool checkVictory() {
//  for (int i = 0; i < NUM_BRICKS_X; i++) {
//    for (int j = 0; j < NUM_BRICKS_Y; j++) {
//      if (bricks[i][j] == 1) {
//        return false; // Henüz tüm tuğlalar yok edilmedi
//      }
//    }
//  }
//  return true; // Tüm tuğlalar yok edildi, bölüm geçildi
//}
//
//// Seviye geçişinde topun hızını artıran fonksiyon
//void increaseLevel() {
//  // Topun hızını artır
//  ballSpeedX *= 1.2;
//  ballSpeedY *= 1.2;
//  
//  // Tuğla dizisini yeniden başlat
//  initBricks();
//
//  // Ekranı temizle ve başlangıç ekranını göster
//  display.clearDisplay();
//  display.setTextSize(1);
//  display.setCursor(20, 20);
//  display.println("Next Level!"); // Yeni seviye bildirimi
//  display.display();
//
//  // Birkaç saniye beklet, sonra devam et
//  delay(5000);
//}

void moveBall() {
  // Topun hareketi
  ballX += ballSpeedX;
  ballY += ballSpeedY;
  // Topun ekranın alt kısmına ulaşıp ulaşmadığının kontrolü
  if (ballY >= SCREEN_HEIGHT) {
    loseLife();
  }
}
void loseLife() {
  // Can kaybı işlemleri
  lives--;
  // Eğer canlar bittiyse oyunu bitir
  if (lives == 0) {
    gameOver();
  } else {
    // Yeni can sayısını ekrana yazdır
    display.setCursor(80, 0);
    display.print("Lives: ");
    display.println(lives);
    display.display();
    delay(200);
    // Topun başlangıç konumunu ayarla
    ballX = paletX + (PALET_WIDTH / 2);
    ballY = paletY - BALL_SIZE;
    ballSpeedX = BALL_SPEED_X;
    ballSpeedY = BALL_SPEED_Y;
  }
}
void gameOver() {
  updateLEDs();
  // Oyun bittiğinde ekrana "Game Over" yazısı yazdır
  display.clearDisplay();
  display.setCursor(35, 20);
  display.println("GAME OVER");
  display.setCursor(35, 40);
  display.print("SCORE: ");
  display.println(score);
  display.display();
  delay(3000);
  // Oyunu sıfırla
  resetGame();
  initBricks();
  // Oyun döngüsünü başlat
  loop();
}
void resetGame() {
  // Topun ve paletin başlangıç konumunu ayarla
  paletX = (SCREEN_WIDTH - PALET_WIDTH) / 2;
  paletY = SCREEN_HEIGHT - 10;
  ballX = paletX + (PALET_WIDTH / 2);
  ballY = paletY - BALL_SIZE;
 
  // Can sayısını yeniden ayarla
  lives = 3;
   // Skoru sıfırla
  score = 0;
  // Ekranı temizle ve başlangıç ekranını göster
  display.clearDisplay();
  display.display();
}
void initBricks() {
  randomSeed(analogRead(0)); // Rastgelelik için seed ayarla
  for (int i = 0; i < NUM_BRICKS_X; i++) {
    for (int j = 0; j < NUM_BRICKS_Y; j++) {
      bricks[i][j] = random(2); // Tuğla varsa 1, yoksa 0
    }
  }
}
void drawBricks() {
  display.setTextSize(1); // Tuğla boyutunu küçült
  for (int j = 0; j < NUM_BRICKS_Y; j++) {
    for (int i = 0; i < NUM_BRICKS_X; i++) {
      if (bricks[i][j] == 1) { // Tuğla varsa çiz
        int brickX = (BRICK_WIDTH + BRICK_GAP) * i;
        int brickY = (BRICK_HEIGHT + BRICK_GAP) * j + 10; // Yazıların altına yerleştir
        display.fillRect(brickX, brickY, BRICK_WIDTH, BRICK_HEIGHT, SSD1306_WHITE);
      }
    }
  }
}
//AABB algoritması (iki objenin birbirine deyme kontolü)
void checkCollisionWithBricks() {
  int ballRight = ballX + BALL_SIZE;
  int ballBottom = ballY + BALL_SIZE;

  for (int i = 0; i < NUM_BRICKS_X; i++) {
    for (int j = 0; j < NUM_BRICKS_Y; j++) {
      if (bricks[i][j] == 1) { // Eğer tuğla varsa
        int brickX = (BRICK_WIDTH + BRICK_GAP) * i;
        int brickY = (BRICK_HEIGHT + BRICK_GAP) * j;

        // Topun tuğla alanına girmesi durumunu kontrol et
        if (ballRight >= brickX && ballX <= brickX + BRICK_WIDTH &&
            ballBottom >= brickY && ballY <= brickY + BRICK_HEIGHT) {
          // Topun yönünü değiştir
          ballSpeedY = -ballSpeedY;

          // Tuğlayı kaldır
          bricks[i][j] = 0;

          // Skoru artır
          score++;
          
          // Objeyi düşürme işlemini gerçekleştir
          dropObject(i, j);

          // Çarpışma tespit edildi, döngüyü sonlandır
          return;
        }
      }
    }
  }
}
void checkWallCollision() {
  // Topun duvarlarla çarpma kontrolü
  if (ballX <= 0 || ballX + BALL_SIZE >= SCREEN_WIDTH) {
    ballSpeedX = -ballSpeedX;
  }
  if (ballY <= 0) {
    ballSpeedY = -ballSpeedY;
  }
}
void checkPaddleCollision() {
  // Topun paletle çarpma kontrolü
  if (ballY + BALL_SIZE >= paletY && ballY <= paletY + PALET_HEIGHT) {
    if (ballX + BALL_SIZE >= paletX && ballX <= paletX + PALET_WIDTH) {
      ballSpeedY = -BALL_SPEED_Y;
    }
  }
}
void updateLEDs() {
  // LED'lerin kontrolü
  if (lives == 3) {
    digitalWrite(GREEN_LED_PIN, HIGH);  // Yeşil LED'i yak
    digitalWrite(YELLOW_LED_PIN, HIGH); // Sarı LED'i yak
    digitalWrite(RED_LED_PIN, HIGH);    // Kırmızı LED'i yak
  } else if (lives == 2) {
    digitalWrite(YELLOW_LED_PIN, HIGH); // Sarı LED'i yak
    digitalWrite(GREEN_LED_PIN, LOW);   // Kırmızı LED'i söndür
  } else if (lives == 1) {
    digitalWrite(GREEN_LED_PIN, LOW);   // Yeşil ve sarı LED'leri söndür
    digitalWrite(YELLOW_LED_PIN, LOW);
  } else {
    digitalWrite(GREEN_LED_PIN, LOW);   // Tüm LED'leri söndür
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
  }
}
const byte digits[10][7] ={
  {1,1,1,1,1,1,0},  // 0
  {0,1,1,0,0,0,0},  // 1
  {1,1,0,1,1,0,1},  // 2
  {1,1,1,1,0,0,1},  // 3
  {0,1,1,0,0,1,1},  // 4
  {1,0,1,1,0,1,1},  // 5
  {1,0,1,1,1,1,1},  // 6
  {1,1,1,0,0,0,0},  // 7
  {1,1,1,1,1,1,1},  // 8
  {1,1,1,1,0,1,1}   // 9
};
// Skoru 7 segment display'e gösteren fonksiyon
void displayScore(int score) {
  // Her bir rakamın 7 segment display'de gösterilmesi için gerekli kombinasyonlar
    digitalWrite(SEG_2, HIGH);
    displayDigit(score / 10);
    digitalWrite(SEG_1, LOW);
    digitalWrite(SEG_2, HIGH);
    delay(1);
    digitalWrite(SEG_1, HIGH);
    displayDigit(score % 10);
    digitalWrite(SEG_1, HIGH);
    digitalWrite(SEG_2, LOW);
}
// 7 segment display'e bir rakamı gösteren fonksiyon
void displayDigit(int digit) {
  digitalWrite(SEG_A, digits[digit][0]);
  digitalWrite(SEG_B, digits[digit][1]);
  digitalWrite(SEG_C, digits[digit][2]);
  digitalWrite(SEG_D, digits[digit][3]);
  digitalWrite(SEG_E, digits[digit][4]);
  digitalWrite(SEG_F, digits[digit][5]);
  digitalWrite(SEG_G, digits[digit][6]);
}


void dropObject(int brickIndexX, int brickIndexY) {
  int chance = random(100);
  if (chance < OBJECT_DROP_CHANCE) { // Eğer düşme şansı varsa
    objectActive = true;
    objectX = brickIndexX * (BRICK_WIDTH + BRICK_GAP) + (BRICK_WIDTH / 2) - (OBJECT_WIDTH / 2);
    objectY = brickIndexY * (BRICK_HEIGHT + BRICK_GAP) + BRICK_HEIGHT + BRICK_GAP;
  }
}

void drawObject() {
  if (objectActive) {
    display.fillRect(objectX, objectY, OBJECT_WIDTH, OBJECT_HEIGHT, SSD1306_WHITE);
  }
}

void moveObject() {
  if (objectActive) {
    objectY += OBJECT_SPEED;
    // Eğer obje ekranın altına ulaşırsa
    if (objectY > SCREEN_HEIGHT) {
      objectActive = false;
    }
    // Objeyi yakalama kontrolü
    if (objectX + OBJECT_WIDTH >= paletX && objectX <= paletX + PALET_WIDTH &&
        objectY + OBJECT_HEIGHT >= paletY && objectY <= paletY + PALET_HEIGHT) {
      // Can sayısını artır
      if (lives < 3) {
        lives++;
        // Can sayısını LED'lerde göster
        updateLEDs();
      }
      objectActive = false; // Objeyi yok et
    }
  }
}