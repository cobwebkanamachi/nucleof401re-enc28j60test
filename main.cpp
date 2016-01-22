#include "mbed.h"
#include <UIPEthernet.h>
#include <UIPServer.h>
#include <UIPClient.h>

#define DHCP       // comment out this line if you'd like to use static IP address
// Microchip ENC28J60 Ethernet Interface Driver
// Author: Guido Socher
// Copyright: GPL V2
// 
// Based on the enc28j60.c file from the AVRlib library by Pascal Stang.
// For AVRlib See http://www.procyonengineering.com/
// Used with explicit permission of Pascal Stang.
//
// 2010-05-20 <jc@wippler.nl>
// 2013-09-20 Modified for mbed by Marcelo Barros de Almeida <marcelobarrosalmeida@gmail.com>
//            http://shiningbits.com/blog/
//            http://br.linkedin.com/in/marcelobarrosalmeida/


// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
// - Register address        (bits 0-4)
// - Bank number        (bits 5-6)
// - MAC/PHY indicator        (bit 7)
#define ADDR_MASK        0x1F
#define BANK_MASK        0x60
#define SPRD_MASK        0x80
// All-bank registers
#define EIE              0x1B
#define EIR              0x1C
#define ESTAT            0x1D
#define ECON2            0x1E
#define ECON1            0x1F
// Bank 0 registers
#define ERDPT           (0x00|0x00)
#define EWRPT           (0x02|0x00)
#define ETXST           (0x04|0x00)
#define ETXND           (0x06|0x00)
#define ERXST           (0x08|0x00)
#define ERXND           (0x0A|0x00)
#define ERXRDPT         (0x0C|0x00)
// #define ERXWRPT         (0x0E|0x00)
#define EDMAST          (0x10|0x00)
#define EDMAND          (0x12|0x00)
// #define EDMADST         (0x14|0x00)
#define EDMACS          (0x16|0x00)
// Bank 1 registers
#define EHT0             (0x00|0x20)
#define EHT1             (0x01|0x20)
#define EHT2             (0x02|0x20)
#define EHT3             (0x03|0x20)
#define EHT4             (0x04|0x20)
#define EHT5             (0x05|0x20)
#define EHT6             (0x06|0x20)
#define EHT7             (0x07|0x20)
#define EPMM0            (0x08|0x20)
#define EPMM1            (0x09|0x20)
#define EPMM2            (0x0A|0x20)
#define EPMM3            (0x0B|0x20)
#define EPMM4            (0x0C|0x20)
#define EPMM5            (0x0D|0x20)
#define EPMM6            (0x0E|0x20)
#define EPMM7            (0x0F|0x20)
#define EPMCS           (0x10|0x20)
// #define EPMO            (0x14|0x20)
#define EWOLIE           (0x16|0x20)
#define EWOLIR           (0x17|0x20)
#define ERXFCON          (0x18|0x20)
#define EPKTCNT          (0x19|0x20)
// Bank 2 registers
#define MACON1           (0x00|0x40|0x80)
#define MACON2           (0x01|0x40|0x80)
#define MACON3           (0x02|0x40|0x80)
#define MACON4           (0x03|0x40|0x80)
#define MABBIPG          (0x04|0x40|0x80)
#define MAIPG           (0x06|0x40|0x80)
#define MACLCON1         (0x08|0x40|0x80)
#define MACLCON2         (0x09|0x40|0x80)
#define MAMXFL          (0x0A|0x40|0x80)
#define MAPHSUP          (0x0D|0x40|0x80)
#define MICON            (0x11|0x40|0x80)
#define MICMD            (0x12|0x40|0x80)
#define MIREGADR         (0x14|0x40|0x80)
#define MIWR            (0x16|0x40|0x80)
#define MIRD            (0x18|0x40|0x80)
// Bank 3 registers
#define MAADR1           (0x00|0x60|0x80)
#define MAADR0           (0x01|0x60|0x80)
#define MAADR3           (0x02|0x60|0x80)
#define MAADR2           (0x03|0x60|0x80)
#define MAADR5           (0x04|0x60|0x80)
#define MAADR4           (0x05|0x60|0x80)
#define EBSTSD           (0x06|0x60)
#define EBSTCON          (0x07|0x60)
#define EBSTCS          (0x08|0x60)
#define MISTAT           (0x0A|0x60|0x80)
#define EREVID           (0x12|0x60)
#define ECOCON           (0x15|0x60)
#define EFLOCON          (0x17|0x60)
#define EPAUS           (0x18|0x60)

