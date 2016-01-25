/**
 * Copyright 2015 Afero, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "afLib.h"
#include "afErrors.h"
#include "msg_types.h"

#include "gpiolib.h"

typedef uint8_t byte;
static const char *device = "/dev/spidev0.0";
//static uint8_t mode=SPI_CS_HIGH|SPI_LSB_FIRST;
//static uint8_t mode=SPI_CS_HIGH;
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 1000000;
//static uint32_t speed = 500000;
static uint16_t delay_amount =0 ;
static int fd = -1;
#define IS_MCU_ATTR(x) (x >= 0 && x < 1024)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static int gpio_cs_fd;

static iafLib *_iaflib = NULL;

static void pabort(const char *s)
{
	perror(s);
	abort();
}
void noInterrupts()
{
}

void interrupts()
{
}

#if 0
static void transfer(int junk)
{
        int ret;
        uint8_t tx[] = {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD,
                0xF0, 0x0D,
        };
        uint8_t rx[ARRAY_SIZE(tx)] = {0, };
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = ARRAY_SIZE(tx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,
        };

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("can't send spi message");

        for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
                if (!(ret % 6))
                        puts("");
                printf("%.2X ", rx[ret]);
        }
        puts("");
}
#endif

// Reverse table is used because we don't know how to change bit-order on SPI settings
uint8_t BitReverseTable256[] = {0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
                      0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
                      0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
                      0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
                      0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
                      0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
                      0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
                      0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
                      0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
                      0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
                      0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
                      0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
                      0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
                      0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
                      0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
                      0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF};

static uint8_t rbo(uint8_t in)
{
 return BitReverseTable256[in&0xff];
//   return in;
}


static void transfer2(int junk)
{
        int ret;
        int i;
        uint8_t tx[] = {
#if 1
0x30, 0x00, 0x00, 0x00, 0x00, 0x30,
#else
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD,
                0xF0, 0x0D,
#endif
        };

        for (i=0;i<ARRAY_SIZE(tx);i++) tx[i]=rbo(tx[i]);

        uint8_t rx[ARRAY_SIZE(tx)] = {0, };
        struct spi_ioc_transfer tr;
                tr.tx_buf = (unsigned long)tx;
                tr.rx_buf = (unsigned long)rx;
                tr.len = ARRAY_SIZE(tx);
                tr.delay_usecs = delay_amount;
                tr.speed_hz = speed;
                tr.bits_per_word = bits;
                tr.cs_change= 0;
                tr.pad= 0;
#if 0
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = ARRAY_SIZE(tx),
                .delay_usecs = delay,
                .speed_hz = speed,
                .bits_per_word = bits,

        };
#endif

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("can't send spi message");

        for (i=0;i<ARRAY_SIZE(rx);i++) rx[i]=rbo(rx[i]);

        for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
                if (!(ret % 6))
                        puts("");
                printf("%.2X ", rx[ret]);
        }
        puts("");
}

static void transfer_array(char *bytes,int len)
{
        int ret;
        int i;
        uint8_t *tx = (uint8_t *)bytes;

        for (i=0;i<ARRAY_SIZE(tx);i++) tx[i]=rbo(tx[i]);

        uint8_t rx[ARRAY_SIZE(tx)] = {0, };
        struct spi_ioc_transfer tr;
                tr.tx_buf = (unsigned long)tx;
                tr.rx_buf = (unsigned long)rx;
                tr.len = len;
                tr.delay_usecs = delay_amount;
                tr.speed_hz = speed;
                tr.bits_per_word = bits;
                tr.cs_change= 0;
                tr.pad= 0;

        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("can't send spi message");

        for (i=0;i<len;i++) rx[i]=rbo(rx[i]);

        for (ret = 0; ret < len; ret++) {
                if (!(ret % 6))
                        puts("");
                printf("%.2X ", rx[ret]);
        }
        puts("");
fprintf(stdout,"delay_time = %d len = %d\n",delay_amount,tr.len);

        memcpy(bytes,rx,len);
}





static uint8_t transfer(uint8_t data)
{
#if 1
        int ret;
        uint8_t tx[2];
         printf("transfer(%.2X)\n", data);
        data = rbo(data);
         printf("after rbo transfer(%.2X)\n", data);
        tx[0]=data;
        tx[1]=0;
        uint8_t rx[2] = {0, };
        struct spi_ioc_transfer tr;
                tr.tx_buf = (unsigned long)tx;
                tr.rx_buf = (unsigned long)rx;
                tr.len = 1;
                tr.delay_usecs = delay_amount;
                tr.speed_hz = speed;
                tr.bits_per_word = bits;
                tr.cs_change= 0;
                //tr.tx_nbits= 0;
                //tr.rx_nbits= 0;
                tr.pad= 0;
/*
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = 1,
                .delay_usecs = delay_amount,
                .speed_hz = speed,
                .bits_per_word = bits,
        };
*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
        {
           printf("%.2X ", tx[0]);
                pabort("can't send spi message");
        }
         printf("from afero: %.2X \n", rx[0]);
         printf("from afero reverse: %.2X \n", rbo(rx[0]));

        return rbo(rx[0]);
#endif
}
#if 0
static uint8_t transfer(uint8_t *buffer,int size)
{
        int ret;
        uint8_t *tx = buffer;
        uint8_t rx[size] = {0, };
        struct spi_ioc_transfer tr;
                tr.tx_buf = (unsigned long)tx;
                tr.rx_buf = (unsigned long)rx;
                tr.len = size;
                tr.delay_usecs = delay_amount;
                tr.speed_hz = speed;
                tr.bits_per_word = bits;
/*
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = size,
                .delay_usecs = delay_amount,
                .speed_hz = speed,
                .bits_per_word = bits,
        };
*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
        if (ret < 1)
                pabort("can't send spi message");

        for (ret = 0; ret < size; ret++) {
                if (!(ret % 6))
                        puts("");
                printf("%.2X ", rx[ret]);
        }
        puts("");

        memcpy(buffer,rx,size);

        return rx[0];
}
#endif

iafLib *iafLib::create(const int chipSelect, const int mcuInterrupt, isr isrWrapper,
                       onAttributeSet attrSet, onAttributeSetComplete attrSetComplete) {
    if (_iaflib == NULL) {
        _iaflib = new afLib(chipSelect, mcuInterrupt, isrWrapper, attrSet, attrSetComplete);
    }

    return _iaflib;
}

afLib::afLib(const int chipSelect, const int mcuInterrupt, isr isrWrapper,
             onAttributeSet attrSet, onAttributeSetComplete attrSetComplete) {
	int ret = 0;
    queueInit();
    _request.p_value = NULL;

    
    //_spiSettings = SPISettings(1000000, LSBFIRST, SPI_MODE0);
	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
    _interrupts_pending = 0;
    _state = STATE_IDLE;

    _writeCmd = NULL;
    _writeCmdOffset = 0;

    _readCmd = NULL;
    _readCmdOffset = 0;
    _readBufferLen = 0;

    _txStatus = new StatusCommand();
    _rxStatus = new StatusCommand();

    _chipSelect = chipSelect;
    _onAttrSet = attrSet;
    _onAttrSetComplete = attrSetComplete;

    // Chip Select should be setup for us already


        unsigned int gpio = 0;
        int len;

        //gpio_export(chipSelect);
        //gpio_set_dir(chipSelect, 1);
//        gpio_set_edge(chipSelect, "falling");
        //gpio_cs_fd = gpio_fd_open(chipSelect);


//wiringPiSetup();
//pinMode(0, INPUT);
//pinMode(_chipSelect, OUTPUT);

#if 0
    gpio_set_value(_chipSelect,HIGH);
#endif

#if 0
    pinMode(chipSelect, OUTPUT);
    digitalWrite(chipSelect, HIGH);
    SPI.begin();

    pinMode(mcuInterrupt, INPUT);
    attachInterrupt(mcuInterrupt, isrWrapper, FALLING);
#endif
    // we need to setup the interrupt pin as a GPIO

}

void afLib::loop(void) {
    if (isIdle() && (queueGet(&_request.messageType, &_request.requestId, &_request.attrId, &_request.valueLen,
                              &_request.p_value) == afSUCCESS)) {
        switch (_request.messageType) {
            case MSG_TYPE_GET:
                doGetAttribute(_request.requestId, _request.attrId);
                break;

            case MSG_TYPE_SET:
                doSetAttribute(_request.requestId, _request.attrId, _request.valueLen, _request.p_value);
                break;

            case MSG_TYPE_UPDATE:
                doUpdateAttribute(_request.requestId, _request.attrId, 0, _request.valueLen, _request.p_value);
                break;

            default:
                fprintf(stdout,"loop: request type!\n");
        }
    }

    if (_request.p_value != NULL) {
        delete (_request.p_value);
        _request.p_value = NULL;
    }
    checkInterrupt();
}

void afLib::updateIntsPending(int amount) {
    noInterrupts();
    _interrupts_pending += amount;
    interrupts();
}

void afLib::sendCommand(void) {
    noInterrupts();
    if (_interrupts_pending == 0 && _state == STATE_IDLE) {
        _interrupts_pending++;
    }
    interrupts();
}

int afLib::getAttribute(const uint16_t attrId) {
    _requestId++;
    uint8_t dummy; // This value isn't actually used.
    return queuePut(MSG_TYPE_GET, _requestId++, attrId, 0, &dummy);
}

/**
 * The many moods of setAttribute
 */
