/**
 * \addtogroup Secondary PCI loader
 * \{
 */
/**
 * \file
 * Contains the SOC's secondary PCI bootloader. 
 * The bootloader is more or less the same than the PCI bootloader in SOC's flash beside the 
 * fact that the PCI-bridge configuration can be configured.
 * The bootloader 
 *  - writes 0x0000000 to TPA-RAM+0 when started
 *  - copies version information to the address TPA-RAM+0x17f0
 *  - initializes the PCI bridge as specified in the configuration file "spb_config_<product>.h".
 *  - writex 0xbeebbeep to TPA-RAM+0 when initialization is finished
 *  - waits until someone writes 0x68799786 to TPA-RAM+0 (first 4 bytes) per PCI 
 *  - then executes code at TPA-RAM+4 (which has hopefully been filled per PCI before ;-)
 *
 * The bootloader can be programmed to NAND (it is <2KB) or SPI flash.
 * 
 * Copyright (c) SIEMENS AG 2013 <br>
 * All rights reserved
 *
 * \author Martin Behringer, A&D AS RD DH A2
 */
#ifndef SPI_SPECIFIC_H_
#define SPI_SPECIFIC_H_

/**********************************************************************
 * Includes                                                           *
 *********************************************************************/
/* sk3367
#include <stdint.h>

//for register macros
#include <s12i_regdef.h>
#include <s12i_SOC1_TOP.h>

#include <spb_config.h>
*/
/**********************************************************************
 * Defines
 *********************************************************************/

/**
 * Version of the common code (all files in folder "boot/soc1/sec_pci_boot".
 * The number gets increased every time the common code gets modified. 
 */
#define SPB_COMMON_VERSION 1

// identifier for the product this bootloader is designated for
#define SPB_PROD_CPU1518  0x01  ///< bootloader is designated for the product CPU1518
#define SPB_PROD_EC       0x11  ///< bootloader is designated for the product Embedded Controller (EC)
#define SPB_PROD_PC_BOARD 0x21  ///< bootloader is designated for the product PC Boards (PCI / PCIe)

// identifier for the hardware the bootloader is running on
#define SPB_HW_CPU1518               0x10   ///< SOC within the 1518 Hardware
#define SPB_HW_PC_SOC1_PCI_CARD      0x20   ///< SOC PCI card
#define SPB_HW_PC_SOC1_PCIE_CARD     0x30   ///< SOC PCI-Express card
#define SPB_HW_EC257A                0x40   ///< SOC within the embedded controller hardware

/// loader writes this value into first 32bit of tpa ram when it has finished its initalization and
/// waits for "BOOT_KENNUNG"
#define SPB_INIT_FINISHED_KENNUNG 0xbeebbeeb
/** 
 * Loader waits for this value in the first 32bit of TPA-RAM. After this value has been written,
 * the loader executes the code at TPA-RAM+4
 */
#define SPB_BOOT_KENNUNG        0x68799786

/**********************************************************************
 * Structures, Types, Variables                                       *
 *********************************************************************/

/// Structure that holds information about the bootloaders version, 
/// the hardware it is running on and the product its designated for.
typedef struct spb_boot_version_s {
  uint8_t ver_type;         /// Version type (V, R, B, etc.)
  uint8_t ver_maj;          /// Major version number of specific bootloader
  uint8_t ver_min;          /// Minor version number of specific bootloader
  uint8_t ver_build;        /// Build number of specific bootloader
  
  uint16_t hardware_id;     /// Actual Hardware the bootloader is running on (e.g. SOC-PCI-Card, CPU1518-HW, EC-HW)
  uint16_t bar_config_id;   /// Version number of the PCI-bridge settings
  uint8_t  product_id;      /// Product the loader is designated for (e.g. 1518, EC PC_BOARD) 
  
  uint8_t string[7];        /// Optional version string (up to 7 bytes INCLUDING the \0)
} spb_boot_version_t;

/**********************************************************************
 * Prototypes                                                         *
 *********************************************************************/

#endif 
/// \}