// ENC28J60 ERXFCON Register Bit Definitions
#define ERXFCON_UCEN     0x80
#define ERXFCON_ANDOR    0x40
#define ERXFCON_CRCEN    0x20
#define ERXFCON_PMEN     0x10
#define ERXFCON_MPEN     0x08
#define ERXFCON_HTEN     0x04
#define ERXFCON_MCEN     0x02
#define ERXFCON_BCEN     0x01
// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE        0x80
#define EIE_PKTIE        0x40
#define EIE_DMAIE        0x20
#define EIE_LINKIE       0x10
#define EIE_TXIE         0x08
#define EIE_WOLIE        0x04
#define EIE_TXERIE       0x02
#define EIE_RXERIE       0x01
// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF        0x40
#define EIR_DMAIF        0x20
#define EIR_LINKIF       0x10
#define EIR_TXIF         0x08
#define EIR_WOLIF        0x04
#define EIR_TXERIF       0x02
#define EIR_RXERIF       0x01
// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT        0x80
#define ESTAT_LATECOL    0x10
#define ESTAT_RXBUSY     0x04
#define ESTAT_TXABRT     0x02
#define ESTAT_CLKRDY     0x01
// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC    0x80
#define ECON2_PKTDEC     0x40
#define ECON2_PWRSV      0x20
#define ECON2_VRPS       0x08
// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST      0x80
#define ECON1_RXRST      0x40
#define ECON1_DMAST      0x20
#define ECON1_CSUMEN     0x10
#define ECON1_TXRTS      0x08
#define ECON1_RXEN       0x04
#define ECON1_BSEL1      0x02
#define ECON1_BSEL0      0x01
// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK    0x10
#define MACON1_TXPAUS    0x08
#define MACON1_RXPAUS    0x04
#define MACON1_PASSALL   0x02
#define MACON1_MARXEN    0x01
// ENC28J60 MACON2 Register Bit Definitions
#define MACON2_MARST     0x80
#define MACON2_RNDRST    0x40
#define MACON2_MARXRST   0x08
#define MACON2_RFUNRST   0x04
#define MACON2_MATXRST   0x02
#define MACON2_TFUNRST   0x01
// ENC28J60 MACON3 Register Bit Definitions
#define MACON3_PADCFG2   0x80
#define MACON3_PADCFG1   0x40
#define MACON3_PADCFG0   0x20
#define MACON3_TXCRCEN   0x10
#define MACON3_PHDRLEN   0x08
#define MACON3_HFRMLEN   0x04
#define MACON3_FRMLNEN   0x02
#define MACON3_FULDPX    0x01
// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN    0x02
#define MICMD_MIIRD      0x01
// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID    0x04
#define MISTAT_SCAN      0x02
#define MISTAT_BUSY      0x01

// ENC28J60 EBSTCON Register Bit Definitions
#define EBSTCON_PSV2     0x80
#define EBSTCON_PSV1     0x40
#define EBSTCON_PSV0     0x20
#define EBSTCON_PSEL     0x10
#define EBSTCON_TMSEL1   0x08
#define EBSTCON_TMSEL0   0x04
#define EBSTCON_TME      0x02
#define EBSTCON_BISTST    0x01

// PHY registers
#define PHCON1           0x00
#define PHSTAT1          0x01
#define PHHID1           0x02
#define PHHID2           0x03
#define PHCON2           0x10
#define PHSTAT2          0x11
#define PHIE             0x12
#define PHIR             0x13
#define PHLCON           0x14

// ENC28J60 PHY PHCON1 Register Bit Definitions
#define PHCON1_PRST      0x8000
#define PHCON1_PLOOPBK   0x4000
#define PHCON1_PPWRSV    0x0800
#define PHCON1_PDPXMD    0x0100
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define PHSTAT1_PFDPX    0x1000
#define PHSTAT1_PHDPX    0x0800
#define PHSTAT1_LLSTAT   0x0004
#define PHSTAT1_JBSTAT   0x0002
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK   0x4000
#define PHCON2_TXDIS     0x2000
#define PHCON2_JABBER    0x0400
#define PHCON2_HDLDIS    0x0100

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN  0x08
#define PKTCTRL_PPADEN   0x04
#define PKTCTRL_PCRCEN   0x02
#define PKTCTRL_POVERRIDE 0x01

