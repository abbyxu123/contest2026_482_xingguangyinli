#include "lc_sid_contract.h"

#include <ctype.h>
#include <stdio.h>

static bool valid_session_id(const char *session_id)
{
  size_t index;

  if (session_id == NULL || session_id[0] == '\0')
    {
      return false;
    }

  for (index = 0u; session_id[index] != '\0'; index++)
    {
      unsigned char value = (unsigned char)session_id[index];
      if (index >= 63u ||
          (!isalnum(value) && value != '_' && value != '-'))
        {
          return false;
        }
    }

  return true;
}

bool lc_sid_contract_for_choice(lc_choice_id_t choice,
                                lc_sid_contract_t *contract)
{
  if (contract == NULL)
    {
      return false;
    }

  contract->requires_confirmation = true;
  switch (choice)
    {
      case LC_CHOICE_TAKEOUT:
        contract->input_text = "帮我点外卖";
        contract->channel = "delivery";
        contract->novelty = "balanced";
        return true;

      case LC_CHOICE_MYSTERY:
        contract->input_text = "从我过去喜欢的外卖品类里随机推荐一个";
        contract->channel = "delivery";
        contract->novelty = "bold";
        return true;

      case LC_CHOICE_HOME:
        contract->input_text = "查看冰箱里的食材并建议在家吃什么";
        contract->channel = "dine_in";
        contract->novelty = "conservative";
        return true;

      default:
        return false;
    }
}

bool lc_sid_build_input_json(const lc_sid_contract_t *contract,
                             const char *session_id,
                             char *output,
                             size_t output_capacity)
{
  int length;

  if (contract == NULL || contract->input_text == NULL ||
      contract->channel == NULL || contract->novelty == NULL ||
      output == NULL || output_capacity == 0u ||
      !valid_session_id(session_id))
    {
      return false;
    }

  length = snprintf(output, output_capacity,
                    "{\"session_id\":\"%s\",\"text\":\"%s\","
                    "\"soft_preferences\":{\"novelty\":\"%s\"},"
                    "\"context\":{\"channel\":\"%s\"}}",
                    session_id, contract->input_text, contract->novelty,
                    contract->channel);
  return length >= 0 && (size_t)length < output_capacity;
}
