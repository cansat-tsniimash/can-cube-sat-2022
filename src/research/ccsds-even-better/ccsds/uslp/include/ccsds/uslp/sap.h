#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "uslp_types.h"
#include "ccsds/epp/epp.h"





size_t sap_epp_send(sap_t* sap, const epp_packet_t* packet);

size_t sap_send(sap_t* sap, const uint8_t* data, size_t size);

