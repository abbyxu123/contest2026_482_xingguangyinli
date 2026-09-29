# Competition submission status

Last verified: 2026-09-29 (Asia/Shanghai)

This page maps the current Living Canvas repository to the official openvela
AI hardware competition requirements. It is an evidence index, not a claim
that every planned product capability is complete.

## Required repository deliverables

| Requirement | Status | Repository evidence |
| --- | --- | --- |
| Work developed in the official team repository | Complete | This repository and its merged pull-request history |
| openvela application source | Complete | `app/hello_app/`, `openvela.xml`, `contest2026_482_xingguangyinli.xml` |
| Apache-2.0 licensing | Complete | `LICENSE` |
| AI Coding development Skill | Complete | `skills/gemini-s1-firmware-acceptance/SKILL.md` |
| Runtime custom Skill source | Complete at source level | `app/hello_app/skills/dinner-assistant.md`, `docs/SKILL_DEMO.md` |
| AI Coding session logs | Pending | Only collector-generated, reviewed, validated JSONL may be added under `logs/abbyxu123/` |

## Verified engineering gates

| Gate | Status | Evidence |
| --- | --- | --- |
| Core C state machine and safety boundaries | Host tested | `app/hello_app/tests/host/` |
| Living Canvas decision backend | Host tested | `backend/tests/` |
| Gemini-S1 target build and package inspection | Verified | `tests/evidence/build/` |
| Full NAND programming and NAND boot | Verified | `tests/evidence/device/gemini-s1-full-flash-runtime-20260926.txt` |
| 240×320 upright Living Canvas UI | Verified on the physical SPI panel | `tests/evidence/device/gemini-s1-portrait-runtime-20260929.txt` |
| Portrait choice state machine and LVGL touch integration | Host tested and target built | `tests/evidence/build/gemini-s1-touch-choice-20260929.txt` |

## Physical gates still pending

The following items remain independent physical acceptance gates and are not
claimed as complete by this repository:

- portrait touch-coordinate mapping and repeated two-tap selection;
- `ai_agent` runtime, LLM connectivity, and one live interaction channel on
  Gemini-S1;
- deployment and invocation of
  `/data/agent/skills/dinner-assistant.md` on the device;
- one demonstrated event-driven proactive flow followed by a bounded,
  user-confirmed execution;
- public project-QR readability, audio output, network recovery, sensors, and
  multi-device interaction.

## External submission items

The competition also requires a work-introduction document, a demonstration
video no longer than five minutes, and the official team-repository URL.
These items are submitted through the competition submission channel; their
final portal status cannot be inferred from this Git repository.

## Final repository checks

Before the final submission cutoff:

1. Generate a real AI Coding session from inside the openvela workspace using
   a supported collector-enabled tool.
2. Review the entire session for project relevance and sensitive information.
   Delete an unsuitable session as a whole; never edit generated events.
3. Validate the selected log with the official `validate-log.py` tool and
   commit only the generated JSONL and manifest entry.
4. Run the host tests, backend tests, privacy checks, and `git diff --check`.
5. Confirm the work-introduction document, current Gemini-S1 video, and team
   repository URL are present in the official submission channel.
