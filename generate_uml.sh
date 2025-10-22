#!/bin/bash

echo "==================================="
echo "R-Type UML Diagram Generation"
echo "==================================="

# Check if PlantUML is installed
if ! command -v plantuml &> /dev/null; then
    echo "❌ Error: PlantUML is not installed."
    echo ""
    echo "Please install PlantUML:"
    echo "  Ubuntu/Debian: sudo apt-get install plantuml"
    echo "  macOS: brew install plantuml"
    echo "  Windows: Download from https://plantuml.com/"
    exit 1
fi

echo "✅ PlantUML found"
echo ""

# Create output directory
OUTPUT_DIR="archi_mindmap"
echo "📁 Output directory: $OUTPUT_DIR"
echo ""

echo "🧹 Cleaning previous PNG files..."
if ls $OUTPUT_DIR/*.png 1> /dev/null 2>&1; then
    rm $OUTPUT_DIR/*.png
    echo "   ✅ Previous PNG files removed"
else
    echo "   ℹ️  No previous PNG files to remove"
fi
echo ""

echo "🎨 Generating UML diagrams..."
echo ""

# Discover and generate all PlantUML files
FILES=$(ls archi_mindmap/*.puml 2>/dev/null)
if [ -z "$FILES" ]; then
    echo "❌ No PlantUML files found in archi_mindmap/ directory"
    exit 1
fi

DIAGRAM_COUNT=1
GENERATED_FILES=()

for FILE in $FILES; do
    FILENAME=$(basename "$FILE" .puml)
    
    # Get emoji and description based on filename
    case "$FILENAME" in
        "architecture")
            EMOJI="📋"
            DESCRIPTION="Architecture Class Diagram"
            ;;
        "sequence")
            EMOJI="🔄"
            DESCRIPTION="Sequence Diagram"
            ;;
        "components")
            EMOJI="🧩"
            DESCRIPTION="Component Diagram"
            ;;
        *)
            EMOJI="📊"
            DESCRIPTION="$(echo $FILENAME | sed 's/.*/\u&/') Diagram"
            ;;
    esac
    
    echo "${DIAGRAM_COUNT}️⃣  Generating $DESCRIPTION..."
    
    if plantuml -tpng "$FILE"; then
        echo "   ✅ $FILENAME diagram: archi_mindmap/$FILENAME.png"
        GENERATED_FILES+=("archi_mindmap/$FILENAME.png")
    else
        echo "   ❌ Failed to generate $FILENAME diagram"
    fi
    
    ((DIAGRAM_COUNT++))
done

echo ""
echo "✅ Diagram generation complete!"
echo ""
echo "📊 Generated files:"

# Display generated files dynamically
for generated_file in "${GENERATED_FILES[@]}"; do
    filename=$(basename "$generated_file" .png)
    case "$filename" in
        "architecture")
            echo "   📋 Architecture (Class Diagram):"
            ;;
        "sequence")
            echo "   🔄 Communication (Sequence Diagram):"
            ;;
        "components")
            echo "   🧩 Modules (Component Diagram):"
            ;;
        *)
            echo "   📊 $(echo $filename | sed 's/.*/\u&/') Diagram:"
            ;;
    esac
    echo "      - $generated_file"
    echo ""
done
echo "💡 How to view:"
echo "   • Open PNG files in any image viewer"
echo "   • Use VS Code PlantUML extension to edit .puml files"
echo ""
echo "🎯 Project Documentation:"
echo "   • archi_mindmap/README.md - Complete architecture documentation"
echo "   • archi_mindmap/SUMMARY.md - Files summary"
echo ""