int afLib::setAttribute(const uint16_t attrId, const bool value) {
    _requestId++;
    return queuePut(IS_MCU_ATTR(attrId) ? MSG_TYPE_UPDATE : MSG_TYPE_SET, _requestId, attrId, sizeof(value),
                    (uint8_t *)&value);
}

int afLib::setAttribute(const uint16_t attrId, const int8_t value) {
    _requestId++;
    return queuePut(IS_MCU_ATTR(attrId) ? MSG_TYPE_UPDATE : MSG_TYPE_SET, _requestId, attrId, sizeof(value),
                    (uint8_t *)&value);
}

int afLib::setAttribute(const uint16_t attrId, const int16_t value) {
    _requestId++;
    return queuePut(IS_MCU_ATTR(attrId) ? MSG_TYPE_UPDATE : MSG_TYPE_SET, _requestId, attrId, sizeof(value),
                    (uint8_t *) &value);
}

int afLib::setAttribute(const uint16_t attrId, const int32_t value) {
    _requestId++;
    return queuePut(IS_MCU_ATTR(attrId) ? MSG_TYPE_UPDATE : MSG_TYPE_SET, _requestId, attrId, sizeof(value),
                    (uint8_t *) &value);
}

int afLib::setAttribute(const uint16_t attrId, const int64_t value) {
    _requestId++;
    return queuePut(IS_MCU_ATTR(attrId) ? MSG_TYPE_UPDATE : MSG_TYPE_SET, _requestId, attrId, sizeof(value),
                    (uint8_t *) &value);
}

