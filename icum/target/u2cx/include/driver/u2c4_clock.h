#ifndef U2C4_CLOCK_H
#define U2C4_CLOCK_H

#define U2C4_CLOCK_REG(address) (*(volatile uint32_t *)(address))

static uint32_t u2c4_clock_wait(uint32_t address, uint32_t mask, uint32_t expected)
{
    uint32_t remaining = 1000000UL;
    while (remaining-- != 0UL)
    {
        if ((U2C4_CLOCK_REG(address) & mask) == expected)
        {
            return 1UL;
        }
    }
    return 0UL;
}

static void u2c4_clock_delay(void)
{
    volatile uint32_t remaining = 80000UL;
    while (remaining-- != 0UL)
    {
    }
}

static uint32_t u2c4_clock_init(void)
{
    uint32_t result = 0UL;
    uint32_t step;
    uint32_t sscg_divider;
    uint32_t clock_mode = (U2C4_CLOCK_REG(0xFF320BCCUL) >> 30) & 3UL;
    const uint32_t initial_sscg[4] = {12UL, 10UL, 7UL, 5UL};

    if ((U2C4_CLOCK_REG(0xFF980108UL) == 0UL) &&
        (U2C4_CLOCK_REG(0xFF980114UL) == 0UL) &&
        (U2C4_CLOCK_REG(0xFF980120UL) == 0UL) &&
        (U2C4_CLOCK_REG(0xFF980130UL) == 0UL))
    {
        return 1UL;
    }

    U2C4_CLOCK_REG(0xFF980700UL) = 0xA5A5A501UL;
    if ((U2C4_CLOCK_REG(0xFF988004UL) & 1UL) == 0UL)
    {
        U2C4_CLOCK_REG(0xFF988000UL) = 1UL;
    }
    if (!u2c4_clock_wait(0xFF988004UL, 3UL, 3UL))
    {
        goto finish;
    }
    if ((U2C4_CLOCK_REG(0xFF980004UL) & 1UL) == 0UL)
    {
        U2C4_CLOCK_REG(0xFF980000UL) = 1UL;
    }
    if (!u2c4_clock_wait(0xFF980004UL, 3UL, 3UL))
    {
        goto finish;
    }

    U2C4_CLOCK_REG(0xFF980120UL) = 4UL;
    sscg_divider = initial_sscg[clock_mode];
    U2C4_CLOCK_REG(0xFF980130UL) = sscg_divider;
    if (!u2c4_clock_wait(0xFF980128UL, 2UL, 2UL) ||
        !u2c4_clock_wait(0xFF980134UL, 2UL, 2UL))
    {
        goto finish;
    }
    if (U2C4_CLOCK_REG(0xFF9800C0UL) != 0UL)
    {
        U2C4_CLOCK_REG(0xFF9800C0UL) = 1UL;
        if (!u2c4_clock_wait(0xFF9800C4UL, 2UL, 2UL))
        {
            goto finish;
        }
        u2c4_clock_delay();
        U2C4_CLOCK_REG(0xFF9800C0UL) = 0UL;
        if (!u2c4_clock_wait(0xFF9800C4UL, 2UL, 2UL))
        {
            goto finish;
        }
        u2c4_clock_delay();
    }

    U2C4_CLOCK_REG(0xFF980100UL) = 0UL;
    U2C4_CLOCK_REG(0xFF980110UL) = 0UL;
    if (!u2c4_clock_wait(0xFF980108UL, 1UL, 0UL) ||
        !u2c4_clock_wait(0xFF980114UL, 1UL, 0UL))
    {
        goto finish;
    }
    u2c4_clock_delay();
    for (step = 6UL; step <= 16UL; step += 2UL)
    {
        U2C4_CLOCK_REG(0xFF980120UL) = (step == 16UL) ? 0UL : step;
        if (sscg_divider != 0UL)
        {
            sscg_divider += 2UL;
            if (sscg_divider >= 16UL)
            {
                sscg_divider = 0UL;
            }
            U2C4_CLOCK_REG(0xFF980130UL) = sscg_divider;
        }
        if (!u2c4_clock_wait(0xFF980128UL, 2UL, 2UL) ||
            !u2c4_clock_wait(0xFF980134UL, 2UL, 2UL))
        {
            goto finish;
        }
        u2c4_clock_delay();
    }
    result = 1UL;

finish:
    U2C4_CLOCK_REG(0xFF980700UL) = 0xA5A5A500UL;
    return result;
}

#endif
