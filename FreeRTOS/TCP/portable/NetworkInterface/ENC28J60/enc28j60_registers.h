/**
  enc28j60.h

  Company:
    Microchip Technology Inc.

  File Name:
    enc28j60.h

  Summary:
    Brief Description of the file (will placed in a table if using Doc-o-Matic)

  Description:
    This section is for a description of the file.  It should be in complete
    sentences describing the purpose of this file.

 */

/*

CopyRight  [2015] Microchip Technology Inc. and its subsidiaries.  You may use this software
and any derivatives exclusively with Microchip products.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS,
IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF
NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE, OR ITS
INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE
IN ANY APPLICATION.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL
OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED
TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY
OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S
TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS.

*/

#ifndef __ENC28J60_REGISTERS_H
#define __ENC28J60_REGISTERS_H

#define SFR_BANK0 0x00
#define SFR_BANK1 0x40
#define SFR_BANK2 0x80
#define SFR_BANK3 0xC0
#define SFR_COMMON 0xE0
#define BANK_MASK 0xE0
#define SFR_MASK 0x1F

typedef enum {
	rcr_inst = 0x00,
	rbm_inst = 0x3A,
	wcr_inst = 0x40,
	wbm_inst = 0x7A,
	bfs_inst = 0x80,
	bfc_inst = 0xa0,
	src_inst = 0xFF
} spi_inst_t;

typedef enum {
	sfr_bank0  = SFR_BANK0,
	sfr_bank1  = SFR_BANK1,
	sfr_bank2  = SFR_BANK2,
	sfr_bank3  = SFR_BANK3,
	sfr_common = SFR_COMMON
} sfr_bank_t;

