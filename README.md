# Medição de ângulo de inclinação com MPU-6050 e ESP32-C3 Mini

Trabalho prático de Instrumentação Eletrônica para Engenharia (UnB/FGA, 02/2026), Entrega 2:
calibração estática do acelerômetro e medição do ângulo de inclinação.

## Hardware

| MPU-6050 | ESP32-C3 Mini |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO8 |
| SCL | GPIO9 |
| AD0 | GND (endereço I²C 0x68) |

## Estrutura

- `calibracao/`: mede a média de 200 leituras por posição (6 posições) e mostra X, Y, Z em g.
- `angulo/`: aplica ganho e offset e calcula o ângulo de inclinação.

## Como usar

### 1. Calibração

1. Na Arduino IDE, ative *Tools → USB CDC On Boot → Enabled*.
2. Grave `calibracao/calibracao.ino`, abra o monitor serial (115200 baud).
3. Posicione o sensor em cada eixo para cima (+) e para baixo (−) e envie uma tecla em cada posição.
4. Calcule, para cada eixo $i$:

$$\text{Ganho}_i = \frac{\bar A_{+1g,i} - \bar A_{-1g,i}}{2} \qquad A_{offset,i} = \frac{\bar A_{+1g,i} + \bar A_{-1g,i}}{2}$$

### Parâmetros obtidos

| Eixo | Ganho | Offset [g] |
|---|---|---|
| X | 0,99761 | +0,01919 |
| Y | 1,00474 | −0,00058 |
| Z | 1,01942 | −0,02001 |

### 2. Medição do ângulo

1. Copie os valores de ganho e offset para as constantes no início de `angulo/angulo.ino`.
2. Fixe o sensor no braço com o eixo Z paralelo ao eixo de rotação.
3. Grave `angulo/angulo.ino` e abra o monitor serial. A saída é o ângulo em graus.

O ângulo é calculado por:

$$\theta = \sigma \cdot \mathrm{atan2}(C_X, C_Y)\cdot\frac{180}{\pi} + Z_0$$

com $C_i = (A_{saida,i} - A_{offset,i})/\text{Ganho}_i$, $\sigma = -1$ (sentido horário) e $Z_0 = -90{,}0^\circ$.

## Autor

Pedro, UnB/FGA
