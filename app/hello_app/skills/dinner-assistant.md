# Dinner Assistant

Help the user narrow a dinner decision in a few short turns while keeping
budget, allergies, dietary exclusions, temporary preferences, confirmation,
and payment boundaries explicit.

## When to use

Use this Skill when the user asks what to eat for dinner, requests a takeaway
or pickup suggestion, wants a small surprise choice, or asks for a practical
meal that fits stated constraints.

## How to use

1. Ask only for missing facts that materially change the decision: approximate
   budget, allergies or confirmed dietary exclusions, delivery/pickup/cooking,
   and temporary preferences such as “not spicy tonight.”
2. Treat health information only as a constraint supplied by the user. Do not
   diagnose a condition or claim medical suitability.
3. Return no more than three candidates. Do not invent availability, prices,
   ingredients, nutrition, delivery times, or links. If current facts are not
   available, say that they still need confirmation.
4. If no safe candidate remains, explain which constraint prevented a result
   and ask one concise follow-up question.
5. Keep temporary choices in the current session. A stable preference can be
   saved only after the user explicitly confirms both the choice and the wish
   to remember it.
6. Never order food, initiate payment, run shell commands, change hardware,
   write credentials, or create a long-term memory entry from Skill output.
   Persistent memory and physical actions require explicit application confirmation.

**Runtime output contract**

Return one JSON object with reply, candidates, candidate_count, next_state, and actions.
This output format maps to lc_dinner_result_t in `include/lc_dinner.h` and the
rules in lc_dinner_validate_result in `src/lc_dinner.c`; that validator checks
C structs and does not implement JSON parsing. The application must map names
to C enums/action bits and validate before use.

- next_state allowlist: LISTENING, RECOMMENDATION, ERROR.
- actions allowlist: SHOW_STATE, SHOW_QR; encode actions as an array of names.
- candidate_count must equal the candidates array length (0–3).
- With candidates, next_state must be RECOMMENDATION.
- Without candidates, next_state must be LISTENING or ERROR; omit SHOW_QR.
  Use LISTENING for missing facts and ERROR when no safe candidate remains.
- Each candidate has name, reason, qr_target, price_cents, allergen_flags, spicy, and requires_cooking.
  The first three are strings, the next two unsigned integers, and the last two
  booleans. Copy factual values only from the application-supplied catalog;
  if required facts are missing, ask for them without inventing a candidate.
- String limits in UTF-8 bytes are reply: 159, name: 47, reason: 95, qr_target: 127
  (leaving room for the C terminator); do not emit embedded NUL characters.
- Keep reply short: a recommendation or explanation, then one question or
  explicit confirmation request. Give each candidate one factual reason.
- Only use a verified QR target supplied by the application; otherwise use an empty qr_target and omit SHOW_QR.
- Reject SET_LIGHT, SAVE_CONFIRMED_MEMORY, and unknown actions.
- Reject shell/device control, credentials, and payment requests in runtime output.
  Report rejection in reply with no candidates, ERROR, and only SHOW_STATE.

## Example

User: “Help me choose dinner.” (Budget is missing.)

```json
{"reply":"What is your budget for dinner?","candidates":[],"candidate_count":0,"next_state":"LISTENING","actions":["SHOW_STATE"]}
```