typedef enum {
	// bank 0
	J60_ERDPTL   = (SFR_BANK0 | 0x00),
	J60_ERDPTH   = (SFR_BANK0 | 0x01),
	J60_EWRPTL   = (SFR_BANK0 | 0x02),
	J60_EWRPTH   = (SFR_BANK0 | 0x03),
	J60_ETXSTL   = (SFR_BANK0 | 0x04),
	J60_ETXSTH   = (SFR_BANK0 | 0x05),
	J60_ETXNDL   = (SFR_BANK0 | 0x06),
	J60_ETXNDH   = (SFR_BANK0 | 0x07),
	J60_ERXSTL   = (SFR_BANK0 | 0x08),
	J60_ERXSTH   = (SFR_BANK0 | 0x09),
	J60_ERXNDL   = (SFR_BANK0 | 0x0A),
	J60_ERXNDH   = (SFR_BANK0 | 0x0B),
	J60_ERXRDPTL = (SFR_BANK0 | 0x0C),
	J60_ERXRDPTH = (SFR_BANK0 | 0x0D),
	J60_ERXWRPTL = (SFR_BANK0 | 0x0E),
	J60_ERXWRPTH = (SFR_BANK0 | 0x0F),
	J60_EDMASTL  = (SFR_BANK0 | 0x10),
	J60_EDMASTH  = (SFR_BANK0 | 0x11),
	J60_EDMANDL  = (SFR_BANK0 | 0x12),
	J60_EDMANDH  = (SFR_BANK0 | 0x13),
	J60_EDMADSTL = (SFR_BANK0 | 0x14),
	J60_EDMADSTH = (SFR_BANK0 | 0x15),
	J60_EDMACSL  = (SFR_BANK0 | 0x16),
	J60_EDMACSH  = (SFR_BANK0 | 0x17),
	RSRV_018     = (SFR_BANK0 | 0x18),
	RSRV_019     = (SFR_BANK0 | 0x19),
	RSRV_01A     = (SFR_BANK0 | 0x1A),
	// bank 1
	J60_EHT0    = (SFR_BANK1 | 0x00),
	J60_EHT1    = (SFR_BANK1 | 0x01),
	J60_EHT2    = (SFR_BANK1 | 0x02),
	J60_EHT3    = (SFR_BANK1 | 0x03),
	J60_EHT4    = (SFR_BANK1 | 0x04),
	J60_EHT5    = (SFR_BANK1 | 0x05),
	J60_EHT6    = (SFR_BANK1 | 0x06),
	J60_EHT7    = (SFR_BANK1 | 0x07),
	J60_EPMM0   = (SFR_BANK1 | 0x08),
	J60_EPMM1   = (SFR_BANK1 | 0x09),
	J60_EPMM2   = (SFR_BANK1 | 0x0A),
	J60_EPMM3   = (SFR_BANK1 | 0x0B),
	J60_EPMM4   = (SFR_BANK1 | 0x0C),
	J60_EPMM5   = (SFR_BANK1 | 0x0D),
	J60_EPMM6   = (SFR_BANK1 | 0x0E),
	J60_EPMM7   = (SFR_BANK1 | 0x0F),
	J60_EPMCSL  = (SFR_BANK1 | 0x10),
	J60_EPMCSH  = (SFR_BANK1 | 0x11),
	RSRV_112    = (SFR_BANK1 | 0x12),
	RSRV_113    = (SFR_BANK1 | 0x13),
	J60_EPMOL   = (SFR_BANK1 | 0x14),
	J60_EPMOH   = (SFR_BANK1 | 0x15),
	RSRV_116    = (SFR_BANK1 | 0x16),
	RSRV_117    = (SFR_BANK1 | 0x17),
	J60_ERXFCON = (SFR_BANK1 | 0x18),
	J60_EPKTCNT = (SFR_BANK1 | 0x19),
	RSRV_11A    = (SFR_BANK1 | 0x1A),
	// bank 2
	J60_MACON1   = (SFR_BANK2 | 0x00),
	J60_MACON2   = (SFR_BANK2 | 0x01),
	J60_MACON3   = (SFR_BANK2 | 0x02),
	J60_MACON4   = (SFR_BANK2 | 0x03),
	J60_MABBIPG  = (SFR_BANK2 | 0x04),
	RSRV_205     = (SFR_BANK2 | 0x05),
	J60_MAIPGL   = (SFR_BANK2 | 0x06),
	J60_MAIPGH   = (SFR_BANK2 | 0x07),
	J60_MACLCON1 = (SFR_BANK2 | 0x08),
	J60_MACLCON2 = (SFR_BANK2 | 0x09),
	J60_MAMXFLL  = (SFR_BANK2 | 0x0A),
	J60_MAMXFLH  = (SFR_BANK2 | 0x0B),
	RSRV_20C     = (SFR_BANK2 | 0x0C),
	RSRV_20D     = (SFR_BANK2 | 0x0D),
	RSRV_20E     = (SFR_BANK2 | 0x0E),
	RSRV_20F     = (SFR_BANK2 | 0x0F),
	RSRV_210     = (SFR_BANK2 | 0x10),
	RSRV_211     = (SFR_BANK2 | 0x11),
	J60_MICMD    = (SFR_BANK2 | 0x12),
	RSRV_213     = (SFR_BANK2 | 0x13),
	J60_MIREGADR = (SFR_BANK2 | 0x14),
	RSRV_215     = (SFR_BANK2 | 0x15),
	J60_MIWRL    = (SFR_BANK2 | 0x16),
	J60_MIWRH    = (SFR_BANK2 | 0x17),
	J60_MIRDL    = (SFR_BANK2 | 0x18),
	J60_MIRDH    = (SFR_BANK2 | 0x19),
	RSRV_21A     = (SFR_BANK2 | 0x1A),
	// bank 3
	J60_MAADR5  = (SFR_BANK3 | 0x00),
	J60_MAADR6  = (SFR_BANK3 | 0x01),
	J60_MAADR3  = (SFR_BANK3 | 0x02),
	J60_MAADR4  = (SFR_BANK3 | 0x03),
	J60_MAADR1  = (SFR_BANK3 | 0x04),
	J60_MAADR2  = (SFR_BANK3 | 0x05),
	J60_EBSTSD  = (SFR_BANK3 | 0x06),
	J60_EBSTCON = (SFR_BANK3 | 0x07),
	J60_EBSTCSL = (SFR_BANK3 | 0x08),
	J60_EBSTCSH = (SFR_BANK3 | 0x09),
	J60_MISTAT  = (SFR_BANK3 | 0x0A),
	RSRV_30B    = (SFR_BANK3 | 0x0B),
	RSRV_30C    = (SFR_BANK3 | 0x0C),
	RSRV_30D    = (SFR_BANK3 | 0x0D),
	RSRV_30E    = (SFR_BANK3 | 0x0E),
	RSRV_30F    = (SFR_BANK3 | 0x0F),
	RSRV_310    = (SFR_BANK3 | 0x10),
	RSRV_311    = (SFR_BANK3 | 0x11),
	J60_EREVID  = (SFR_BANK3 | 0x12),
	RSRV_313    = (SFR_BANK3 | 0x13),
	RSRV_314    = (SFR_BANK3 | 0x14),
	J60_ECOCON  = (SFR_BANK3 | 0x15),
	RSRV_316    = (SFR_BANK3 | 0x16),
	J60_EFLOCON = (SFR_BANK3 | 0x17),
	J60_EPAUSL  = (SFR_BANK3 | 0x18),
	J60_EPAUSH  = (SFR_BANK3 | 0x19),
	RSRV_31A    = (SFR_BANK3 | 0x1A),
	// the following 5 registers are in all banks
	J60_EIE   = (SFR_COMMON | 0x1B),
	J60_EIR   = (SFR_COMMON | 0x1C),
	J60_ESTAT = (SFR_COMMON | 0x1D),
	J60_ECON2 = (SFR_COMMON | 0x1E),
	J60_ECON1 = (SFR_COMMON | 0x1F)
} enc28j60_registers_t;