// SPI operation codes
#define ENC28J60_READ_CTRL_REG       0x00
#define ENC28J60_READ_BUF_MEM        0x3A
#define ENC28J60_WRITE_CTRL_REG      0x40
#define ENC28J60_WRITE_BUF_MEM       0x7A
#define ENC28J60_BIT_FIELD_SET       0x80
#define ENC28J60_BIT_FIELD_CLR       0xA0
#define ENC28J60_SOFT_RESET          0xFF

// The RXSTART_INIT must be zero. See Rev. B4 Silicon Errata point 5.
// Buffer boundaries applied to internal 8K ram
// the entire available packet buffer space is allocated

#define RXSTART_INIT        0x0000  // start of RX buffer, room for 2 packets
#define RXSTOP_INIT         0x0BFF  // end of RX buffer
                            
#define TXSTART_INIT        0x0C00  // start of TX buffer, room for 1 packet
#define TXSTOP_INIT         0x11FF  // end of TX buffer
                            
#define SCRATCH_START       0x1200  // start of scratch area
#define SCRATCH_LIMIT       0x2000  // past end of area, i.e. 3.5 Kb 
#define SCRATCH_PAGE_SHIFT  6       // addressing is in pages of 64 bytes
#define SCRATCH_PAGE_SIZE   (1 << SCRATCH_PAGE_SHIFT)

// max frame length which the conroller will accept:
// (note: maximum ethernet frame length would be 1518)
#define MAX_FRAMELEN      1500        

#define FULL_SPEED  1   // switch to full-speed SPI for bulk transfers

 
//SPI spi(PTD2,PTD3,PTD1); // mosi, miso, sclk
//DigitalOut cs(PTD0);
//DigitalIn ethInt(PTD4);
SPI spi(PA_7, PA_6, PA_5); // mosi, miso, sclk
//EthernetInterface eth(&spi, PB_6, PA_10); // spi, cs, reset
DigitalOut cs(PB_6);
Serial pc(USBTX, USBRX); // tx, rx

static uint8_t Enc28j60Bank;
static int gNextPacketPtr;
#define DHCP       // comment out this line if you'd like to use static IP address
//#define UIPETHERNET_DEBUG 1
UIPEthernetClass    UIPEthernet(PA_7, PA_6, PA_5, PB_6);        // mosi, miso, sck, cs

const uint8_t       macaddr[] = { 0x00, 0xFF, 0x7A, 0xA5, 0x06, 0xDD}; 
const IPAddress MY_IP(MYIP);    // change to your one.
const uint16_t  MY_PORT = 80;   // for HTTP connection
EthernetServer  myServer = EthernetServer(MY_PORT);
const IPAddress MY_DNS(MYDNS);
const IPAddress MY_GW(MYGW);
const IPAddress MY_NM(MYNETMASK);

static void enableChip(void) 
{
    cs = 0;
}

static void disableChip(void) 
{
    cs = 1;
}

static uint8_t readOp(uint8_t op, uint8_t address) 
{
    uint8_t result;
    
    //pc.printf("readOp %02X %02X\r\n",op,address);
    
    enableChip();
    
    spi.write(op | (address & ADDR_MASK));
    result = spi.write(0x00);
    if (address & 0x80)
        result = spi.write(0x00);
    
    disableChip();
    
    //pc.printf("readOp result %02X\r\n",result);
    
    return result;
}

static void writeOp(uint8_t op, uint8_t address, uint8_t data) 
{

    //pc.printf("writeOp %02X %02X %02X\r\n",op,address,data);
    
    enableChip();
    
    spi.write(op | (address & ADDR_MASK));
    spi.write(data);
    
    disableChip();
}

static void SetBank (uint8_t address) 
{
    if ((address & BANK_MASK) != Enc28j60Bank) 
    {
        writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1|ECON1_BSEL0);
        Enc28j60Bank = address & BANK_MASK;
        writeOp(ENC28J60_BIT_FIELD_SET, ECON1, Enc28j60Bank>>5);
    }
}

static uint8_t readRegByte (uint8_t address) 
{
    SetBank(address);
    return readOp(ENC28J60_READ_CTRL_REG, address);
}

