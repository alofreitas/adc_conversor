# Projeto - Controle de LEDs RGB e Display SSD1306 com Joystick na BitDogLab

## Link do vídeo
https://drive.google.com/file/d/18w_AlF0dtZFNysxnXW8Yd7iS05__I5gr/view?usp=sharing

## Descrição
Este projeto tem como objetivo utilizar a placa BitDogLab para capturar valores de um joystick e controlar LEDs RGB por meio de PWM, além de exibir um quadrado dinâmico em um display SSD1306 via protocolo I2C. O sistema também responde a eventos de botões por meio de interrupções, alternando estados e estilos de borda do display.

## Requisitos
- Placa de desenvolvimento BitDogLab
- Joystick analógico
- LED RGB
- Display OLED SSD1306
- Ambiente de desenvolvimento configurado para RP2040
- Biblioteca pico-sdk instalada

## Conexão dos Componentes
- **LED RGB**
  - Azul -> GPIO 11
  - Verde -> GPIO 12
  - Vermelho -> GPIO 13
- **Joystick**
  - Eixo X -> GPIO 26 (ADC)
  - Eixo Y -> GPIO 27 (ADC)
  - Botão -> GPIO 22
- **Botão A** -> GPIO 5
- **Display SSD1306** (I2C)
  - SDA -> GPIO 14
  - SCL -> GPIO 15

## Compilando e Executando
1. Configure o ambiente de desenvolvimento do RP2040 conforme a documentação oficial.
2. Clone o repositório e acesse a pasta do projeto:
   ```bash
   git clone https://github.com/alofreitas/adc_conversor.git
   cd nome_do_projeto
   ```
3. Compile o programa:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
4. Transfira o arquivo `.uf2` para a BitDogLab:
   - Conecte a placa ao computador mantendo o botão BOOTSEL pressionado.
   - Copie o arquivo gerado `main.uf2` para a unidade montada.
   - O programa será executado automaticamente após a transferência.

## Funcionamento
1. O joystick fornece valores analógicos (0 a 4095) para os eixos X e Y.
2. Os LEDs RGB são controlados por PWM:
   - Azul: varia conforme o eixo Y.
   - Vermelho: varia conforme o eixo X.
3. O display OLED exibe um quadrado 8x8 pixels que se move conforme o joystick.
4. O botão do joystick:
   - Alterna o estado do LED Verde.
   - Modifica a borda do display a cada pressiona.
5. O botão A ativa ou desativa os LEDs RGB.

## Autor
Ana Luiza Oliveira de Freitas