int afLib::setAttribute(const uint16_t attrId, const std::string &value) {
    _requestId++;
    return queuePut(IS_MCU_ATTR(attrId) ? MSG_TYPE_UPDATE : MSG_TYPE_SET, _requestId, attrId, value.length(),
                    (uint8_t *) value.c_str());
}

int afLib::setAttribute(const uint16_t attrId, const uint16_t valueLen, const char *value) {
    if (valueLen > MAX_ATTRIBUTE_SIZE) {
        return afERROR_INVALID_PARAM;
    }

    if (value == NULL) {
        return afERROR_INVALID_PARAM;
    }

    _requestId++;
    return queuePut(IS_MCU_ATTR(attrId) ? MSG_TYPE_UPDATE : MSG_TYPE_SET, _requestId, attrId, valueLen,
                    (const uint8_t *) value);
}

int afLib::setAttribute(const uint16_t attrId, const uint16_t valueLen, const uint8_t *value) {
    if (valueLen > MAX_ATTRIBUTE_SIZE) {
        return afERROR_INVALID_PARAM;
    }

    if (value == NULL) {
        return afERROR_INVALID_PARAM;
    }

    _requestId++;
    return queuePut(IS_MCU_ATTR(attrId) ? MSG_TYPE_UPDATE : MSG_TYPE_SET, _requestId, attrId, valueLen, value);
}

int afLib::setAttributeComplete(uint8_t requestId, const uint16_t attrId, const uint16_t valueLen, const uint8_t *value) {
    if (valueLen > MAX_ATTRIBUTE_SIZE) {
        return afERROR_INVALID_PARAM;
    }

    if (value == NULL) {
        return afERROR_INVALID_PARAM;
    }

    return queuePut(MSG_TYPE_UPDATE, requestId, attrId, valueLen, value);
}

