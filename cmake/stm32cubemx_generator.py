import subprocess
import sys
import threading
import time
import itertools

def animate(stop_event):
    """Displays a Braille spinner animation in the console."""
    for char in itertools.cycle("⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏"):
        if stop_event.is_set():
            break
        # Use \r to return to the beginning of the line
        sys.stdout.write(f"\r{char} Generating files... ")
        sys.stdout.flush()
        time.sleep(0.1)
    # Clear the line at the end
    sys.stdout.write("\r" + " " * 30 + "\r")

def main():
    # The command to run is passed as arguments to this script
    command_to_run = sys.argv[1:]
    if not command_to_run:
        print("Usage: python run_and_animate.py <command> [args...]")
        sys.exit(1)

    stop_animation = threading.Event()
    animation_thread = threading.Thread(target=animate, args=(stop_animation,))
    animation_thread.start()

    result = None
    try:
        # Run the actual command, hiding its output
        result = subprocess.run(
            command_to_run,
            capture_output=True,
            text=True
        )
        result.check_returncode() # Raise an exception if the command failed
    except subprocess.CalledProcessError:
        # Stop the animation before printing the error
        stop_animation.set()
        animation_thread.join()
        print("\n--- Command failed! ---")
        print("STDOUT:")
        print(result.stdout)
        print("\nSTDERR:")
        print(result.stderr)
        sys.exit(result.returncode)
    except FileNotFoundError:
        stop_animation.set()
        animation_thread.join()
        print(f"\nError: Command not found - '{command_to_run[0]}'")
        sys.exit(1)
    finally:
        # Ensure the animation thread is always stopped
        stop_animation.set()
        animation_thread.join()

if __name__ == "__main__":
    main()