#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "inc/ssd1306.h" // Biblioteca do display OLED
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "led.pio.h"

#define MICROPHONE_PIN 28  // Pino do microfone
#define LED_PIN 13         // LED vermelho está no pino 13
#define BUTTON_PIN 5       // Botão A
#define MATRIX_LED_PIN 7   // Matriz de LEDs 5x5
#define NUM_PIXELS 25      

// Configuração do I2C para o OLED
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_PORT i2c1
#define ENDERECO_OLED 0x3C

// Configuração do filtro e detecção de som
#define SAMPLE_SIZE 5
#define THRESHOLD 2100  

// Tempo para desligamento automático (3 minutos = 180000 ms)
#define TEMPO_DESLIGAMENTO 180000 

volatile int led_on = 0;  // Estado do LED
ssd1306_t display;       
PIO pio;
uint sm;
double r = 0.10, g = 0.10, b = 0.10;  // Branco para os LEDs da matriz

// Definição do padrão da CASA na matriz 5x5
double casa[25] = {
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 0.0, 1.0, 0.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};

// Sequência correta dos LEDs
int sequencia[25] = {
    0, 1, 2, 3, 4,
    9, 8, 7, 6, 5,
    10, 11, 12, 13, 14,
    19, 18, 17, 16, 15,
    20, 21, 22, 23, 24
};

// Converter valores RGB para WS2812
uint32_t matrix_rgb(double r, double g, double b) {
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Exibir a CASA na matriz de LEDs (Branco)
void desenhar_casa_led() {
    uint32_t valor_led;
    for (int i = 0; i < NUM_PIXELS; i++) {
        valor_led = matrix_rgb(r * casa[24 - i], g * casa[24 - i], b * casa[24 - i]);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Apagar LEDs da matriz
void apagar_matriz_led() {
    uint32_t valor_led = matrix_rgb(0.0, 0.0, 0.0);
    for (int i = 0; i < NUM_PIXELS; i++) {
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Inicializar ADC do microfone
void init_adc() {
    adc_init();
    adc_gpio_init(MICROPHONE_PIN);
    adc_select_input(2);
}

// Ler valor do microfone
uint16_t read_microphone() {
    return adc_read();
}

// Acender LED vermelho (pino 13)
void light_up_led() {
    gpio_put(LED_PIN, 1);
}

// Apagar LED vermelho
void turn_off_led() {
    gpio_put(LED_PIN, 0);
}

// Média móvel dos últimos valores
uint16_t moving_average(uint16_t new_sample, uint16_t *history, uint8_t *index) {
    history[*index] = new_sample;
    *index = (*index + 1) % SAMPLE_SIZE;
    uint32_t sum = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        sum += history[i];
    }
    return (uint16_t)(sum / SAMPLE_SIZE);
}

// Detectar som da campainha
int detect_bell_sound(uint16_t audio_signal, uint16_t *history, uint8_t *index) {
    uint16_t avg_signal = moving_average(audio_signal, history, index);
    printf("Valor lido: %d\n", audio_signal);
    return (avg_signal > THRESHOLD) ? 1 : 0;
}

// ⚡ Interrupção do botão A
void button_irq_handler(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN && events & GPIO_IRQ_EDGE_FALL) {
        if (led_on) {
            turn_off_led();
            led_on = 0;
            ssd1306_fill(&display, false);
            ssd1306_send_data(&display);
            apagar_matriz_led();
        }
    }
}

// Inicializar display OLED
void init_display() {
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&display, 128, 64, false, ENDERECO_OLED, I2C_PORT);
    ssd1306_config(&display);
    ssd1306_send_data(&display);
}

// Atualizar display OLED
void atualizar_display(ssd1306_t *ssd, const char *linha1, const char *linha2) {
    ssd1306_fill(ssd, false);
    printf("Exibindo no display: %s, %s\n", linha1, linha2);
    ssd1306_draw_string(ssd, linha1, 30, 20);
    ssd1306_draw_string(ssd, linha2, 40, 40);
    ssd1306_send_data(ssd);
    sleep_ms(1000);
}

// Loop principal
int main() {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Configuração do botão A
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Inicializar display OLED
    init_display();    

    // Inicializar PIO para LEDs WS2812
    pio = pio0;
    uint offset = pio_add_program(pio, &led_program);
    sm = pio_claim_unused_sm(pio, true);
    led_program_init(pio, sm, offset, MATRIX_LED_PIN);

    // Configurar interrupção do botão
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, button_irq_handler);

    // Inicializar ADC do microfone
    init_adc();

    uint16_t history[SAMPLE_SIZE] = {0};
    uint8_t index = 0;
    uint32_t tempo_inicio = to_ms_since_boot(get_absolute_time());

    while (true) {
        uint16_t audio_signal = read_microphone();

        if (detect_bell_sound(audio_signal, history, &index)) {
            if (!led_on) {
                light_up_led();
                led_on = 1;
                atualizar_display(&display, "Campainha", "Tocando");
                desenhar_casa_led();
                tempo_inicio = to_ms_since_boot(get_absolute_time());
            }
        }

        if (led_on && (to_ms_since_boot(get_absolute_time()) - tempo_inicio >= TEMPO_DESLIGAMENTO)) {
            turn_off_led();
            led_on = 0;
            ssd1306_fill(&display, false);
            ssd1306_send_data(&display);
            apagar_matriz_led();
        }

        sleep_ms(100);
    }
    return 0;
} 