int afLib::doGetAttribute(uint8_t requestId, uint16_t attrId) {
    if (_interrupts_pending > 0 || _writeCmd != NULL) {
        return afERROR_BUSY;
    }

    _writeCmd = new Command(requestId, MSG_TYPE_GET, attrId);
    if (!_writeCmd->isValid()) {
        fprintf(stdout,"getAttribute invalid command:");
        //Serial.print("getAttribute invalid command:");
        _writeCmd->dumpBytes();
        _writeCmd->dump();
        delete (_writeCmd);
        _writeCmd = NULL;
        return afERROR_INVALID_COMMAND;
    }

    // Start the transmission.
    sendCommand();

    return afSUCCESS;
}

int afLib::doSetAttribute(uint8_t requestId, uint16_t attrId, uint16_t valueLen, uint8_t *value) {
    if (_interrupts_pending > 0 || _writeCmd != NULL) {
        return afERROR_BUSY;
    }

    _writeCmd = new Command(requestId, MSG_TYPE_SET, attrId, valueLen, value);
    if (!_writeCmd->isValid()) {
        fprintf(stdout,"setAttributeComplete invalid command:");
        //Serial.print("setAttributeComplete invalid command:");
        _writeCmd->dumpBytes();
        _writeCmd->dump();
        delete (_writeCmd);
        _writeCmd = NULL;
        return afERROR_INVALID_COMMAND;
    }

    // Start the transmission.
    sendCommand();

    return afSUCCESS;
}

int afLib::doUpdateAttribute(uint8_t requestId, uint16_t attrId, uint8_t status, uint16_t valueLen, uint8_t *value) {
    if (_interrupts_pending > 0 || _writeCmd != NULL) {
        return afERROR_BUSY;
    }

    _writeCmd = new Command(requestId, MSG_TYPE_UPDATE, attrId, status, 3 /* MCU Set it */, valueLen, value);
    if (!_writeCmd->isValid()) {
        fprintf(stdout,"updateAttribute invalid command:");
        _writeCmd->dumpBytes();
        _writeCmd->dump();
        delete (_writeCmd);
        return afERROR_INVALID_COMMAND;
    }

    // Start the transmission.
    sendCommand();

    return afSUCCESS;
}

int afLib::parseCommand(const char *cmd) {
    if (_interrupts_pending > 0 || _writeCmd != NULL) {
        fprintf(stdout,"Busy: ");
        fprintf(stdout,"%d",_interrupts_pending);
        fprintf(stdout,", ");
        fprintf(stdout,"%d\n",(_writeCmd != NULL));
        return afERROR_BUSY;
    }

    int reqId = _requestId++;
    _writeCmd = new Command(reqId, cmd);
    if (!_writeCmd->isValid()) {
        fprintf(stdout,"BAD: ");
        fprintf(stdout,"%s\n",cmd);
        _writeCmd->dumpBytes();
        _writeCmd->dump();
        delete (_writeCmd);
        _writeCmd = NULL;
        return afERROR_INVALID_COMMAND;
    }

    // Start the transmission.
    sendCommand();

    return afSUCCESS;
}

