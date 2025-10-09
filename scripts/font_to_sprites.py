#!/usr/bin/env python3
"""
Font to Sprite Generator

This script takes an OTF font file and generates individual PNG images
for each specified character. Useful for creating game sprite assets.

Usage:
    python font_to_sprites.py <font_file> <characters> <output_dir> <width> <height>

Example:
    python font_to_sprites.py assets/fonts/r-type.otf "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" assets/sprites/letters 32 32
"""

import sys
import os
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont


def generate_character_sprite(font_path, character, output_path, width, height):
    """
    Generate a PNG image for a single character using the specified font.
    
    Args:
        font_path (str): Path to the OTF/TTF font file
        character (str): The character to render
        output_path (str): Output path for the PNG file
        width (int): Width of the output image in pixels
        height (int): Height of the output image in pixels
    
    Returns:
        bool: True if successful, False otherwise
    """
    try:
        # Create a new RGBA image (transparent background)
        image = Image.new('RGBA', (width, height), (0, 0, 0, 0))
        draw = ImageDraw.Draw(image)
        
        # Load the font - try to fit the character in the image
        # Start with a font size slightly smaller than the image height
        font_size = int(height * 0.8)
        font = ImageFont.truetype(font_path, font_size)
        
        # Get the bounding box of the character
        bbox = draw.textbbox((0, 0), character, font=font)
        text_width = bbox[2] - bbox[0]
        text_height = bbox[3] - bbox[1]
        
        # Adjust font size if character is too large
        while (text_width > width * 0.9 or text_height > height * 0.9) and font_size > 1:
            font_size -= 1
            font = ImageFont.truetype(font_path, font_size)
            bbox = draw.textbbox((0, 0), character, font=font)
            text_width = bbox[2] - bbox[0]
            text_height = bbox[3] - bbox[1]
        
        # Center the character in the image
        x = (width - text_width) // 2 - bbox[0]
        y = (height - text_height) // 2 - bbox[1]
        
        # Draw the character in white
        draw.text((x, y), character, font=font, fill=(255, 255, 255, 255))
        
        # Save the image
        image.save(output_path, 'PNG')
        return True
        
    except Exception as e:
        print(f"✗ Error generating sprite for '{character}': {e}", file=sys.stderr)
        return False


def sanitize_filename(character):
    """
    Convert a character to a safe filename.
    
    Args:
        character (str): The character to sanitize
    
    Returns:
        str: Safe filename (without extension)
    """
    # Map special characters to readable names
    special_chars = {
        ' ': 'space',
        '!': 'exclamation',
        '"': 'quote',
        '#': 'hash',
        '$': 'dollar',
        '%': 'percent',
        '&': 'ampersand',
        "'": 'apostrophe',
        '(': 'lparen',
        ')': 'rparen',
        '*': 'asterisk',
        '+': 'plus',
        ',': 'comma',
        '-': 'minus',
        '.': 'period',
        '/': 'slash',
        ':': 'colon',
        ';': 'semicolon',
        '<': 'less',
        '=': 'equals',
        '>': 'greater',
        '?': 'question',
        '@': 'at',
        '[': 'lbracket',
        '\\': 'backslash',
        ']': 'rbracket',
        '^': 'caret',
        '_': 'underscore',
        '`': 'backtick',
        '{': 'lbrace',
        '|': 'pipe',
        '}': 'rbrace',
        '~': 'tilde',
    }
    
    if character in special_chars:
        return special_chars[character]
    
    # For alphanumeric characters, use them directly
    if character.isalnum():
        return character
    
    # For other characters, use their Unicode code point
    return f"U{ord(character):04X}"


def main():
    """Main entry point for the script."""
    
    # Parse command-line arguments
    if len(sys.argv) != 6:
        print("Usage: python font_to_sprites.py <font_file> <characters> <output_dir> <width> <height>")
        print("\nExample:")
        print('  python font_to_sprites.py assets/fonts/r-type.otf "ABCDEF" output 32 32')
        sys.exit(1)
    
    font_file = sys.argv[1]
    characters = sys.argv[2]
    output_dir = sys.argv[3]
    
    try:
        width = int(sys.argv[4])
        height = int(sys.argv[5])
    except ValueError:
        print("✗ Error: Width and height must be integers", file=sys.stderr)
        sys.exit(1)
    
    # Validate inputs
    if not os.path.isfile(font_file):
        print(f"✗ Error: Font file '{font_file}' not found", file=sys.stderr)
        sys.exit(1)
    
    if not font_file.lower().endswith(('.otf', '.ttf')):
        print(f"⚠ Warning: Font file should be .otf or .ttf format", file=sys.stderr)
    
    if width <= 0 or height <= 0:
        print("✗ Error: Width and height must be positive integers", file=sys.stderr)
        sys.exit(1)
    
    if not characters:
        print("✗ Error: No characters specified", file=sys.stderr)
        sys.exit(1)
    
    # Create output directory
    output_path = Path(output_dir)
    output_path.mkdir(parents=True, exist_ok=True)
    
    # Print configuration
    print("=" * 60)
    print("Font to Sprite Generator")
    print("=" * 60)
    print(f"Font File:     {font_file}")
    print(f"Characters:    {characters} ({len(characters)} total)")
    print(f"Output Dir:    {output_dir}")
    print(f"Dimensions:    {width}x{height} pixels")
    print("=" * 60)
    print()
    
    # Generate sprites
    success_count = 0
    fail_count = 0
    
    for char in characters:
        # Generate safe filename
        filename = sanitize_filename(char)
        output_file = output_path / f"{filename}.png"
        
        # Generate the sprite
        print(f"Generating '{char}' → {output_file.name}...", end=" ")
        
        if generate_character_sprite(font_file, char, str(output_file), width, height):
            print("✓")
            success_count += 1
        else:
            print("✗")
            fail_count += 1
    
    # Print summary
    print()
    print("=" * 60)
    print("Generation Summary")
    print("=" * 60)
    print(f"✓ Successful: {success_count}")
    print(f"✗ Failed:     {fail_count}")
    print(f"Total:        {len(characters)}")
    print("=" * 60)
    
    # Exit with appropriate code
    sys.exit(0 if fail_count == 0 else 1)


if __name__ == "__main__":
    main()
