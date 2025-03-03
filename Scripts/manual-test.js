const Handlebars = require('handlebars');
const fs = require('fs');
const path = require('path');

// Manually create a template and test data to verify Handlebars is working
const manualTemplate = `// AUTO-GENERATED - DO NOT MODIFY
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
            {{#each inputs}}
            {{#if @last}}
            {"{{id}}", [this] { Handle{{pascalCase id}}(); }}
            {{else}}
            {"{{id}}", [this] { Handle{{pascalCase id}}(); }},
            {{/if}}
            {{/each}}
        };

        if (auto it = handlers.find(elementId); it != handlers.end()) {
            it->second();
        } else {
            LogError("Unknown element:", elementId);
        }
    }

    {{#each inputs}}
    virtual void Handle{{pascalCase id}}() {
        throw NotImplementedException("Handle{{pascalCase id}}");
    }
    {{/each}}

private:
    void LogError(const std::string& message, const std::string& elementId) {
        // Add your logging implementation here
    }
};`;

// Add a safe Pascal Case converter that handles undefined/null values
Handlebars.registerHelper('pascalCase', (str) => {
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
});

// Define test data
const testData = {
    inputs: [
        { type: "button", id: "`change-heartrate-${action}`", event: "click" },
        { type: "button", id: "auto-manual-control", event: "click" }
    ]
};

try {
    console.log("Compiling manual template...");
    const template = Handlebars.compile(manualTemplate);
    
    console.log("Applying test data...");
    const result = template(testData);
    
    console.log("\nGenerated content (first 500 chars):");
    console.log("----------------------------------------");
    console.log(result.substring(0, 500));
    console.log("----------------------------------------");
    
    console.log("\nGenerated content (last 500 chars):");
    console.log("----------------------------------------");
    console.log(result.substring(result.length - 500));
    console.log("----------------------------------------");
    
    // Now try with the actual files
    console.log("\nTrying with actual files...");
    
    const templatePath = path.join('C:', 'Users', 'bruce', 'source', 'cppgui', 'HandlerTemplate.cpp.hbs');
    const inputPath = path.join(__dirname, '..', 'UI-Metadata', 'ui-elements-compact.json');
    
    // Read and parse actual files
    const templateContent = fs.readFileSync(templatePath, 'utf8');
    const inputData = JSON.parse(fs.readFileSync(inputPath, 'utf8'));
    
    console.log("Compiling actual template...");
    const actualTemplate = Handlebars.compile(templateContent);
    
    console.log("Applying actual data...");
    const actualResult = actualTemplate(inputData);
    
    console.log("\nActual generated content (first 500 chars):");
    console.log("----------------------------------------");
    console.log(actualResult.substring(0, 500));
    console.log("----------------------------------------");
    
    console.log("\nActual generated content (last 500 chars):");
    console.log("----------------------------------------");
    console.log(actualResult.substring(actualResult.length - 500));
    console.log("----------------------------------------");
    
    console.log("\nDone testing. Check results above for any issues.");
    
} catch (error) {
    console.error("Error during manual test:", error.message);
}