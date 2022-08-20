#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/uart.h"
#include <string.h>
#include <stdlib.h>

#include "../lib/esp_01/esp_01.h"
#include "../lib/joystick/joystick.h"

// should put in defines
// char *wifi_name = "Stofa70521";
// char *wifi_password = "bis56lage63";
// char *server_ip = "192.168.87.178";
// char *server_port = "4000";


char *wifi_name = "ESP32_wifi";
char *wifi_password = "1234567890";
char *server_ip = "192.168.8.1";
char *server_port = "3500";

volatile bool send_data = false;

void button_callback(){
    printf("\nCALLED\n");
    send_data = !send_data;
}

char* int_to_string(uint number){
    uint8_t negative = 0;
    // if (number < 0){
    //     negative = 1;
    //     number *= -1;
    // }
    if(number == 0){
        char *string = malloc(2);
        string[0] = '0';
        string[1] = '\0'; 
        return string;
    }
    
    // get digit count
    uint8_t count = 0;   // variable declaration 
    int number_temp = number; 
    while(number_temp!=0)  {  
        number_temp=number_temp/10;
        count++;  
    }    

    char *string = malloc(count+negative+1);
    
    for(uint8_t i = 0+negative; i < count+negative; i++){
        uint32_t divisor = 1;
        for(uint8_t j = 0; j < count-1-i; j++){
            divisor = divisor * 10;
        }

        string[i] = 48 + (number / divisor);
        number =  number % divisor;
    }  

    if(negative){
        string[0] = '-';
    }

    string[count] = '\0';

    return string;
}

char* generate_message_joystick(uint x, uint y, char *ADDRESS){
    char* x_str = int_to_string(x);
    char* y_str = int_to_string(y);

    uint connection_length = strlen("GET //") + strlen(" HTTP/1.1\r\nHost: ")+ strlen("\r\n") + strlen(ADDRESS) + strlen(x_str) + strlen(y_str);
    char connection[connection_length];
    memset(connection, 0, connection_length * sizeof(char));

    strcat(connection, "GET /");
    strcat(connection, x_str);
    strcat(connection, "/");
    strcat(connection, y_str);
    strcat(connection, " HTTP/1.1\r\nHost: ");
    strcat(connection, ADDRESS);
    strcat(connection, "\r\n");

    free(x_str);
    free(y_str);

    char *string = malloc(connection_length+1);

    for(uint8_t i = 0; i < connection_length-1; i++){
        string[i] = connection[i];
    }
    string[connection_length] = '\0';

    return string;
}

// esp 01
int main() {
    stdio_init_all();
    
    // led
    gpio_init(2);
    gpio_set_dir(2, GPIO_OUT);
    gpio_put(2, 1);

    // sleep because the program executes too quick
    // and doesn't cache what is sent
    sleep_ms(5000);

    // init joystick
    init_joystick(26,27,22, button_callback);

    // init esp 01
    init_esp_01_client(uart1, 3);
    // esp_01_connect_wifi(uart1, wifi_name, wifi_password);
    // esp_01_send_http(
    //     uart1, 
    //     server_ip, 
    //     server_port, 
    //     "GET /users HTTP/1.1\r\nHost: 192.168.8.1\r\n"
    // );


    while (true) {
        if(send_data){
            char *string = generate_message_joystick((uint)get_x_percentage(), (uint)get_y_percentage(), server_ip);
            printf(string);
            esp_01_send_http(
                uart1, 
                server_ip, 
                server_port, 
                string
            );

            
            free(string);

            printf("\nCurrent x: %d\n", (uint)get_x_percentage());
            printf("Current y: %d\n", (uint)get_y_percentage());
        }
        
        gpio_put(2, 1);
        sleep_ms(100);

        gpio_put(2, 0);
        sleep_ms(100);
    }
}

// esp_01_send_http(
//     uart1, 
//     server_ip, 
//     server_port, 
//     "GET / HTTP/1.1\r\nHost: 192.168.87.178\r\n"
// );

// Blink led and print to serial monitor
// #include "pico/stdlib.h"
// #include <stdio.h>

// int main() {
//     stdio_init_all();
//     const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    
//     gpio_init(LED_PIN);
//     gpio_set_dir(LED_PIN, GPIO_OUT);

//     while (true) {
//         gpio_put(LED_PIN, 1);
//         sleep_ms(100);
//         printf("ON\n");

//         gpio_put(LED_PIN, 0);
//         sleep_ms(100);
//         printf("OFF\n");
//     }
// }