void afLib::checkInterrupt(void) {
    int result;

    if (_interrupts_pending > 0) {
        //Serial.print("_interrupts_pending: "); Serial.println(_interrupts_pending);

        switch (_state) {
            case STATE_IDLE:
                if (_writeCmd != NULL) {
                    // Include 2 bytes for length
                    _bytesToSend = _writeCmd->getSize() + 2;
                } else {
                    _bytesToSend = 0;
                }
                _state = STATE_STATUS_SYNC;
                printState(_state);
                return;

            case STATE_STATUS_SYNC:
                _txStatus->setAck(false);
                _txStatus->setBytesToSend(_bytesToSend);
                _txStatus->setBytesToRecv(0);

                result = exchangeStatus(_txStatus, _rxStatus);

                if (result == 0 && _rxStatus->isValid() && inSync(_txStatus, _rxStatus)) {
                    _state = STATE_STATUS_ACK;
                    if (_txStatus->getBytesToSend() == 0 && _rxStatus->getBytesToRecv() > 0) {
                        _bytesToRecv = _rxStatus->getBytesToRecv();
                    }
                } else {
                    // Try resending the preamble
                    _state = STATE_STATUS_SYNC;
                    fprintf(stdout,"Collision\n");//Serial.println("Collision");
          _txStatus->dumpBytes();
          _rxStatus->dumpBytes();
                }
                printState(_state);
                break;

            case STATE_STATUS_ACK:
                _txStatus->setAck(true);
                _txStatus->setBytesToRecv(_rxStatus->getBytesToRecv());
                _bytesToRecv = _rxStatus->getBytesToRecv();
                result = writeStatus(_txStatus);
                if (result != 0) {
                    _state = STATE_STATUS_SYNC;
                    printState(_state);
                    break;
                }
                if (_bytesToSend > 0) {
                    _writeBufferLen = (uint16_t) _writeCmd->getSize();
                    _writeBuffer = new uint8_t[_bytesToSend];
                    memcpy(_writeBuffer, (uint8_t * ) & _writeBufferLen, 2);
                    _writeCmd->getBytes(&_writeBuffer[2]);
                    _state = STATE_SEND_BYTES;
                } else if (_bytesToRecv > 0) {
                    _state = STATE_RECV_BYTES;
                } else {
                    _state = STATE_CMD_COMPLETE;
                }
                printState(_state);
                break;

            case STATE_SEND_BYTES:
//        Serial.print("send bytes: "); Serial.println(_bytesToSend);		
                sendBytes();

                if (_bytesToSend == 0) {
                    _writeBufferLen = 0;
                    delete (_writeBuffer);
                    _writeBuffer = NULL;
                    _state = STATE_CMD_COMPLETE;
                    printState(_state);
                }
                break;

            case STATE_RECV_BYTES:
//        Serial.print("receive bytes: "); Serial.println(_bytesToRecv);
                recvBytes();
                if (_bytesToRecv == 0) {
                    _state = STATE_CMD_COMPLETE;
                    printState(_state);
                    _readCmd = new Command(_readBufferLen, &_readBuffer[2]);
                    delete (_readBuffer);
                    _readBuffer = NULL;
                }
                break;

            case STATE_CMD_COMPLETE:
                _state = STATE_IDLE;
                printState(_state);
                if (_readCmd != NULL) {
                    byte val;
                    _readCmd->getValue(&val);

                    switch (_readCmd->getCommand()) {
                        case MSG_TYPE_SET:
                            _onAttrSet(_readCmd->getReqId(), _readCmd->getAttrId(), _readCmd->getValueLen(), &val);
                            break;

                        case MSG_TYPE_UPDATE:
                            _onAttrSetComplete(_readCmd->getReqId(), _readCmd->getAttrId(), _readCmd->getValueLen(), &val);
                            break;

                        default:
                            break;
                    }
                    delete (_readCmd);
                    _readCmdOffset = 0;
                    _readCmd = NULL;
                }

                if (_writeCmd != NULL) {
                    delete (_writeCmd);
                    _writeCmdOffset = 0;
                    _writeCmd = NULL;
                }
                break;
        }

        updateIntsPending(-1);
    }
}

void afLib::beginSPI() {
    //SPI.beginTransaction(_spiSettings);
    //digitalWrite(_chipSelect, LOW);
    //digitalWrite(2, LOW);
    //gpio_set_value(_chipSelect,LOW);

    //delayMicroseconds(8);
}

void afLib::endSPI() {
    //delayMicroseconds(1);
    //digitalWrite(_chipSelect, HIGH);
    //SPI.endTransaction();
    //gpio_set_value(_chipSelect,HIGH);
}

