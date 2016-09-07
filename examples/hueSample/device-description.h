/*
 * Afero Device Profile header file
 * Device Description:		36f21273-5944-475f-b1d2-e97845865c53
 * Schema Version:	2
 */


#define ATTRIBUTE_TYPE_SINT8                                   2
#define ATTRIBUTE_TYPE_SINT16                                  3
#define ATTRIBUTE_TYPE_SINT32                                  4
#define ATTRIBUTE_TYPE_SINT64                                  5
#define ATTRIBUTE_TYPE_BOOLEAN                                 1
#define ATTRIBUTE_TYPE_UTF8S                                  20
#define ATTRIBUTE_TYPE_BYTES                                  21
#define ATTRIBUTE_TYPE_FLOAT32                                10

//region Service ID 1
// Attribute Light1
#define AF_LIGHT1                                              1
#define AF_LIGHT1_SZ                                           1
#define AF_LIGHT1_TYPE                    ATTRIBUTE_TYPE_BOOLEAN

// Attribute Light2
#define AF_LIGHT2                                              2
#define AF_LIGHT2_SZ                                           1
#define AF_LIGHT2_TYPE                    ATTRIBUTE_TYPE_BOOLEAN

// Attribute Light3
#define AF_LIGHT3                                              3
#define AF_LIGHT3_SZ                                           1
#define AF_LIGHT3_TYPE                    ATTRIBUTE_TYPE_BOOLEAN

// Attribute Light1Label
#define AF_LIGHT1LABEL                                         4
#define AF_LIGHT1LABEL_SZ                                    255
#define AF_LIGHT1LABEL_TYPE                 ATTRIBUTE_TYPE_UTF8S

// Attribute Light2Label
#define AF_LIGHT2LABEL                                         5
#define AF_LIGHT2LABEL_SZ                                    255
#define AF_LIGHT2LABEL_TYPE                 ATTRIBUTE_TYPE_UTF8S

// Attribute Light3Label
#define AF_LIGHT3LABEL                                         6
#define AF_LIGHT3LABEL_SZ                                    255
#define AF_LIGHT3LABEL_TYPE                 ATTRIBUTE_TYPE_UTF8S

// Attribute Light3Bri
#define AF_LIGHT3BRI                                           7
#define AF_LIGHT3BRI_SZ                                        2
#define AF_LIGHT3BRI_TYPE                  ATTRIBUTE_TYPE_SINT16

// Attribute pair
#define AF_PAIR                                                8
#define AF_PAIR_SZ                                             1
#define AF_PAIR_TYPE                      ATTRIBUTE_TYPE_BOOLEAN

// Attribute Bootloader Version
#define AF_BOOTLOADER_VERSION                               2001
#define AF_BOOTLOADER_VERSION_SZ                               8
#define AF_BOOTLOADER_VERSION_TYPE         ATTRIBUTE_TYPE_SINT64

// Attribute Softdevice Version
#define AF_SOFTDEVICE_VERSION                               2002
#define AF_SOFTDEVICE_VERSION_SZ                               8
#define AF_SOFTDEVICE_VERSION_TYPE         ATTRIBUTE_TYPE_SINT64

// Attribute Application Version
#define AF_APPLICATION_VERSION                              2003
#define AF_APPLICATION_VERSION_SZ                              8
#define AF_APPLICATION_VERSION_TYPE        ATTRIBUTE_TYPE_SINT64

// Attribute Profile Version
#define AF_PROFILE_VERSION                                  2004
#define AF_PROFILE_VERSION_SZ                                  8
#define AF_PROFILE_VERSION_TYPE            ATTRIBUTE_TYPE_SINT64

// Attribute Security Enabled
#define AF_SECURITY_ENABLED                                60000
#define AF_SECURITY_ENABLED_SZ                                 1
#define AF_SECURITY_ENABLED_TYPE          ATTRIBUTE_TYPE_BOOLEAN

// Attribute Attribute ACK
#define AF_ATTRIBUTE_ACK                                   65018
#define AF_ATTRIBUTE_ACK_SZ                                    2
#define AF_ATTRIBUTE_ACK_TYPE              ATTRIBUTE_TYPE_SINT16

// Attribute Reboot Reason
#define AF_REBOOT_REASON                                   65019
#define AF_REBOOT_REASON_SZ                                  100
#define AF_REBOOT_REASON_TYPE               ATTRIBUTE_TYPE_UTF8S

// Attribute BLE Comms
#define AF_BLE_COMMS                                       65020
#define AF_BLE_COMMS_SZ                                       12
#define AF_BLE_COMMS_TYPE                   ATTRIBUTE_TYPE_BYTES

// Attribute SPI Enabled
#define AF_SPI_ENABLED                                     65021
#define AF_SPI_ENABLED_SZ                                      1
#define AF_SPI_ENABLED_TYPE               ATTRIBUTE_TYPE_BOOLEAN
//endregion
