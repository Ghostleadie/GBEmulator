#pragma once

#include <memory>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "log/Log.h"
#include "Utility/utility.h"

//checks a specific bit in a number set
#define BIT(a, n) ((a & (1 << n)) ? 1 : 0)

#define BIT_SET(a, n, on) (on ? (a) |= (1 << n) : (a) &= ~(1 << n))

#define BETWEEN(a, b, c) ((a >= b) && (a <= c))