typedef enum {
	J60_PHCON1  = 0x00,
	J60_PHSTAT1 = 0x01,
	J60_PHID1   = 0x02,
	J60_PHID2   = 0x03,
	J60_PHCON2  = 0x10,
	J60_PHSTAT2 = 0x11,
	J60_PHIE    = 0x12,
	J60_PHIR    = 0x13,
	J60_PHLCON  = 0x14 // display receive activity
} enc28j60_phy_registers_t;

// select register definitions
typedef union {
	char val;
	struct {
		unsigned RXERIF : 1;
		unsigned TXERIF : 1;
		unsigned : 1;
		unsigned TXIF : 1;
		unsigned LINKIF : 1;
		unsigned DMAIF : 1;
		unsigned PKTIF : 1;
		unsigned : 1;
	};
} eir_t;

typedef union {
	unsigned int val;
	struct {
		unsigned : 5;
		unsigned PLRITY : 1;
		unsigned : 3;
		unsigned DPXSTAT : 1;
		unsigned LSTAT : 1;
		unsigned COLSTAT : 1;
		unsigned RXSTAT : 1;
		unsigned TXSTAT : 1;
		unsigned : 2;
	};
} phstat2_t;

// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
// - Register address        (bits 0-4)
// - Bank number        (bits 5-6)
// - MAC/PHY indicator        (bit 7)

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
// ENC28J60 MACON4 Register Bit Definitions
#define MACON4_DEFER	 0x40
#define MACON4_BPEN		 0x20
#define MACON4_NOBKOFF	 0x10
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
// ENC28J60 PHY PHSTAT2 Register Bit Definitions
#define PHSTAT2_TXSTAT	 0x2000
#define PHSTAT2_RXSTAT	 0x1000
#define PHSTAT2_COLSTAT	 0x0800
#define PHSTAT2_LSTAT	 0x0400
#define PHSTAT2_DPXSTAT	 0x0200
#define PHSTAT2_PLRITY	 0x0020
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK   0x4000
#define PHCON2_TXDIS     0x2000
#define PHCON2_JABBER    0x0400
#define PHCON2_HDLDIS    0x0100
/* ENC28J60 PHY PHIE Register Bit Definitions */
#define PHIE_PLNKIE	(1 << 4)
#define PHIE_PGEIE	(1 << 1)
/* ENC28J60 PHY PHIR Register Bit Definitions */
#define PHIR_PLNKIF	(1 << 4)
#define PHIR_PGEIF	(1 << 1)

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN  0x08
#define PKTCTRL_PPADEN   0x04
#define PKTCTRL_PCRCEN   0x02
#define PKTCTRL_POVERRIDE 0x01

#define RXSTART_INIT	0x0000
#define RXSTOP_INIT		0x19FF			// room for 3 packets
#define TXSTART_INIT	(RXSTOP_INIT+1)
#define TXSTOP_INIT		0x1FFF			// room for 2 packets

typedef struct {
	uint8_t nextPacketPointerL;
	uint8_t nextPackterPointerH;
	uint8_t	status7_0;
	uint8_t status15_8;
	uint8_t status23_16;
	uint8_t status31_24;
}__attribute__((packed)) enc28_recv_status_vector_t;

#endif // __ENC28J60_REGISTERS_H
