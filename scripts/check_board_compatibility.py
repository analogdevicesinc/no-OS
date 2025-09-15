#!/usr/bin/env python3
"""
Board Compatibility Checker

This script helps manage and validate board compatibility across all projects.
Usage:
    python3 scripts/check_board_compatibility.py --list-matrix
    python3 scripts/check_board_compatibility.py --validate-project ftc_workshop max32650_fthr
    python3 scripts/check_board_compatibility.py --generate-docs
"""

import argparse
import os
import sys
from pathlib import Path
from typing import Dict, List, Set

class BoardCompatibilityChecker:
    def __init__(self, no_os_root: Path):
        self.no_os_root = Path(no_os_root)
        self.projects_dir = self.no_os_root / "projects"

    def load_project_boards(self, project_name: str) -> List[str]:
        """Load supported boards for a project from its boards/ directory."""
        boards_dir = self.projects_dir / project_name / "boards"

        if not boards_dir.exists() or not boards_dir.is_dir():
            return []

        boards = []
        for board_file in boards_dir.glob("*.conf"):
            board_name = board_file.stem
            boards.append(board_name)
        return sorted(boards)

    def get_all_projects(self) -> List[str]:
        """Get all project directories."""
        projects = []
        for item in self.projects_dir.iterdir():
            if item.is_dir() and item.name != '__pycache__':
                projects.append(item.name)
        return sorted(projects)

    def get_all_boards(self) -> Set[str]:
        """Get all unique board names across all projects."""
        boards = set()
        for project in self.get_all_projects():
            project_boards = self.load_project_boards(project)
            boards.update(project_boards)
        return boards

    def generate_compatibility_matrix(self) -> Dict[str, List[str]]:
        """Generate the complete compatibility matrix."""
        matrix = {}
        for project in self.get_all_projects():
            matrix[project] = self.load_project_boards(project)
        return matrix

    def validate_project_board(self, project: str, board: str) -> bool:
        """Check if a project supports a specific board."""
        supported_boards = self.load_project_boards(project)
        return board in supported_boards

    def print_compatibility_matrix(self):
        """Print the full compatibility matrix."""
        matrix = self.generate_compatibility_matrix()
        all_boards = sorted(self.get_all_boards())

        print("📋 Board Compatibility Matrix")
        print("=" * 80)
        print()

        # Header
        print(f"{'Project':<25} | {'Supported Boards'}")
        print("-" * 25 + "-|-" + "-" * 50)

        for project in sorted(matrix.keys()):
            boards = matrix[project]
            if boards:
                boards_str = ", ".join(boards)
                print(f"{project:<25} | {boards_str}")
            else:
                print(f"{project:<25} | ⚠️  No boards.conf (accepts any)")

        print()
        print(f"Available boards: {', '.join(all_boards)}")
        print(f"Total projects: {len(matrix)}")
        print(f"Projects with restrictions: {len([p for p, b in matrix.items() if b])}")

    def generate_markdown_docs(self) -> str:
        """Generate markdown documentation for the compatibility matrix."""
        matrix = self.generate_compatibility_matrix()
        all_boards = sorted(self.get_all_boards())

        md = ["# Board Compatibility Matrix", ""]
        md.append("This matrix shows which boards are supported by each project.")
        md.append("Generated automatically from `projects/*/boards.conf` files.")
        md.append("")

        # Summary table
        md.append("## Summary")
        md.append("")
        md.append("| Project | Supported Boards | Count |")
        md.append("|---------|------------------|-------|")

        for project in sorted(matrix.keys()):
            boards = matrix[project]
            if boards:
                boards_str = ", ".join(boards)
                md.append(f"| {project} | {boards_str} | {len(boards)} |")
            else:
                md.append(f"| {project} | ⚠️ Any (no restrictions) | ∞ |")

        md.append("")

        # Board support table
        md.append("## Board Support Matrix")
        md.append("")
        md.append("| Board | Supported Projects |")
        md.append("|-------|--------------------|")

        for board in all_boards:
            supporting_projects = [p for p, boards in matrix.items() if board in boards]
            if supporting_projects:
                projects_str = ", ".join(supporting_projects)
                md.append(f"| {board} | {projects_str} |")

        md.append("")
        md.append("## Usage")
        md.append("")
        md.append("```bash")
        md.append("# Valid combinations")

        # Show some examples
        for project in list(sorted(matrix.keys()))[:3]:
            boards = matrix[project]
            if boards:
                md.append(f"cmake --preset {boards[0]} -DPROJECT_NAME={project}")

        md.append("")
        md.append("# Invalid combinations will fail with clear error messages")
        md.append("cmake --preset stm32f756_nucleo -DPROJECT_NAME=ftc_workshop  # Fails")
        md.append("```")

        return "\n".join(md)

def main():
    parser = argparse.ArgumentParser(description="Board Compatibility Checker")
    parser.add_argument("--no-os-root", default=".", help="Path to no-OS repository root")
    parser.add_argument("--list-matrix", action="store_true", help="Print compatibility matrix")
    parser.add_argument("--validate-project", nargs=2, metavar=("PROJECT", "BOARD"),
                       help="Validate if PROJECT supports BOARD")
    parser.add_argument("--generate-docs", action="store_true", help="Generate markdown documentation")

    args = parser.parse_args()

    checker = BoardCompatibilityChecker(args.no_os_root)

    if args.list_matrix:
        checker.print_compatibility_matrix()

    if args.validate_project:
        project, board = args.validate_project
        is_valid = checker.validate_project_board(project, board)
        if is_valid:
            print(f"✅ {project} supports {board}")
            sys.exit(0)
        else:
            supported = checker.load_project_boards(project)
            print(f"❌ {project} does not support {board}")
            if supported:
                print(f"Supported boards: {', '.join(supported)}")
            else:
                print("No boards.conf file found - project accepts any board")
            sys.exit(1)

    if args.generate_docs:
        docs = checker.generate_markdown_docs()
        output_file = Path(args.no_os_root) / "BOARD_COMPATIBILITY_GENERATED.md"
        with open(output_file, 'w') as f:
            f.write(docs)
        print(f"Generated documentation: {output_file}")

    if not any([args.list_matrix, args.validate_project, args.generate_docs]):
        parser.print_help()

if __name__ == "__main__":
    main()