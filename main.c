#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "ImageData.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "EPD_2in13_V3.h"
#include <stdlib.h>
#include "./wifi.h"

#include "pico/cyw43_arch.h"

static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    if (result) {
        printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
            result->ssid, result->rssi, result->channel,
            result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
            result->auth_mode);
    }
    return 0;
}

#include "hardware/vreg.h"
#include "hardware/clocks.h"

int initialize_board(){
    bi_decl(bi_program_description("https://github.com/pirakansa"));

    stdio_init_all();
    if(DEV_Module_Init()!=0){
        return -1;
    }
    EPD_2in13_V3_Init();
    EPD_2in13_V3_Clear();

    printf("boot pico program\n");
    return 0;
}

int main ()
{
    if(initialize_board()!=0){
        return -1;
    }

    //Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2in13_V3_WIDTH % 8 == 0)? (EPD_2in13_V3_WIDTH / 8 ): (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 0, WHITE);
	// Paint_Clear(WHITE);

    // Paint_SelectImage(BlackImage);
    Paint_DrawBitMap(lennaImage);
    EPD_2in13_V3_Display(BlackImage);

    TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));
    if (!state) {
        DEBUG_printf("failed to allocate state\n");
        return 1;
    }

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    // Get notified if the user presses a key
    state->context = cyw43_arch_async_context();
    key_pressed_worker.user_data = state;
    async_context_add_when_pending_worker(cyw43_arch_async_context(), &key_pressed_worker);
    stdio_set_chars_available_callback(key_pressed_func, state);

    const char *ap_name = "picow_test";
#if 0
    const char *password = "password";
#else
    const char *password = NULL;
#endif

    cyw43_arch_enable_ap_mode(ap_name, password, CYW43_AUTH_WPA2_AES_PSK);

    ip4_addr_t mask;
    IP4_ADDR(ip_2_ip4(&state->gw), 192, 168, 4, 1);
    IP4_ADDR(ip_2_ip4(&mask), 255, 255, 255, 0);

    // Start the dhcp server
    dhcp_server_t dhcp_server;
    dhcp_server_init(&dhcp_server, &state->gw, &mask);

    // Start the dns server
    dns_server_t dns_server;
    dns_server_init(&dns_server, &state->gw);

    if (!tcp_server_open(state, ap_name)) {
        DEBUG_printf("failed to open server\n");
        return 1;
    }

    state->complete = false;
    while(!state->complete) {
        // the following #ifdef is only here so this same example can be used in multiple modes;
        // you do not need it in your code
#if PICO_CYW43_ARCH_POLL
        // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
        // main loop (not from a timer interrupt) to check for Wi-Fi driver or lwIP work that needs to be done.
        cyw43_arch_poll();
        // you can poll as often as you like, however if you have nothing else to do you can
        // choose to sleep until either a specified time, or cyw43_arch_poll() has work to do:
        cyw43_arch_wait_for_work_until(make_timeout_time_ms(1000));
#else
        // if you are not using pico_cyw43_arch_poll, then Wi-FI driver and lwIP work
        // is done via interrupt in the background. This sleep is just an example of some (blocking)
        // work you might be doing.
        sleep_ms(1000);
#endif
    }

//     absolute_time_t scan_time = nil_time;
//     bool scan_in_progress = false;

//     while(1)
//     {
//         if (absolute_time_diff_us(get_absolute_time(), scan_time) < 0) {
//             if (!scan_in_progress) {
//                 cyw43_wifi_scan_options_t scan_options = {0};
//                 int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
//                 if (err == 0) {
//                     printf("\nPerforming wifi scan\n");
//                     scan_in_progress = true;
//                 } else {
//                     printf("Failed to start scan: %d\n", err);
//                     scan_time = make_timeout_time_ms(10000); // wait 10s and scan again
//                 }
//             } else if (!cyw43_wifi_scan_active(&cyw43_state)) {
//                 scan_time = make_timeout_time_ms(10000); // wait 10s and scan again
//                 scan_in_progress = false; 
//             }
//         }
//         #if PICO_CYW43_ARCH_POLL
//         // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
//         // main loop (not from a timer) to check for Wi-Fi driver or lwIP work that needs to be done.
//         cyw43_arch_poll();
//         // you can poll as often as you like, however if you have nothing else to do you can
//         // choose to sleep until either a specified time, or cyw43_arch_poll() has work to do:
//         cyw43_arch_wait_for_work_until(scan_time);
// #else
//         // if you are not using pico_cyw43_arch_poll, then WiFI driver and lwIP work
//         // is done via interrupt in the background. This sleep is just an example of some (blocking)
//         // work you might be doing.
//         sleep_ms(1000);
// #endif
//     }

    tcp_server_close(state);
    dns_server_deinit(&dns_server);
    dhcp_server_deinit(&dhcp_server);
    cyw43_arch_deinit();
    free(BlackImage);
    BlackImage = NULL;
    
    DEV_Module_Exit();
    return 0;
}