static uint16_t readReg(uint8_t address) 
{
    return readRegByte(address) + (readRegByte(address+1) << 8);
}

static void writeRegByte (uint8_t address, uint8_t data) 
{
    SetBank(address);
    writeOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

static void writeReg(uint8_t address, uint16_t data) 
{
    writeRegByte(address, data);
    writeRegByte(address + 1, data >> 8);
}
void enableBroadcast () 
{
    writeRegByte(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN|ERXFCON_BCEN);
}

void disableBroadcast () 
{
    writeRegByte(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
}

void disableMulticast () 
{ // disable multicast filter , enable multicast reception
    writeRegByte(ERXFCON, ERXFCON_CRCEN);
}
static uint16_t readPhyByte(uint8_t address) 
{
    writeRegByte(MIREGADR, address);
    writeRegByte(MICMD, MICMD_MIIRD);
    while (readRegByte(MISTAT) & MISTAT_BUSY);
    writeRegByte(MICMD, 0x00);
    return readRegByte(MIRD+1);
}

static void writePhy(uint8_t address, uint16_t data) 
{
    writeRegByte(MIREGADR, address);
    writeReg(MIWR, data);
    while (readRegByte(MISTAT) & MISTAT_BUSY)
        ;
}

static void writeBuf(const uint8_t* data, uint16_t len) 
{
    enableChip();
    
    spi.write(ENC28J60_WRITE_BUF_MEM);
    while (len--)
        spi.write(*data++);
        
    disableChip();
}

static void readBuf(uint8_t* data, uint16_t len) 
{
    enableChip();
    
    spi.write(ENC28J60_READ_BUF_MEM);
    while (len--) {
        *data++ = spi.write(0x00);
    }
    
    disableChip();
}

uint8_t isLinkUp(void) 
{
    return (readPhyByte(PHSTAT2) >> 2) & 1;
}

void packetSend(uint8_t *buffer, uint16_t len) 
{
    pc.printf("packetSend %d bytes ...\r\n",len);
    while (readOp(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_TXRTS)
    {
        if (readRegByte(EIR) & EIR_TXERIF) 
        {
            writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
            writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
        }
    }
    writeReg(EWRPT, TXSTART_INIT);
    writeReg(ETXND, TXSTART_INIT+len);
    writeOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
    writeBuf(buffer,len);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}

uint16_t packetReceive(uint8_t *buffer, uint16_t bufferSize) 
{
    uint16_t len = 0;
    if (readRegByte(EPKTCNT) > 0) 
    {
        writeReg(ERDPT, gNextPacketPtr);

        struct {
            uint16_t nextPacket;
            uint16_t byteCount;
            uint16_t status;
        } header;
        
        readBuf((uint8_t*) &header, sizeof header);

        gNextPacketPtr  = header.nextPacket;
        len = header.byteCount - 4; //remove the CRC count
        if (len>bufferSize-1)
            len=bufferSize-1;
        if ((header.status & 0x80)==0)
            len = 0;
        else
            readBuf(buffer,len);
        buffer[len] = 0;
        if (gNextPacketPtr - 1 > RXSTOP_INIT)
            writeReg(ERXRDPT, RXSTOP_INIT);
        else
            writeReg(ERXRDPT, gNextPacketPtr - 1);
        writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    }
    return len;
}

// Contributed by Alex M. Based on code from: http://blog.derouineau.fr
//                  /2011/07/putting-enc28j60-ethernet-controler-in-sleep-mode/
void powerDown() 
{
    writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_RXEN);
    while(readRegByte(ESTAT) & ESTAT_RXBUSY);
    while(readRegByte(ECON1) & ECON1_TXRTS);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_VRPS);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PWRSV);
}

