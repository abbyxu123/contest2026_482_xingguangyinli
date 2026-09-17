#!/usr/bin/env python3
"""Verify the Living Canvas P0 flow against a running SID gateway.

This intentionally uses only the Python standard library so it can run from
the Mac or the openvela build VM without installing another client package.
"""

from __future__ import annotations

import argparse
import json
import sys
import time
import urllib.error
import urllib.parse
import urllib.request


class NoRedirect(urllib.request.HTTPRedirectHandler):
    def redirect_request(self, req, fp, code, msg, headers, newurl):
        return None


def request_json(base_url: str, path: str, body: dict | None = None) -> dict:
    data = None
    headers = {"Accept": "application/json"}
    method = "GET"
    if body is not None:
        data = json.dumps(body, ensure_ascii=False).encode("utf-8")
        headers["Content-Type"] = "application/json"
        method = "POST"

    request = urllib.request.Request(
        urllib.parse.urljoin(base_url + "/", path.lstrip("/")),
        data=data,
        headers=headers,
        method=method,
    )
    try:
        with urllib.request.urlopen(request, timeout=15) as response:
            return json.loads(response.read().decode("utf-8"))
    except urllib.error.HTTPError as error:
        detail = error.read().decode("utf-8", errors="replace")
        raise RuntimeError(f"{path} returned HTTP {error.code}: {detail}") from error
    except urllib.error.URLError as error:
        raise RuntimeError(f"cannot reach SID gateway at {base_url}: {error.reason}") from error


def verify_redirect(base_url: str, token: str, session_id: str) -> None:
    opener = urllib.request.build_opener(NoRedirect)
    request = urllib.request.Request(
        urllib.parse.urljoin(base_url + "/", f"c/{token}"), method="GET"
    )
    try:
        opener.open(request, timeout=15)
    except urllib.error.HTTPError as error:
        if error.code != 307:
            raise RuntimeError(f"compact handoff returned HTTP {error.code}") from error
        expected = f"/console?sid={session_id}"
        actual = error.headers.get("Location", "")
        if actual != expected:
            raise RuntimeError(
                f"compact handoff location mismatch: expected {expected}, got {actual}"
            )
        return
    raise RuntimeError("compact handoff did not return HTTP 307")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--base-url",
        default="http://127.0.0.1:8090",
        help="running SID gateway URL",
    )
    args = parser.parse_args()
    base_url = args.base_url.rstrip("/")

    health = request_json(base_url, "/health")
    require(health.get("gateway") == "ok", "SID health check is not ok")

    created = request_json(base_url, "/v1/session", {"device_id": "gemini-s1"})
    session_id = str(created.get("session_id", ""))
    require(session_id.startswith("sess_") and len(session_id) == 17,
            "SID returned an invalid session id")

    decision = request_json(
        base_url,
        "/v1/input",
        {
            "session_id": session_id,
            "text": "1人，帮我点外卖",
            "hard_constraints": {"budget_max": 50, "eat_by_minutes": 30},
            "soft_preferences": {"novelty": "balanced"},
            "context": {"people": 1, "channel": "delivery"},
        },
    )
    require(decision.get("state") == "candidate",
            f"decision did not reach candidate state: {decision.get('state')}")
    require(bool(decision.get("final_choice")), "decision has no final choice")
    require(bool(decision.get("candidates")), "decision has no candidates")

    event = request_json(
        base_url,
        "/v1/device/event",
        {
            "device_id": "gemini-s1",
            "session_id": session_id,
            "event": "right_ear",
            "timestamp": int(time.time() * 1000),
            "firmware_version": "living-canvas-p0",
        },
    )
    require(event.get("state") == "confirming",
            f"right-ear confirmation failed: {event.get('state')}")

    confirmed = request_json(base_url, "/v1/confirm", {"session_id": session_id})
    require(confirmed.get("ok") is True, "SID confirmation failed")
    require(confirmed.get("action") == "order_deeplink",
            f"unexpected action: {confirmed.get('action')}")
    require(str(confirmed.get("url", "")).startswith(("http://", "https://")),
            "SID did not return a phone-safe web URL")

    token = session_id.removeprefix("sess_")
    verify_redirect(base_url, token, session_id)

    chosen_id = decision["final_choice"].get("candidate_id", "")
    print(f"PASS: SID takeout flow session={session_id} candidate={chosen_id}")
    print(f"PASS: compact handoff {base_url}/c/{token} -> /console?sid={session_id}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except RuntimeError as error:
        print(f"FAIL: {error}", file=sys.stderr)
        raise SystemExit(1)
