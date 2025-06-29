from fastapi import FastAPI
from pydantic import BaseModel
import uvicorn

app = FastAPI()

class ProblemRequest(BaseModel):
    problem: str
    data: dict

@app.get("/")
def read_root():
    return {"status": "optimizer-service"}

@app.post("/api/solve")
async def solve_problem(request: ProblemRequest):
    if request.problem == "fertilizer":
        # Just echo back the data for now
        return {
            "status": "success",
            "problem": "fertilizer",
            "data": request.data
        }
    elif request.problem == "sudoku":
        return {
            "status": "success",
            "problem": "sudoku"
        }
    else:
        return {
            "status": "error",
            "message": f"Unknown problem type: {request.problem}"
        }

if __name__ == "__main__":
    uvicorn.run(
        "main:app",
        host="0.0.0.0",
        port=8421,
        reload=True
    )