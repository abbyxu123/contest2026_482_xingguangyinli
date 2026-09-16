#include "lc_choice.h"
#include "lc_sid_contract.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_fixed_endpoints(void)
{
  assert(strcmp(LC_SID_SESSION_ENDPOINT, "/v1/session") == 0);
  assert(strcmp(LC_SID_INPUT_ENDPOINT, "/v1/input") == 0);
  assert(strcmp(LC_SID_CONFIRM_ENDPOINT, "/v1/confirm") == 0);
}

static void test_takeout_contract(void)
{
  lc_sid_contract_t contract;
  char json[320];

  assert(lc_sid_contract_for_choice(LC_CHOICE_TAKEOUT, &contract));
  assert(strcmp(contract.channel, "delivery") == 0);
  assert(strcmp(contract.novelty, "balanced") == 0);
  assert(contract.requires_confirmation);
  assert(lc_sid_build_input_json(&contract, "sess_123", json,
                                 sizeof(json)));
  assert(strstr(json, "\"session_id\":\"sess_123\"") != NULL);
  assert(strstr(json, "\"channel\":\"delivery\"") != NULL);
}

static void test_mystery_and_home_mapping(void)
{
  lc_sid_contract_t mystery;
  lc_sid_contract_t home;

  assert(lc_sid_contract_for_choice(LC_CHOICE_MYSTERY, &mystery));
  assert(strcmp(mystery.channel, "delivery") == 0);
  assert(strcmp(mystery.novelty, "bold") == 0);
  assert(strstr(mystery.input_text, "随机") != NULL);

  assert(lc_sid_contract_for_choice(LC_CHOICE_HOME, &home));
  assert(strcmp(home.channel, "dine_in") == 0);
  assert(strcmp(home.novelty, "conservative") == 0);
  assert(strstr(home.input_text, "冰箱") != NULL);
}

static void test_rejects_unsafe_or_truncated_session_ids(void)
{
  lc_sid_contract_t contract;
  char json[48];

  assert(lc_sid_contract_for_choice(LC_CHOICE_TAKEOUT, &contract));
  assert(!lc_sid_build_input_json(&contract, "bad\"id", json,
                                  sizeof(json)));
  assert(!lc_sid_build_input_json(&contract, "sess_123", json,
                                  sizeof(json)));
}

int main(void)
{
  test_fixed_endpoints();
  test_takeout_contract();
  test_mystery_and_home_mapping();
  test_rejects_unsafe_or_truncated_session_ids();
  puts("PASS: lc_sid_contract");
  return 0;
}
