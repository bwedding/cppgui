const fs = require('fs');
const path = require('path');

// This script converts the standard extractor output to the compact format
// expected by the handler generator

const inputPath = path.join(__dirname, '..', 'UI-Metadata', 'ui-elements.json');
const outputPath = path.join(__dirname, '..', 'UI-Metadata', 'ui-elements-compact.json');

try {
    // Read the JSON file
    console.log(`Reading from: ${inputPath}`);
    const data = JSON.parse(fs.readFileSync(inputPath, 'utf8'));
    
    // Check if it's already in compact format
    if (data.inputs && Array.isArray(data.inputs)) {
        console.log('File already in compact format, no conversion needed.');
        process.exit(0);
    }
    
    // Check if it's in standard format with elements array
    if (!data.elements || !Array.isArray(data.elements)) {
        console.error('Error: Input file is not in expected format (missing elements array)');
        process.exit(1);
    }
    
    // Convert to compact format
    console.log('Converting to compact format...');
    const inputs = data.elements
        .filter(el => el.attributes && el.attributes['data-backend'])
        .map(element => {
            // Determine element type (lowercase)
            let elementType = element.name.toLowerCase();
            
            // Determine ID (use id if available, otherwise use name, or null if neither exists)
            let elementId = element.attributes.id || element.attributes.name || null;
            
            // Determine event type based on element type
            let eventType;
            if (elementType.includes('button')) {
                eventType = 'click';
            } else if (elementType.includes('check') || elementType.includes('radio') || 
                     elementType.includes('select') || elementType.includes('switch') ||
                     elementType.includes('slider')) {
                eventType = 'change';
            } else if (elementType.includes('input') || elementType.includes('text')) {
                eventType = 'input';
            } else {
                eventType = 'change'; // Default event type
            }
            
            return {
                type: elementType,
                id: elementId,
                event: eventType
            };
        });
    
    // Create compact format
    const compactData = { inputs };
    
    // Write to output file
    console.log(`Writing to: ${outputPath}`);
    fs.writeFileSync(outputPath, JSON.stringify(compactData, null, 2));
    
    console.log(`Conversion successful. Found ${inputs.length} input elements.`);
} catch (error) {
    console.error('Error:', error.message);
    process.exit(1);
}