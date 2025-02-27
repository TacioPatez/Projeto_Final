#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "pico/bootrom.h"

// Wifi Credentials
const char WIFI_SSID[] = "Shalom";
const char *WIFI_PASSWORD[] = {
    "senha123",
    "admin",
    "123456",
    "qwerty",
    "password",
    "ADILSONFREIRE"
};

// Display Constants
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
ssd1306_t ssd; // Initializer display struct
bool color = true; // Display Border

void display_init(){
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Clear display
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 58, color, !color); // Draw a rectangle
    ssd1306_draw_string(&ssd, "Projeto Final", 15, 30);
    ssd1306_send_data(&ssd);
}

void display_write(const char *str) {
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 58, color, !color); // Draw a rectangle
    ssd1306_draw_string(&ssd, str, 15, 30);
    ssd1306_send_data(&ssd);
}

void display_write2(const char *str, const char *str2) {
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 58, color, !color); // Draw a rectangle
    ssd1306_draw_string(&ssd, str, 15, 15);
    ssd1306_draw_string(&ssd, str2, 15, 35);
    ssd1306_send_data(&ssd);
}
void display_write4(const char *str, const char *str2, const char *str3, const char *str4) {
    ssd1306_fill(&ssd, false);
    ssd1306_rect(&ssd, 3, 3, 122, 58, color, !color); // Draw a rectangle
    ssd1306_draw_string(&ssd, str, 15, 7);
    ssd1306_draw_string(&ssd, str2, 8, 20);
    ssd1306_draw_string(&ssd, str3, 15, 33);
    ssd1306_draw_string(&ssd, str4, 8, 48);
    ssd1306_send_data(&ssd);
}

int main()
{
    stdio_init_all();
    display_init();
    sleep_ms(5000);

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        display_write("Falhar ao iniciar WiFi");
        printf("Falha ao iniciar WiFi\n");
        return -1;
    }

    // Array length
    int num_senhas = sizeof(WIFI_PASSWORD) / sizeof(WIFI_PASSWORD[0]);

    // Enable wifi station
    cyw43_arch_enable_sta_mode();
    printf("Conectando a %s... ", WIFI_SSID);
    display_write2("Conectando a", WIFI_SSID);

    for (int i = 0; i < num_senhas; i++) {
        display_write2("Conectando a", WIFI_SSID);
        printf("Senha: %s\n", WIFI_PASSWORD[i]);
        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD[i], CYW43_AUTH_WPA2_AES_PSK, 10000)) {
            printf("failed to connect.\n");
            display_write2("Falha ao", "conectar");
        } else {
            printf("Connected.\n");
            display_write("Conectado...");
            // Read the ip address in a human readable way
            uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
            char IP[16];
            sprintf(IP, "%d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
            display_write4("IP", IP, "Senha", WIFI_PASSWORD[i]);
            rom_reset_usb_boot(0, 0);
        }
        sleep_ms(5000);
    }
}
