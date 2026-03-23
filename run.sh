APP_PATH="./build/src/app/FilmLibrary"

if [ -f "$APP_PATH" ]; then
    echo "Starting FilmLibrary..."
    $APP_PATH
else
    echo "Executable not found! Please run build.sh first."
    exit 1
fi