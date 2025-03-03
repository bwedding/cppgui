const fs = require('fs');
const path = require('path');

/**
 * This script directly generates an InputHandler.h file without using templates
 * to avoid any issues with template processing
 */

// Define paths
const inputPath = path.join(__dirname, '..', 'UI-Metadata', 'ui-elements-compact.json');
const outputPath = path.join(__dirname, '..', 'Generated', 'InputHandler.h');

// Create directory if needed
if (!fs.existsSync(path.dirname(outputPath))) {
    fs.mkdirSync(path.dirname(outputPath), { recursive: true });
}

// Helper function to convert string to PascalCase
function toPascalCase(str) {
    if (!str || typeof str !== 'string') {
        console.warn(`Warning: Tried to convert non-string value to Pascal case: ${str}`);
        return '';
    }
    
    // For IDs with template literals, extract the meaningful part for the method name
    let processedStr = str;
    
    // Handle backtick-wrapped template literals like `change-heartrate-${action}`
    if (str.startsWith('`') && str.endsWith('`')) {
        // Remove the backticks and extract the part before any template expression
        processedStr = str.slice(1, -1).split('${')[0].trim();
    }
    
    return processedStr.replace(/(?:^\w|[A-Z]|\b\w)/g, word => word.toUpperCase())
                       .replace(/\W+/g, '');
}

// Helper function to format string literal
function formatStringLiteral(str) {
    if (!str || typeof str !== 'string') {
        return '""';
    }
    
    // If the string is a template literal (has backticks), replace with actual string
    if (str.startsWith('`') && str.endsWith('`')) {
        const inner = str.slice(1, -1);
        return `"${inner}"`;
    }
    
    // Regular string - just add quotes
    return `"${str}"`;
}

try {
    // Read and validate JSON input
    console.log(`Reading UI elements from: ${inputPath}`);
    const jsonData = fs.readFileSync(inputPath, 'utf8');
    const inputs = JSON.parse(jsonData);
    
    if (!inputs.inputs || !Array.isArray(inputs.inputs)) {
        throw new Error("Invalid JSON format - missing inputs array");
    }
    
    console.log(`Found ${inputs.inputs.length} input elements.`);
    
    // Generate handler entries
    const handlerEntries = inputs.inputs.map((input, index) => {
        const isLast = index === inputs.inputs.length - 1;
        return `        {${formatStringLiteral(input.id)}, [this] { Handle${toPascalCase(input.id)}(); }}${isLast ? '' : ','}`;
    }).join('\n');
    
    // Generate handler methods
    const handlerMethods = inputs.inputs.map(input => {
        return `    virtual void Handle${toPascalCase(input.id)}() {\n        throw NotImplementedException("Handle${toPascalCase(input.id)}");\n    }`;
    }).join('\n');
    
    // Build the output file content
    const outputContent = `// AUTO-GENERATED - DO NOT MODIFY
#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <stdexcept>

// Custom exception for unimplemented handlers
class NotImplementedException : public std::runtime_error {
public:
    NotImplementedException(const std::string& handlerName) 
        : std::runtime_error("Handler not implemented: " + handlerName) {}
};

class InputHandler {
public:
    using HandlerFunc = std::function<void()>;

    void HandleEvent(const std::string& elementId) {
        static const std::unordered_map<std::string, HandlerFunc> handlers = {
${handlerEntries}
        };

        if (auto it = handlers.find(elementId); it != handlers.end()) {
            it->second();
        } else {
            LogError("Unknown element:", elementId);
        }
    }

${handlerMethods}

private:
    void LogError(const std::string& message, const std::string& elementId) {
        // Add your logging implementation here
    }
};
`;
    
    // Write output
    console.log(`Writing output to: ${outputPath}`);
    fs.writeFileSync(outputPath, outputContent);
    
    console.log('Handler generation completed successfully!');
} catch (e) {
    console.error("Error:", e.message);
    process.exit(1);
}