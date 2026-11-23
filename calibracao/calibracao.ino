#include <Wire.h>

#define SDA_PIN 8
#define SCL_PIN 9

#define MPU_ADDR      0x68   // AD0 no GND
#define REG_PWR_MGMT1 0x6B
#define REG_ACCEL_CFG 0x1C
#define REG_ACCEL_OUT 0x3B

#define LSB_POR_G 16384.0    // faixa ±2g
#define N_LEITURAS 200       // minimo do roteiro: 100

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);

  Wire.beginTransmission(MPU_ADDR);   // acorda o MPU6050
  Wire.write(REG_PWR_MGMT1);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(MPU_ADDR);   // faixa ±2g
  Wire.write(REG_ACCEL_CFG);
  Wire.write(0x00);
  Wire.endTransmission();

  Serial.println("Posicione o sensor e envie qualquer tecla para medir.");
}

void loop() {
  if (!Serial.available()) return;
  while (Serial.available()) Serial.read();   // limpa o buffer

  double soma[3] = {0, 0, 0};
  int ok = 0;

  while (ok < N_LEITURAS) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(REG_ACCEL_OUT);
    Wire.endTransmission(false);

    if (Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)6) == 6) {
      soma[0] += (int16_t)((Wire.read() << 8) | Wire.read());
      soma[1] += (int16_t)((Wire.read() << 8) | Wire.read());
      soma[2] += (int16_t)((Wire.read() << 8) | Wire.read());
      ok++;
    }
    delay(10);
  }

  Serial.printf("Media (g)  X: %.5f   Y: %.5f   Z: %.5f\n",
                soma[0] / ok / LSB_POR_G,
                soma[1] / ok / LSB_POR_G,
                soma[2] / ok / LSB_POR_G);
}
