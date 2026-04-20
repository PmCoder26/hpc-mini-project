#!/bin/bash
echo "Starting PyCUDA / FastAPI HPC Backend..."

# Create virtual environment if it doesn't exist
if [ ! -d ".venv" ]; then
    echo "Creating Python Virtual Environment..."
    python3 -m venv .venv
fi

# Activate virtual environment
source .venv/bin/activate

# Install requirements if uvicorn is missing
if ! command -v uvicorn &> /dev/null
then
    echo "Installing required Python packages (fastapi, uvicorn, pandas, numpy, pycuda)..."
    pip install fastapi uvicorn pandas numpy pycuda pydantic
fi

# Run the backend
echo "Executing FastAPI Server..."
uvicorn backend.main:app --reload
