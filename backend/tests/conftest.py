import pytest
from fastapi.testclient import TestClient

from living_canvas_backend.app import create_app


@pytest.fixture
def client():
    return TestClient(create_app())
