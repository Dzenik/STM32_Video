#ifndef	__BOARD_H__
#define __BOARD_H__

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "ssd1289.h"

#ifndef bool
typedef enum {FALSE = 0, TRUE = !FALSE} bool;
#endif // bool

typedef enum _GRAPHIC_STATE
{
  NOT_ADDRESSED,
  TRANSFER_IN_PROGRESS
} GRAPHIC_STATE;

#endif	// __BOARD_H__