int afLib::exchangeStatus(StatusCommand *tx, StatusCommand *rx) {
    int result = 0;
    uint16_t len = tx->getSize();
    int bytes[len];
    char rbytes[len+1];
    int index = 0;
    tx->getBytes(bytes);

    beginSPI();


fprintf(stdout,"exhangeStatus len = %d index %d bytes[index]==%x bytes[%d]==%d\n",len,index,bytes[index],index+1,bytes[index+1]);

    for (int i=0;i<len;i++)
    {
      rbytes[i]=bytes[i];
    }
    rbytes[len]=tx->getChecksum();
    transfer_array(rbytes,len+1);

    byte cmd = bytes[index++];
    if (cmd != 0x30 && cmd != 0x31) {
        fprintf(stdout,"exchangeStatus bad cmd: ");
//Serial.print("exchangeStatus bad cmd: ");
        fprintf(stdout,"%x\n",cmd);
        //Serial.println(cmd, HEX);
        result = -afERROR_INVALID_COMMAND;
    }

    rx->setBytesToSend(rbytes[index + 0] | (rbytes[index + 1] << 8));
    rx->setBytesToRecv(rbytes[index + 2] | (rbytes[index + 3] << 8));
    rx->setChecksum(rbytes[index+4]);

#if 0
    //byte cmd = SPI.transfer(bytes[index++]);
    byte cmd = transfer(bytes[index++]);
    if (cmd != 0x30 && cmd != 0x31) {
        fprintf(stdout,"exchangeStatus bad cmd: ");
//Serial.print("exchangeStatus bad cmd: ");
        fprintf(stdout,"%x\n",cmd);
        //Serial.println(cmd, HEX);
        result = -afERROR_INVALID_COMMAND;
    }

    //rx->setBytesToSend(SPI.transfer(bytes[index + 0]) | (SPI.transfer(bytes[index + 1]) << 8));
    //rx->setBytesToRecv(SPI.transfer(bytes[index + 2]) | (SPI.transfer(bytes[index + 3]) << 8));
    //rx->setChecksum(SPI.transfer(tx->getChecksum()));
fprintf(stdout,"first byte is correct\n");
    rx->setBytesToSend(transfer(bytes[index + 0]) | (transfer(bytes[index + 1]) << 8));
    rx->setBytesToRecv(transfer(bytes[index + 2]) | (transfer(bytes[index + 3]) << 8));
    rx->setChecksum(transfer(tx->getChecksum()));
#endif

    endSPI();

fprintf(stdout,"result: %d\n",result);

    return result;
}

bool afLib::inSync(StatusCommand *tx, StatusCommand *rx) {
    return (tx->getBytesToSend() == 0 && rx->getBytesToRecv() == 0) ||
           (tx->getBytesToSend() > 0 && rx->getBytesToRecv() == 0) ||
           (tx->getBytesToSend() == 0 && rx->getBytesToRecv() > 0);
}

int afLib::writeStatus(StatusCommand *c) {
    int result = 0;
    uint16_t len = c->getSize();
    int bytes[len];
    char rbytes[len+1];
    int index = 0;
    c->getBytes(bytes);

    beginSPI();

    for (int i=0;i<len;i++)
    {
      rbytes[i]=bytes[i];
    }
    rbytes[len]=c->getChecksum();
    transfer_array(rbytes,len+1);

    byte cmd = rbytes[index++];

    //byte cmd = transfer(bytes[index++]);
    //byte cmd = SPI.transfer(bytes[index++]);
    if (cmd != 0x30 && cmd != 0x31) {
        fprintf(stdout,"writeStatus bad cmd: ");
        fprintf(stdout,"%x\n",cmd);
        //Serial.print("writeStatus bad cmd: ");
        //Serial.println(cmd, HEX);
        result = afERROR_INVALID_COMMAND;
    }

#if 0
    for (int i = 1; i < len; i++) {
        //SPI.transfer(bytes[i]);
        transfer(bytes[i]);
    }
    //SPI.transfer(c->getChecksum());
    transfer(c->getChecksum());
#endif

    endSPI();

  c->dump();
  c->dumpBytes();

    return result;
}

void afLib::sendBytes() {
    uint16_t len = _bytesToSend > SPI_FRAME_LEN ? SPI_FRAME_LEN : _bytesToSend;
    uint8_t bytes[SPI_FRAME_LEN];
    memset(bytes, 0xff, sizeof(bytes));

    memcpy(bytes, &_writeBuffer[_writeCmdOffset], len);

    beginSPI();

fprintf(stdout,"sendBytes called len %d\n",len);
    transfer_array((char *)bytes,len);
/*
    for (int i = 0; i < len; i++) {
        //SPI.transfer(bytes[i]);
        transfer(bytes[i]);
    }
*/
    endSPI();

  dumpBytes("Sending:", len, bytes);

    _writeCmdOffset += len;
    _bytesToSend -= len;
}

