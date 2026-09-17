from fastapi import FastAPI


def create_app() -> FastAPI:
    app = FastAPI(title="Living Canvas Decision Backend", version="0.1.0")

    @app.get("/health")
    def health():
        return {
            "service": "living-canvas-decision-backend",
            "status": "ok",
        }

    return app


app = create_app()
