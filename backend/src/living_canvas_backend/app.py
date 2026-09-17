from fastapi import FastAPI, HTTPException

from .decision import load_catalog, recommend
from .schema import (
    BoardInputRequest,
    SessionCreateRequest,
    SessionState,
)
from .session_store import SessionStore


def create_app() -> FastAPI:
    app = FastAPI(title="Living Canvas Decision Backend", version="0.1.0")
    sessions = SessionStore(max_sessions=200)
    catalog = load_catalog()
    app.state.sessions = sessions

    @app.get("/health")
    def health():
        return {
            "service": "living-canvas-decision-backend",
            "status": "ok",
        }

    @app.post("/v1/session", status_code=201)
    def create_session(request: SessionCreateRequest):
        return sessions.create(request.device_id)

    @app.post("/v1/input")
    def submit_input(request: BoardInputRequest):
        session = sessions.get(request.session_id)
        if session is None:
            raise HTTPException(status_code=404, detail="session_not_found")

        constraints = request.hard_constraints.as_domain()
        recommendation = recommend(
            catalog,
            constraints,
            request.soft_preferences,
        )
        if recommendation is None:
            raise HTTPException(status_code=409, detail="no_safe_candidate")

        updated = session.model_copy(
            update={
                "state": SessionState.CANDIDATE,
                "context": request.context,
                "hard_constraints": constraints,
                "soft_preferences": request.soft_preferences,
                "recommendation": recommendation,
            }
        )
        sessions.save(updated)
        return updated

    return app


app = create_app()