void afLib::recvBytes() {
    uint16_t len = _bytesToRecv > SPI_FRAME_LEN ? SPI_FRAME_LEN : _bytesToRecv;

    if (_readCmdOffset == 0) {
        _readBufferLen = _bytesToRecv;
        _readBuffer = new uint8_t[_readBufferLen];
    }

    beginSPI();

fprintf(stdout,"recvBytes\n");
    char * start =(char*)_readBuffer + _readCmdOffset;
    transfer_array(start,len);
#if 0
    for (int i = 0; i < len; i++) {
        //_readBuffer[i + _readCmdOffset] = SPI.transfer(0);
        _readBuffer[i + _readCmdOffset] = transfer(0);
    }
#endif

    endSPI();

  dumpBytes("Receiving:", len, _readBuffer);

    _readCmdOffset += len;
    _bytesToRecv -= len;
}

void afLib::onIdle() {
}

bool afLib::isIdle() {
    return _interrupts_pending == 0 && _state == STATE_IDLE;
}

void afLib::dumpBytes(char *label, int len, uint8_t *bytes) {
    fprintf(stdout,"%s\n",label);
    //Serial.println(label);
    for (int i = 0; i < len; i++) {
        if (i > 0) {
            //Serial.print(", ");
            fprintf(stdout,", ");
        }
        uint8_t b = bytes[i] & 0xff;

        if (b < 0x10) {
            //Serial.print("0x0");
            //Serial.print(b, HEX);
            fprintf(stdout,"0x0");
            fprintf(stdout,"%x",b);
        } else {
            //Serial.print("0x");
            //Serial.print(b, HEX);
            fprintf(stdout,"0x");
            fprintf(stdout,"%x",b);
        }
    }
    //Serial.println("");
    fprintf(stdout,"\n");
}

void afLib::mcuISR() {
//  Serial.println("mcu");
    _interrupts_pending++;
}

void afLib::printState(int state) {
    //return;
    switch (state) {
        case STATE_IDLE:
            //Serial.println("STATE_IDLE");
            fprintf(stdout,"STATE_IDLE\n");
            break;
        case STATE_STATUS_SYNC:
            fprintf(stdout,"STATE_STATUS_SYNC\n");
            break;
        case STATE_STATUS_ACK:
            fprintf(stdout,"STATE_STATUS_ACK\n");
            break;
        case STATE_SEND_BYTES:
            fprintf(stdout,"STATE_SEND_BYTES\n");
            break;
        case STATE_RECV_BYTES:
            fprintf(stdout,"STATE_RECV_BYTES\n");
            break;
        case STATE_CMD_COMPLETE:
            fprintf(stdout,"STATE_CMD_COMPLETE\n");
            break;
        default:
            fprintf(stdout,"Unknown State!\n");
            break;
    }
}

void afLib::queueInit() {
    for (int i = 0; i < REQUEST_QUEUE_SIZE; i++) {
        _requestQueue[i].p_value = NULL;
    }
}

int afLib::queuePut(uint8_t messageType, uint8_t requestId, const uint16_t attributeId, uint16_t valueLen,
                    const uint8_t *value) {
    for (int i = 0; i < REQUEST_QUEUE_SIZE; i++) {
        if (_requestQueue[i].p_value == NULL) {
            _requestQueue[i].messageType = messageType;
            _requestQueue[i].attrId = attributeId;
            _requestQueue[i].requestId = requestId;
            _requestQueue[i].valueLen = valueLen;
            _requestQueue[i].p_value = new uint8_t[valueLen];
            memcpy(_requestQueue[i].p_value, value, valueLen);
            return afSUCCESS;
        }
    }

    return afERROR_QUEUE_OVERFLOW;
}

int afLib::queueGet(uint8_t *messageType, uint8_t *requestId, uint16_t *attributeId, uint16_t *valueLen,
                    uint8_t **value) {
    for (int i = 0; i < REQUEST_QUEUE_SIZE; i++) {
        if (_requestQueue[i].p_value != NULL) {
            *messageType = _requestQueue[i].messageType;
            *attributeId = _requestQueue[i].attrId;
            *requestId = _requestQueue[i].requestId;
            *valueLen = _requestQueue[i].valueLen;
            *value = new uint8_t[*valueLen];
            memcpy(*value, _requestQueue[i].p_value, *valueLen);
            delete (_requestQueue[i].p_value);
            _requestQueue[i].p_value = NULL;
            return afSUCCESS;
        }
    }

    return afERROR_QUEUE_UNDERFLOW;
}
