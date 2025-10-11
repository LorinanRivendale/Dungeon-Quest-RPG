---

## setup_git.sh
```bash
#!/bin/bash

# setup_git.sh - Git repository initialization script

echo "================================"
echo "  Git Repository Setup"
echo "================================"
echo ""

# Check if git is installed
if ! command -v git &> /dev/null; then
    echo "ERROR: Git is not installed!"
    echo "Please install git first: https://git-scm.com/downloads"
    exit 1
fi

# Check if already a git repository
if [ -d ".git" ]; then
    echo "WARNING: This directory is already a git repository!"
    read -p "Do you want to continue anyway? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

echo "Initializing git repository..."
git init

echo "Creating .gitignore..."
cat > .gitignore << 'EOF'
# Compiled Object files
*.o
*.obj
*.ko
*.elf

# Executables
*.exe
*.out
*.app
rpg_game
rpg_game.exe

# Build directories
obj/
build/
dist/

# Debug files
*.dSYM/
*.su
*.idb
*.pdb

# Linker output
*.ilk
*.map
*.exp

# Precompiled Headers
*.gch
*.pch

# Libraries
*.lib
*.a
*.la
*.lo

# Shared objects
*.dll
*.so
*.so.*
*.dylib

# Editor files
*.swp
*.swo
*~
.vscode/
.idea/
*.sublime-project
*.sublime-workspace

# OS files
.DS_Store
Thumbs.db
Desktop.ini

# Backup files
*.bak
*.backup
*~

# Test files
test_output/
*.log

# Game save files
*.sav
*.dat
EOF

echo "Creating .gitattributes..."
cat > .gitattributes << 'EOF'
# Auto detect text files and normalize line endings to LF
* text=auto

# Source code
*.c text eol=lf
*.h text eol=lf
*.cpp text eol=lf
*.hpp text eol=lf

# Scripts
*.sh text eol=lf
*.bash text eol=lf
Makefile text eol=lf

# Documentation
*.md text eol=lf
*.txt text eol=lf
LICENSE text eol=lf

# Windows specific files
*.bat text eol=crlf
*.cmd text eol=crlf

# Binary files
*.gb binary
*.gbc binary
*.rom binary
*.bin binary
*.png binary
*.jpg binary
*.jpeg binary
*.gif binary
EOF

# Configure git user if not already configured
if [ -z "$(git config user.name)" ]; then
    echo ""
    echo "Git user not configured. Let's set it up:"
    read -p "Enter your name: " git_name
    read -p "Enter your email: " git_email
    git config user.name "$git_name"
    git config user.email "$git_email"
    echo "Git user configured locally for this repository."
fi

echo ""
echo "Adding files to git..."
git add .

echo ""
echo "Creating initial commit..."
git commit -m "Initial commit: RPG framework with dungeon exploration, battle system, and party management

Features:
- 6 job classes (Knight, Black Belt, Thief, Sage, Priest, Mage)
- 4 dungeons + final dungeon
- Turn-based battle system
- Inventory management
- Experience and leveling
- Boss battles with key items
- Designed for eventual GameBoy port"

echo ""
echo "================================"
echo "  Setup Complete!"
echo "================================"
echo ""
echo "Your git repository has been initialized."
echo ""
echo "Current status:"
git status
echo ""
echo "To push to GitHub:"
echo "  1. Create a new repository on GitHub"
echo "  2. Run: git remote add origin YOUR_GITHUB_URL"
echo "  3. Run: git branch -M main"
echo "  4. Run: git push -u origin main"
echo ""
echo "To see commit history:"
echo "  git log"
echo ""