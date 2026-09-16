#include "lc_sid_contract.h"

#include <ctype.h>
#include <stdio.h>

static bool valid_identifier(const char *value)
{
  size_t index;

  if (value == NULL || value[0] == '\0')
    {
      return false;
    }

  for (index = 0u; value[index] != '\0'; index++)
    {
      unsigned char character = (unsigned char)value[index];
      if (index >= 63u ||
          (!isalnum(character) && character != '_' && character != '-'))
        {
          return false;
        }
    }

  return true;
}

static bool build_identifier_json(const char *field,
                                  const char *value,
                                  char *output,
                                  size_t output_capacity)
{
  int length;

  if (field == NULL || !valid_identifier(value) || output == NULL ||
      output_capacity == 0u)
    {
      return false;
    }

  length = snprintf(output, output_capacity, "{\"%s\":\"%s\"}",
                    field, value);
  return length >= 0 && (size_t)length < output_capacity;
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

bool lc_sid_build_session_json(const char *device_id,
                               char *output,
                               size_t output_capacity)
{
  return build_identifier_json("device_id", device_id, output,
                               output_capacity);
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
      !valid_identifier(session_id))
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

bool lc_sid_build_confirm_json(const char *session_id,
                               char *output,
                               size_t output_capacity)
{
  return build_identifier_json("session_id", session_id, output,
                               output_capacity);
}
