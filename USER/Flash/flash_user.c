#include "flash_user.h"

void flash_user_init() {
    norflash_init();
   printf("read_id -> %d", norflash_read_id());
}