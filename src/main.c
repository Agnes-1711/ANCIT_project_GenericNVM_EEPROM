#include "sdk_project_config.h"
#include "nvm_block.h"
#include "eeprom_i2c.h"
#include <string.h>
#include <stdint.h>
#include "peripherals_lpuart_1.h"
extern volatile uint16 NvM_LastDetError;
extern volatile uint16 Det_ErrorCount;
static uint8 flag_write = 1U;
static uint8 flag_read = 0U;
volatile uint8 invalid_block_status = 0U;
volatile uint16 invalid_block_det_error = 0U;
volatile uint8 boundary_test_status = 0U;
volatile uint8 overlap_test_status = 0U;
volatile uint8 unknown_write_status = 0U;
volatile uint16 unknown_write_det_error = 0U;
volatile uint8 allTestsPassed = 0U;

static uint8 data_write[20] =
{
    1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U,
    11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U
};

static uint8 data_read[20] = {0U};

volatile uint8 test_status = 0U;
static void UART_Print(const char *text)
{
    (void)LPUART_DRV_SendDataBlocking(
        INST_LPUART_1,
        (const uint8_t *)text,
        (uint32_t)strlen(text),
        1000U
    );
}


int main(void)
{
    /* 1. Initialize clock */
    CLOCK_DRV_Init(&clockMan1_InitConfig0);

    /* 2. Initialize pins */
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0,
                  g_pin_mux_InitConfigArr0);
    LPUART_DRV_Init(INST_LPUART_1,
                    &lpUartState0,
                    &lpuart_0_InitConfig0);

    /* 3. Initialize EEPROM backend */
    EEPROM_I2C_Init();

    /* 4. Register blocks and test read/write */
    if (NvM_RegisterBlock(1U, 0U, 1U) != E_OK)
        test_status = 1U;

    else if (NvM_RegisterBlock(2U, 16U, 20U) != E_OK)
        test_status = 2U;

    else if (NvM_WriteVariable(1U, &flag_write) != E_OK)
        test_status = 3U;

    else if (NvM_ReadVariable(1U, &flag_read) != E_OK)
        test_status = 4U;

    else if (NvM_WriteVariable(2U, data_write) != E_OK)
        test_status = 5U;

    else if (NvM_ReadVariable(2U, data_read) != E_OK)
        test_status = 6U;

    else if ((flag_read != flag_write) ||
             (memcmp(data_write, data_read,
                     sizeof(data_write)) != 0))
        test_status = 7U;

    else
        test_status = 10U;
    if (NvM_ReadVariable(99U, &flag_read) == E_NOT_OK)
    {
        invalid_block_status = 1U;
    }
    else
    {
        invalid_block_status = 2U;
    }
    invalid_block_det_error = NvM_LastDetError;

    if (NvM_WriteVariable(99U, &flag_write) == E_NOT_OK)
    {
        unknown_write_status = 1U;
    }
    else
    {
        unknown_write_status = 2U;
    }

    unknown_write_det_error = NvM_LastDetError;

    /* Test invalid EEPROM range */
    if (NvM_RegisterBlock(3U, 510U, 4U) == E_NOT_OK)
    {
        boundary_test_status = 1U;
    }
    else
    {
        boundary_test_status = 2U;
    }

    /* Test overlapping EEPROM region */
    if (NvM_RegisterBlock(3U, 16U, 4U) == E_NOT_OK)
    {
        overlap_test_status = 1U;
    }
    else
    {
        overlap_test_status = 2U;
    }
    if ((test_status == 10U) &&
        (invalid_block_status == 1U) &&
        (unknown_write_status == 1U) &&
        (boundary_test_status == 1U) &&
        (overlap_test_status == 1U) &&
        (Det_ErrorCount == 4U))
    {
        allTestsPassed = 1U;
    }
    else
    {
        allTestsPassed = 0U;
    }
    /* Display test results in PuTTY */
    UART_Print("\r\n===== GENERIC NvM TEST =====\r\n");

    UART_Print("Register Flag Block : ");
    UART_Print((test_status != 1U) ? "PASS\r\n" : "FAIL\r\n");

    UART_Print("Register Data Block : ");
    UART_Print((test_status != 1U && test_status != 2U)
               ? "PASS\r\n" : "FAIL\r\n");

    UART_Print("Write Flag          : ");
    UART_Print((test_status != 1U && test_status != 2U &&
                test_status != 3U) ? "PASS\r\n" : "FAIL\r\n");

    UART_Print("Read Flag           : ");
    UART_Print((test_status >= 5U || test_status == 10U)
               ? "PASS\r\n" : "FAIL\r\n");

    UART_Print("Flag Value          : ");
    UART_Print((flag_read == 1U) ? "1\r\n" : "0\r\n");

    UART_Print("Write 20-Byte Data  : ");
    UART_Print((test_status >= 6U || test_status == 10U)
               ? "PASS\r\n" : "FAIL\r\n");

    UART_Print("Read 20-Byte Data   : ");
    UART_Print((test_status == 10U) ? "PASS\r\n" : "FAIL\r\n");

    UART_Print("Data Compare        : ");
    UART_Print((test_status == 10U) ? "PASS\r\n" : "FAIL\r\n");

    UART_Print("Unknown Read Test   : ");
    UART_Print((invalid_block_status == 1U)
               ? "PASS\r\n" : "FAIL\r\n");

    UART_Print("Unknown Write Test  : ");
    UART_Print((unknown_write_status == 1U)
               ? "PASS\r\n" : "FAIL\r\n");

    UART_Print("Boundary Test       : ");
    UART_Print((boundary_test_status == 1U)
               ? "PASS\r\n" : "FAIL\r\n");

    UART_Print("Overlap Test        : ");
    UART_Print((overlap_test_status == 1U)
               ? "PASS\r\n" : "FAIL\r\n");

    UART_Print("DET Error Count     : ");
    UART_Print((Det_ErrorCount == 4U) ? "4\r\n" : "Unexpected value\r\n");

    UART_Print((allTestsPassed == 1U)
               ? "ALL TESTS PASSED\r\n"
               : "TESTS FAILED\r\n");

    while (1)
    {
    }
}
