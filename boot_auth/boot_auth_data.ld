MEMORY
{
    BOOT_DATA_FLASH : ORIGIN = 0xFF23C000, LENGTH = 16K
}

SECTIONS
{
    .boot_auth_data align(4) :>BOOT_DATA_FLASH
    .sdabase :>.
    .robase :>.
    .rosdata :>.
    .tdata :>.
    .secinfo :>.
}
