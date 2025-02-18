#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h" 
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "ssd1306.h"

// Definições de pinos
#define LED_VERDE 11
#define LED_AZUL 12
#define LED_VERMELHO 13

#define BOTAO_A 5
#define I2C_SDA 14
#define I2C_SCL 15
#define JOYSTICK_X 26  
#define JOYSTICK_Y 27  
#define JOYSTICK_BOTAO 22 
#define I2C_PORT i2c1
#define endereco 0x3C
#define WRAP 4095

// Definições de display
volatile bool estado_led = true; // Estado dos LEDs
volatile bool led_verde_estado = false; // Estado do LED verde
volatile int borda = 0; // Estilo da borda
volatile uint32_t ultimo = 0; // Último tempo de pressionamento dos botões

// Função de interrupção
void gpio_irq_handler(uint gpio, uint32_t events) {
    // Uso de debounce
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time()); // Tempo atual
    if(tempo_atual - ultimo > 200){ // Verifica se passou 200ms
        // Verifica qual botão foi pressionado
        if (gpio == BOTAO_A){
            estado_led = !estado_led; // Alterna o estado dos LEDs caso o botão A seja pressionado
            if (!estado_led) {
                    pwm_set_gpio_level(LED_VERMELHO, 0); // Desliga o LED vermelho
                    pwm_set_gpio_level(LED_AZUL, 0); // Desliga o LED azul
            }
        } else if (gpio == JOYSTICK_BOTAO){ // Se o botão do joystick for pressionado
            led_verde_estado = !led_verde_estado; // Alterna o estado do LED verde
            gpio_put(LED_VERDE, led_verde_estado); // Atualiza o estado do LED verde
            borda = (borda + 1) % 3; // Alterna o estilo da borda
        }
        ultimo = tempo_atual; // Atualiza o tempo de pressionamento
    }
}

// Função para configurar o PWM
void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM); // Configura o pino para PWM
    uint slice = pwm_gpio_to_slice_num(pin); // Obtém o slice do PWM
    pwm_config config = pwm_get_default_config(); // Configuração padrão do PWM
    pwm_config_set_wrap(&config, WRAP); // Configura o wrap para 4095
    pwm_init(slice, &config, true); // Inicializa o PWM
}


int main(){
    // Inicialização dos subsistemas
    stdio_init_all();

    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(JOYSTICK_BOTAO);
    gpio_set_dir(JOYSTICK_BOTAO, GPIO_IN);
    gpio_pull_up(JOYSTICK_BOTAO);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BOTAO, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Configura a interrupção para o botão do joystick
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Configura a interrupção para o botão A
    setup_pwm(LED_VERMELHO);
    setup_pwm(LED_AZUL);
    i2c_init(I2C_PORT, 400 * 1000); 
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); 
    ssd1306_config(&ssd); 
    ssd1306_send_data(&ssd);

    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);

    // Variáveis para leitura dos valores do ADC
    uint16_t adc_x; 
    uint16_t adc_y; 
    
    // Variáveis para a posição do cursor
    int eixo_x = WIDTH / 2 - 4, eixo_y = HEIGHT / 2 - 4; 

    while (true){

        adc_select_input(1); // Seleciona o pino do joystick no eixo X
        adc_x = adc_read(); // Lê o valor do ADC
        adc_select_input(0); // Seleciona o pino do joystick no eixo Y
        adc_y = adc_read(); // Lê o valor do ADC
    
        // Cálculo da margem de zona morta
        int delta_x = abs(2048 - adc_x); // Calcula a diferença entre o valor lido e o valor central em relação ao eixo X
        int delta_y = abs(2048 - adc_y); // Calcula a diferença entre o valor lido e o valor central em relação ao eixo Y
        int limite = 200; // Define o limite de zona morta
    
        // Atualização dos LEDs
        if (estado_led) {
            if (delta_x > limite) { // Se a diferença for maior que o limite
                pwm_set_gpio_level(LED_VERMELHO, (delta_x - limite) * 2); // Ajusta o brilho do LED vermelho
            } else {
                pwm_set_gpio_level(LED_VERMELHO, 0); // Desliga o LED vermelho
            }
    
            if (delta_y > limite) { // Se a diferença for maior que o limite
                pwm_set_gpio_level(LED_AZUL, (delta_y - limite) * 2); // Ajusta o brilho do LED azul
            } else {
                pwm_set_gpio_level(LED_AZUL, 0); // Desliga o LED azul
            }
        } else {
            pwm_set_gpio_level(LED_VERMELHO, 0); // Desliga o LED vermelho
            pwm_set_gpio_level(LED_AZUL, 0); // Desliga o LED azul
        }
        
        eixo_x = (adc_x * (WIDTH - 8)) / 4095; // Calcula a posição do cursor no eixo X
        eixo_y = (adc_y * (HEIGHT - 8)) / 4095; // Calcula a posição do cursor no eixo Y
    
        ssd1306_fill(&ssd, false);

        // Desenha a borda 
        if (borda == 1) {
            ssd1306_rect(&ssd, 2, 2, WIDTH - 4, HEIGHT - 4, true, false); 
        } else if (borda == 2) {
            ssd1306_rect(&ssd, 0, 0, WIDTH, HEIGHT, true, false); 
        }

        ssd1306_rect(&ssd, eixo_y, eixo_x, 8, 8, true, false);
        ssd1306_send_data(&ssd);
    
        sleep_ms(100);
    }
    
    
}
