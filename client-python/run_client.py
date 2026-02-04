#!/usr/bin/env python3
# Run the Distributed Bank UDP client. Usage: python run_client.py HOST PORT

import sys

# Run from repo root so src.client is importable
if __name__ == "__main__":
    from src.main import main
    sys.exit(main())
