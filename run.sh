#!/bin/sh
# CardGuard — Launch web UI
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
WEB="$SCRIPT_DIR/web/index.html"

if command -v python3 >/dev/null 2>&1; then
  echo "CardGuard starting on http://localhost:8765"
  echo "Open http://localhost:8765 in your browser."
  (sleep 1 && open "http://localhost:8765" 2>/dev/null || xdg-open "http://localhost:8765" 2>/dev/null) &
  cd "$SCRIPT_DIR/web" && python3 -m http.server 8765
elif command -v python >/dev/null 2>&1; then
  echo "CardGuard starting on http://localhost:8765"
  (sleep 1 && open "http://localhost:8765" 2>/dev/null || xdg-open "http://localhost:8765" 2>/dev/null) &
  cd "$SCRIPT_DIR/web" && python -m http.server 8765
else
  echo "Opening CardGuard (file://)"
  open "$WEB" 2>/dev/null || xdg-open "$WEB" 2>/dev/null || echo "Open: $WEB"
fi
