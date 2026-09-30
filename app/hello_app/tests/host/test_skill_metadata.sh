#!/usr/bin/env bash
set -euo pipefail

app_root="$(cd "$(dirname "$0")/../.." && pwd)"
skill="$app_root/skills/dinner-assistant.md"
demo="$app_root/../../docs/SKILL_DEMO.md"

test -f "$skill"
grep -q '^# Dinner Assistant$' "$skill"
grep -q '^## When to use$' "$skill"
grep -q '^## How to use$' "$skill"
grep -q '^## Example$' "$skill"
grep -q '/data/agent/skills/dinner-assistant.md' "$demo"

assert_contract() {
  if ! grep -Fq -- "$1" "$skill"; then
    echo "FAIL: missing runtime contract: $1" >&2
    exit 1
  fi
}

assert_contract 'Return one JSON object with reply, candidates, candidate_count, next_state, and actions.'
assert_contract 'next_state allowlist: LISTENING, RECOMMENDATION, ERROR.'
assert_contract 'actions allowlist: SHOW_STATE, SHOW_QR; encode actions as an array of names.'
assert_contract 'candidate_count must equal the candidates array length (0–3).'
assert_contract 'With candidates, next_state must be RECOMMENDATION.'
assert_contract 'Without candidates, next_state must be LISTENING or ERROR; omit SHOW_QR.'
assert_contract 'Reject SET_LIGHT, SAVE_CONFIRMED_MEMORY, and unknown actions.'
assert_contract 'Reject shell/device control, credentials, and payment requests in runtime output.'
assert_contract 'Persistent memory and physical actions require explicit application confirmation.'
assert_contract 'Only use a verified QR target supplied by the application; otherwise use an empty qr_target and omit SHOW_QR.'
assert_contract 'lc_dinner_result_t'
assert_contract 'lc_dinner_validate_result'
assert_contract 'does not implement JSON parsing'
assert_contract 'name, reason, qr_target, price_cents, allergen_flags, spicy, and requires_cooking'
assert_contract 'reply: 159, name: 47, reason: 95, qr_target: 127'
assert_contract '{"reply":"What is your budget for dinner?","candidates":[],"candidate_count":0,"next_state":"LISTENING","actions":["SHOW_STATE"]}'

expected_headings=$'# Dinner Assistant\n## When to use\n## How to use\n## Example'
if [[ "$(grep '^#' "$skill")" != "$expected_headings" ]]; then
  echo 'FAIL: dinner assistant headings changed' >&2
  exit 1
fi

echo 'PASS: dinner assistant skill metadata'
