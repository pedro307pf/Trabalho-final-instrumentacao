#include <Wire.h>

#define SDA_PIN 8
#define SCL_PIN 9

#define MPU_ADDR      0x68   // AD0 no GND
#define REG_PWR_MGMT1 0x6B
#define REG_ACCEL_CFG 0x1C
#define REG_ACCEL_OUT 0x3B

#define LSB_POR_G 16384.0f   // faixa ±2g

// ---- Calibracao (da tabela do relatorio) ----
const float GANHO_X  = 0.99761f;
const float GANHO_Y  = 1.00474f;
const float OFFSET_X = +0.01919f;   // g
const float OFFSET_Y = -0.00058f;   // g

// ---- Ajustes do angulo (referencia 0..180, sentido horario) ----
// Sempre trocar SINAL_ANGULO e ZERO_ANGULO juntos:
//   +1.0f e +90.0f   ou   -1.0f e -90.0f   (ZERO_ANGULO com o ajuste fino)
const float SINAL_ANGULO = -1.0f;
const float ZERO_ANGULO  = -90.0f;  // -90 menos 2.1 graus de correcao no zero
const int   N_MEDIA      = 10;      // leituras promediadas por saida

// Leva o angulo para a faixa [-90, 270), para a varredura 0..180 nao dar salto
float normaliza(float a) {
  while (a < -90.0f)  a += 360.0f;
  while (a >= 270.0f) a -= 360.0f;
  return a;
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // USB CDC do C3

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  Wire.beginTransmission(MPU_ADDR);   // acorda o MPU6050
  Wire.write(REG_PWR_MGMT1);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(MPU_ADDR);   // faixa ±2g
  Wire.write(REG_ACCEL_CFG);
  Wire.write(0x00);
  Wire.endTransmission();

  Serial.println("angulo_graus");
}

void loop() {
  double somaX = 0, somaY = 0;
  int ok = 0;

  for (int k = 0; k < N_MEDIA; k++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(REG_ACCEL_OUT);
    Wire.endTransmission(false);

    if (Wire.requestFrom((uint8_t)MPU_ADDR, (uint8_t)6) == 6) {
      int16_t ax = (Wire.read() << 8) | Wire.read();
      int16_t ay = (Wire.read() << 8) | Wire.read();
      Wire.read(); Wire.read();   // descarta o Z
      somaX += ax / LSB_POR_G;
      somaY += ay / LSB_POR_G;
      ok++;
    }
    delay(10);
  }

  if (ok == 0) {
    Serial.println("Erro na leitura I2C");
    return;
  }

  // Compensa offset e ganho
  float ax = (somaX / ok - OFFSET_X) / GANHO_X;
  float ay = (somaY / ok - OFFSET_Y) / GANHO_Y;

  float theta = SINAL_ANGULO * atan2f(ax, ay) * 180.0f / PI + ZERO_ANGULO;
  theta = normaliza(theta);

  Serial.printf("%.2f\n", theta);
}
