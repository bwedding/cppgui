const fs = require('fs');
const path = require('path');

// Check if template file exists and show its content
const templatePath = path.join('C:', 'Users', 'bruce', 'source', 'cppgui', 'HandlerTemplate.cpp.hbs');

try {
    console.log(`Checking template file at: ${templatePath}`);
    
    if (fs.existsSync(templatePath)) {
        console.log('✅ Template file exists!');
        const content = fs.readFileSync(templatePath, 'utf8');
        
        console.log('\nTemplate content preview (first 500 chars):');
        console.log('----------------------------------------');
        console.log(content.substring(0, 500));
        console.log('----------------------------------------');
        
        // Check for potential issues with the template
        if (content.length < 50) {
            console.log('⚠️ Warning: Template file appears to be very short!');
        }
        
        if (!content.includes('{{#each inputs}}')) {
            console.log('⚠️ Warning: Template doesn\'t contain "{{#each inputs}}" tag!');
        }
        
        // Check compact JSON file
        const inputPath = path.join(__dirname, '..', 'UI-Metadata', 'ui-elements-compact.json');
        if (fs.existsSync(inputPath)) {
            console.log('\n✅ Compact JSON file exists!');
            const jsonData = fs.readFileSync(inputPath, 'utf8');
            
            try {
                const parsedData = JSON.parse(jsonData);
                
                console.log('\nCompact JSON content:');
                console.log('----------------------------------------');
                console.log(JSON.stringify(parsedData, null, 2));
                console.log('----------------------------------------');
                
                if (!parsedData.inputs || !Array.isArray(parsedData.inputs)) {
                    console.log('⚠️ Warning: JSON doesn\'t have valid inputs array!');
                } else if (parsedData.inputs.length === 0) {
                    console.log('⚠️ Warning: inputs array is empty!');
                } else {
                    console.log(`\n✅ Found ${parsedData.inputs.length} inputs in the JSON file.`);
                }
                
            } catch (error) {
                console.error('❌ Error parsing JSON:', error.message);
            }
        } else {
            console.error('❌ Compact JSON file does not exist at:', inputPath);
        }
        
    } else {
        console.error('❌ Template file does not exist!');
    }
} catch (error) {
    console.error('❌ Error checking files:', error.message);
}