void powerUp() 
{
    writeOp(ENC28J60_BIT_FIELD_CLR, ECON2, ECON2_PWRSV);
    while(!readRegByte(ESTAT) & ESTAT_CLKRDY);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

static int initialize(void) 
{    
    uint8_t rev;
    writeOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    wait(2);
    
    while(!readOp(ENC28J60_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY);

    gNextPacketPtr = RXSTART_INIT;
    writeReg(ERXST, RXSTART_INIT);
    writeReg(ERXRDPT, RXSTART_INIT);
    writeReg(ERXND, RXSTOP_INIT);
    writeReg(ETXST, TXSTART_INIT);
    writeReg(ETXND, TXSTOP_INIT);
    enableBroadcast(); // change to add ERXFCON_BCEN recommended by epam
    writeReg(EPMM0, 0x303f);
    writeReg(EPMCS, 0xf7f9);
    writeRegByte(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    writeRegByte(MACON2, 0x00);
    writeOp(ENC28J60_BIT_FIELD_SET, MACON3,
                        MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
    writeReg(MAIPG, 0x0C12);
    writeRegByte(MABBIPG, 0x12);
    writeReg(MAMXFL, MAX_FRAMELEN);  
    writeRegByte(MAADR5, macaddr[0]);
    writeRegByte(MAADR4, macaddr[1]);
    writeRegByte(MAADR3, macaddr[2]);
    writeRegByte(MAADR2, macaddr[3]);
    writeRegByte(MAADR1, macaddr[4]);
    writeRegByte(MAADR0, macaddr[5]);
    writePhy(PHCON2, PHCON2_HDLDIS);
    SetBank(ECON1);
    writeOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

    rev = readRegByte(EREVID);
    // microchip forgot to step the number on the silcon when they
    // released the revision B7. 6 is now rev B7. We still have
    // to see what they do when they release B8. At the moment
    // there is no B8 out yet
    if (rev > 5) ++rev;
    return rev;
}

static void setupSPI(void)
{
    spi.format(8,0);
    spi.frequency(8000000);
}

int main(void)
{
    uint8_t rev;
    static uint8_t tx_frame[] = 
        { 0xff,  0xff, 0xff, 0xff, 0xff, 0xff,
          macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5], 
          0x80, 0x00,
          0x53, 0x68, 0x69, 0x6E, 0x69, 0x6E, 0x67, 0x20, 0x42, 0x69, 
          0x74, 0x73, 0x2C, 0x20, 0x65, 0x6D, 0x62, 0x65, 0x64, 0x64, 
          0x65, 0x64, 0x20, 0x69, 0x6E, 0x74, 0x65, 0x6C, 0x6C, 0x69, 
          0x67, 0x65, 0x6E, 0x63, 0x65, 0x20, 0x21 
        }; // dummy frame
    static uint8_t rx_frame[1560];

    pc.printf("Setup SPI ...\r\n");
    setupSPI();
    pc.printf("Setup ethernet ...\r\n");

    rev = initialize();
    
    pc.printf("REV = %d\r\n",rev);
    
    while(1)
    {
        uint8_t link = isLinkUp();
        pc.printf("LINK = %d\r\n",isLinkUp());
        if(link)
            break;
        wait(1);
    }

//    IPAddress   localIP = UIPEthernet.localIP();
UIPEthernet.begin(macaddr, MY_IP,MY_DNS,MY_GW,MY_NM);
IPAddress   localIP = UIPEthernet.localIP();
    pc.printf("Local IP = ");
    for(uint8_t i = 0; i < 3; i++)
        pc.printf("%d.", localIP[i]);
    pc.printf("%d\r\n", localIP[3]);
    myServer.begin();
        while(1) {
        //pc.printf("in while\n\r");
        EthernetClient  client = myServer.available();
        if(client) {
        pc.printf("in client\n\r");
            size_t  size = client.available();
            if(size > 0) {
                pc.printf("in size>0\n\r");
                char*   buf = (char*)malloc(size);
                size = client.read((uint8_t*)buf, size);
                if(buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T') {
                    pc.printf("GET request received:\n\r");
                    pc.printf(buf);
                    char    echoHeader[256] = {};
                    sprintf
                    (
                        echoHeader,
                        "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text\r\nConnection: About to close\r\n\r\n",
                        size
                    );
                    client.write((uint8_t*)echoHeader, strlen(echoHeader));
                    client.write((uint8_t*)buf, size);
                    pc.printf("Echo done.\r\n");
                }
                free(buf);
            }else{
                pc.printf("in size<=0\n\r");
            }
        }
    }
#if 0    
    while(1)
    {
        uint8_t reg = readRegByte(EIR);
        if(reg & EIR_PKTIF)
        {
            uint16_t len;
            pc.printf("INTERRUPT = %2X\r\n",reg);
            len = packetReceive(rx_frame,1560);
            pc.printf("RX %d bytes\r\n",len);
            writeOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_PKTIF);

        }
        packetSend(tx_frame,51);
        wait(1);
    }
#endif 
}

