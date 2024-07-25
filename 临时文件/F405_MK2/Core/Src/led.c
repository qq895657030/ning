#include "gpio.h"


// IO pin identification
// make sure that ioTag_t can't be assigned into IO_t without warning
typedef uint8_t ioTag_t;       // packet tag to specify IO pin
typedef void* IO_t;            // type specifying IO pin. Currently ioRec_t pointer, but this may change

// NONE initializer for ioTag_t variables
#define IO_TAG_NONE 0

// NONE initializer for IO_t variable
#define IO_NONE ((IO_t)0)

// both ioTag_t and IO_t are guarantied to be zero if pinid is NONE (no pin)
// this simplifies initialization (globals are zeroed on start) and allows
//  omitting unused fields in structure initializers.
// it is also possible to use IO_t and ioTag_t as boolean value
//   TODO - this may conflict with requirement to generate warning/error on IO_t - ioTag_t assignment
//   IO_t being pointer is only possibility I know of ..

// pin config handling
// pin config is packed into ioConfig_t to decrease memory requirements
// IOCFG_x macros are defined for common combinations for all CPUs; this
//  helps masking CPU differences

typedef uint8_t ioConfig_t;  // packed IO configuration


#define STATUS_LED_NUMBER 3

typedef struct statusLedConfig_s {
    ioTag_t ioTags[STATUS_LED_NUMBER];
    uint8_t inversion;
} statusLedConfig_t;

static IO_t leds[STATUS_LED_NUMBER];
static uint8_t ledInversion = 0;

#ifndef LED0_PIN
#define LED0_PIN NONE
#endif

#ifndef LED1_PIN
#define LED1_PIN NONE
#endif

#ifndef LED2_PIN
#define LED2_PIN NONE
#endif

void pgResetFn_statusLedConfig(statusLedConfig_t *statusLedConfig)
{
    statusLedConfig->ioTags[0] = IO_TAG(LED0_PIN);
    statusLedConfig->ioTags[1] = IO_TAG(LED1_PIN);
    statusLedConfig->ioTags[2] = IO_TAG(LED2_PIN);

    statusLedConfig->inversion = 0
#ifdef LED0_INVERTED
    | BIT(0)
#endif
#ifdef LED1_INVERTED
    | BIT(1)
#endif
#ifdef LED2_INVERTED
    | BIT(2)
#endif
    ;
}

void ledInit(const statusLedConfig_t *statusLedConfig)
{
    ledInversion = statusLedConfig->inversion;
    for (int i = 0; i < STATUS_LED_NUMBER; i++) {
        if (statusLedConfig->ioTags[i]) {
            leds[i] = IOGetByTag(statusLedConfig->ioTags[i]);
            IOInit(leds[i], OWNER_LED, RESOURCE_INDEX(i));
            IOConfigGPIO(leds[i], IOCFG_OUT_PP);
        } else {
            leds[i] = IO_NONE;
        }
    }

    LED0_OFF;
    LED1_OFF;
    LED2_OFF;
}

void ledToggle(int led)
{
    IOToggle(leds[led]);
}

void ledSet(int led, bool on)
{
    const bool inverted = (1 << (led)) & ledInversion;
    IOWrite(leds[led], on ? inverted : !inverted);
}
