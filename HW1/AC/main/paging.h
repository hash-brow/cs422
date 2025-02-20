/*
 * paging.h
 * cs422 hw 1
 *
 * created on feb 19, 2025
 *
 */

#ifndef _PAGING_H
#define _PAGING_H

#include <stdint.h>

// granularity in bytes
#define GRAN 32

#define PG_DIR_SZ 1024
#define PG_TABLE_SZ 1024
#define PG_SZ 4096

// present bits for PDE, PTE
#define PTE_P 0x1
#define PDE_P 0x1

// page dir entry, page table entry
typedef uint32_t pde_t;
typedef uint32_t pte_t;

#define PTXSHIFT 12 // offset of PTX in a linear addr
#define PDXSHIFT 22 // offset of PDX in a linear addr
#define PGXSHIFT 5  // offset of PGX in a linear addr

// first 10 bits of va are pgdir offset, next 10 are pg table offset
#define PDX(va) ((uint32_t)(va) >> PDXSHIFT) 
#define PTX(va) (((uint32_t)(va) >> PTXSHIFT) & 0x3FF)
#define PGX(va) (((uint32_t)(va) >> PGXSHIFT) & 0x7F)

#define PGDOWN(va) ((uint32_t)(va) & ~(0x1F))

#endif // _PAGING_H
