#ifndef LIVING_CANVAS_LC_SID_CONTRACT_H
#define LIVING_CANVAS_LC_SID_CONTRACT_H

#include "lc_choice.h"

#include <stdbool.h>
#include <stddef.h>

#define LC_SID_SESSION_ENDPOINT "/v1/session"
#define LC_SID_INPUT_ENDPOINT "/v1/input"
#define LC_SID_CONFIRM_ENDPOINT "/v1/confirm"

typedef struct
{
  const char *input_text;
  const char *channel;
  const char *novelty;
  bool requires_confirmation;
} lc_sid_contract_t;

bool lc_sid_contract_for_choice(lc_choice_id_t choice,
                                lc_sid_contract_t *contract);
bool lc_sid_build_input_json(const lc_sid_contract_t *contract,
                             const char *session_id,
                             char *output,
                             size_t output_capacity);

#endif
