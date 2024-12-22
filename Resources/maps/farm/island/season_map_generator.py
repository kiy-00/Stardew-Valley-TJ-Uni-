import os
import sys
import xml.etree.ElementTree as ET
from shutil import copy2

def generate_tsx_file(source_tsx, target_season):
    """
    Generate a new TSX file for the target season
    
    Args:
        source_tsx (str): Path to source TSX file
        target_season (str): Target season ('spring', 'summer', 'fall', 'winter')
    Returns:
        str: Path to new TSX file
    """
    # Create new tsx filename
    source_dir = os.path.dirname(source_tsx)
    source_filename = os.path.basename(source_tsx)
    new_filename = source_filename.replace('spring', target_season)
    new_tsx_path = os.path.join(source_dir, new_filename)
    
    # Parse source tsx
    tree = ET.parse(source_tsx)
    root = tree.getroot()
    
    # Update image source
    image_elem = root.find('image')
    if image_elem is not None and 'source' in image_elem.attrib:
        source = image_elem.attrib['source']
        new_source = source.replace('spring', target_season)
        image_elem.attrib['source'] = new_source
    
    # Update tileset name
    if 'name' in root.attrib:
        root.attrib['name'] = root.attrib['name'].replace('spring', target_season)
    
    # Write tsx file, overwriting if exists
    tree.write(new_tsx_path, encoding='UTF-8', xml_declaration=True)
    print(f"Generated TSX file: {new_tsx_path}")
    return new_filename

def generate_season_map(source_map, target_season):
    """
    Generate a new season map and its required TSX files
    
    Args:
        source_map (str): Path to source TMX file
        target_season (str): Target season ('spring', 'summer', 'fall', 'winter')
    """
    # Validate season
    valid_seasons = ['spring', 'summer', 'fall', 'winter']
    if target_season not in valid_seasons:
        raise ValueError(f"Invalid season. Must be one of {valid_seasons}")
    
    # Create new map filename
    source_dir = os.path.dirname(source_map)
    source_filename = os.path.basename(source_map)
    new_filename = source_filename.replace('spring', target_season)
    new_map_path = os.path.join(source_dir, new_filename)
    
    # Parse source map
    tree = ET.parse(source_map)
    root = tree.getroot()
    
    # Process each tileset and generate corresponding TSX files
    for tileset in root.findall('tileset'):
        if 'source' in tileset.attrib:
            source = tileset.attrib['source']
            if '_spring.' in source:
                # Generate new TSX file
                source_tsx_path = os.path.join(source_dir, source)
                if os.path.exists(source_tsx_path):
                    new_tsx_filename = generate_tsx_file(source_tsx_path, target_season)
                    tileset.attrib['source'] = new_tsx_filename
    
    # Write map file, overwriting if exists
    tree.write(new_map_path, encoding='UTF-8', xml_declaration=True)
    print(f"Generated TMX file: {new_map_path}")

def main():
    if len(sys.argv) != 3:
        print("Usage: python script.py <source_map.tmx> <target_season>")
        print("Example: python script.py island_spring.tmx summer")
        sys.exit(1)
    
    source_map = sys.argv[1]
    target_season = sys.argv[2].lower()
    
    try:
        generate_season_map(source_map, target_season)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()