// JSX/TSX/JS Element Extractor
// This tool parses React files to extract UI components like buttons, inputs, etc.

const fs = require('fs');
const path = require('path');
const parser = require('@babel/parser');
const traverse = require('@babel/traverse').default;

// Configuration
const targetElements = {
  // Native HTML elements
  button: { type: 'Button', attributes: ['onClick', 'disabled', 'type', 'data-backend', 'eventMetadata'] },
  input: { type: 'Input', attributes: ['type', 'placeholder', 'value', 'onChange', 'disabled', 'required', 'min', 'max', 'step', 'pattern', 'accept', 'src', 'alt', 'data-backend', 'eventMetadata'] },
  select: { type: 'Select', attributes: ['value', 'onChange', 'disabled', 'multiple', 'data-backend', 'eventMetadata'] },
  textarea: { type: 'Textarea', attributes: ['value', 'onChange', 'placeholder', 'rows', 'cols', 'data-backend', 'eventMetadata'] },
  form: { type: 'Form', attributes: ['onSubmit', 'action', 'method', 'data-backend', 'eventMetadata'] },
  
  // Common React component names (case-insensitive matching will be applied)
  Button: { type: 'Button', attributes: ['onClick', 'disabled', 'variant', 'size', 'data-backend', 'eventMetadata'] },
  Input: { type: 'Input', attributes: ['type', 'value', 'onChange', 'placeholder', 'data-backend', 'eventMetadata'] },
  TextField: { type: 'Input', attributes: ['value', 'onChange', 'label', 'variant', 'data-backend', 'eventMetadata'] },
  Select: { type: 'Select', attributes: ['value', 'onChange', 'options', 'data-backend', 'eventMetadata'] },
  Checkbox: { type: 'Checkbox', attributes: ['checked', 'onChange', 'label', 'data-backend', 'eventMetadata'] },
  Radio: { type: 'Radio', attributes: ['checked', 'onChange', 'value', 'name', 'data-backend', 'eventMetadata'] },
  Switch: { type: 'Switch', attributes: ['checked', 'onChange', 'data-backend', 'eventMetadata'] },
  Dropdown: { type: 'Dropdown', attributes: ['value', 'options', 'onChange', 'data-backend', 'eventMetadata'] },
  Form: { type: 'Form', attributes: ['onSubmit', 'initialValues', 'data-backend', 'eventMetadata'] },
};

// Parse options for different file types
const getParserOptions = (filePath) => {
  const ext = path.extname(filePath);
  const baseOptions = {
    sourceType: 'module',
    plugins: ['jsx']
  };
  
  if (ext === '.ts' || ext === '.tsx') {
    baseOptions.plugins.push('typescript');
  }
  
  // Add additional parser plugins for modern JS features
  baseOptions.plugins.push(
    'classProperties',
    'decorators-legacy',
    'objectRestSpread',
    'optionalChaining',
    'nullishCoalescingOperator'
  );
  
  return baseOptions;
};

// Extract elements from a single file
const extractElementsFromFile = (filePath, options = {}) => {
  try {
    const fileContent = fs.readFileSync(filePath, 'utf-8');
    const parserOptions = getParserOptions(filePath);
    
    const ast = parser.parse(fileContent, parserOptions);
    const elements = [];
    
    traverse(ast, {
      JSXOpeningElement(path) {
        const elementName = path.node.name.name || 
                           (path.node.name.property && path.node.name.property.name);
        
        // Check if this element is one we're looking for
        const targetElement = findTargetElement(elementName);
        if (!targetElement) return;
        
        const attributes = {};
        let elementText = '';
        let inputType = '';
        
        // Get the entire JSX element as text for deeper parsing if needed
        let elementStartPos = path.node.start;
        let elementEndPos = null;
        
        // Try to find the end of the entire JSX element (includes closing tag)
        const parentPath = path.parentPath;
        if (parentPath && parentPath.node.type === 'JSXElement' && parentPath.node.end) {
          elementEndPos = parentPath.node.end;
        }
        
        // Get the element source code if we found both start and end positions
        const fullElementText = (elementStartPos !== undefined && elementEndPos !== undefined) 
          ? fileContent.substring(elementStartPos, elementEndPos)
          : '';
        
        // Extract attributes
        path.node.attributes.forEach(attr => {
          if (attr.type === 'JSXAttribute') {
            const attrName = attr.name.name;
            
            // Skip className attribute if includeClassNames is not enabled
            if (attrName === 'className' && !options.includeClassNames) {
              return;
            }
            
            let attrValue = '';
            
            if (attr.value) {
              if (attr.value.type === 'StringLiteral') {
                attrValue = attr.value.value;
              } else if (attr.value.type === 'JSXExpressionContainer') {
                // Extract the actual code for the expression
                const expressionStart = attr.value.start;
                const expressionEnd = attr.value.end;
                
                // Get the source code for this expression
                if (expressionStart !== undefined && expressionEnd !== undefined) {
                  // +1 and -1 to remove the curly braces around the expression
                  const expressionCode = fileContent.substring(expressionStart + 1, expressionEnd - 1).trim();
                  attrValue = expressionCode;
                } else {
                  attrValue = '[Expression]';
                }
              }
            } else {
              // Boolean attribute (like disabled)
              attrValue = true;
            }
            
            attributes[attrName] = attrValue;
          }
        });
        
        // Try to find text content for this element
        const parent = path.parentPath;
        if (parent && parent.node.children) {
          parent.node.children.forEach(child => {
            if (child.type === 'JSXText') {
              elementText += child.value.trim();
            } else if (child.type === 'JSXExpressionContainer' && 
                      child.expression.type === 'StringLiteral') {
              elementText += child.expression.value;
            }
          });
        }
        
        elements.push({
          type: targetElement.type,
          name: elementName,
          attributes,
          text: elementText || undefined,
          location: {
            file: filePath,
            line: path.node.loc ? path.node.loc.start.line : undefined,
            column: path.node.loc ? path.node.loc.start.column : undefined
          }
        });
      }
    });
    
    return elements;
  } catch (error) {
    console.error(`Error processing file ${filePath}:`, error);
    return [];
  }
};

// Helper to find the appropriate target element (case-insensitive)
const findTargetElement = (elementName) => {
  if (!elementName) return null;
  
  // Direct match
  if (targetElements[elementName]) {
    return targetElements[elementName];
  }
  
  // Case-insensitive match for React components
  const lowerName = elementName.toLowerCase();
  for (const [key, value] of Object.entries(targetElements)) {
    if (key.toLowerCase() === lowerName) {
      return value;
    }
  }
  
  return null;
};

// Process all files in a directory and its subdirectories
const processDirectory = (dirPath, fileExtensions = ['.js', '.jsx', '.ts', '.tsx'], options = {}) => {
  const allElements = [];
  
  const processDir = (currentPath) => {
    const files = fs.readdirSync(currentPath);
    
    files.forEach(file => {
      const filePath = path.join(currentPath, file);
      const stat = fs.statSync(filePath);
      
      if (stat.isDirectory()) {
        // Skip node_modules and other common directories to ignore
        if (!['node_modules', 'dist', 'build', '.git'].includes(file)) {
          processDir(filePath);
        }
      } else if (fileExtensions.includes(path.extname(filePath))) {
        const elements = extractElementsFromFile(filePath, options);
        
        // Filter elements by data-backend if the option is enabled
        if (options.backendOnly) {
          const backendElements = elements.filter(el => el.attributes && el.attributes['data-backend']);
          allElements.push(...backendElements);
        } else {
          allElements.push(...elements);
        }
      }
    });
  };
  
  processDir(dirPath);
  return allElements;
};

// Generate a summary report
const generateReport = (elements) => {
  const summary = {
    totalElements: elements.length,
    byType: {},
    byFile: {},
    specialSections: {
      inputsMissingIdentifiers: [],
      elementsWithBackendData: [],
      elementsWithDefaultNames: []
    }
  };
  
  // Group by type
  elements.forEach(element => {
    if (!summary.byType[element.type]) {
      summary.byType[element.type] = 0;
    }
    summary.byType[element.type]++;
    
    // Group by file
    const filename = element.location.file;
    if (!summary.byFile[filename]) {
      summary.byFile[filename] = {};
    }
    
    if (!summary.byFile[filename][element.type]) {
      summary.byFile[filename][element.type] = 0;
    }
    summary.byFile[filename][element.type]++;
    
    // Special section: inputs missing name or id
    if ((element.type === 'Input' || element.type.startsWith('Input:')) && 
        (!element.attributes.name && !element.attributes.id)) {
      summary.specialSections.inputsMissingIdentifiers.push({
        type: element.type,
        location: element.location,
        attributes: element.attributes
      });
    }
    
    // Special section: elements with default names or no names
    const elementTypeName = element.name.toLowerCase();
    if (!element.attributes.name || element.attributes.name === elementTypeName) {
      // Check if the element type is worth flagging (buttons, inputs, etc.)
      const importantTypes = ['button', 'input', 'select', 'textarea', 'form', 'checkbox', 'radio'];
      if (importantTypes.includes(elementTypeName) || 
          importantTypes.some(type => element.type.toLowerCase().includes(type.toLowerCase()))) {
        summary.specialSections.elementsWithDefaultNames.push({
          type: element.type,
          name: element.name,
          location: element.location,
          attributes: element.attributes,
          issue: element.attributes.name ? 'Default type name used' : 'No name attribute'
        });
      }
    }
    
    // Special section: elements with data-backend attribute
    if (element.attributes['data-backend']) {
      summary.specialSections.elementsWithBackendData.push({
        type: element.type,
        name: element.name,
        location: element.location,
        backendData: element.attributes['data-backend'],
        attributes: element.attributes
      });
    }
  });
  
  return summary;
};

// Export results to JSON
const exportResults = (elements, outputPath, options = {}) => {
  const results = {
    elements,
    summary: generateReport(elements)
  };
  
  if (options.markdownOutput) {
    // Export as Markdown if requested
    const markdownContent = generateMarkdownReport(results);
    const markdownPath = outputPath.replace(/\.json$/, '.md');
    fs.writeFileSync(markdownPath, markdownContent);
    console.log(`Markdown results exported to ${markdownPath}`);
  } else {
    // Default JSON output
    fs.writeFileSync(outputPath, JSON.stringify(results, null, 2));
    console.log(`JSON results exported to ${outputPath}`);
  }
  
  return results;
};

// Generate a markdown report from the results
const generateMarkdownReport = (results) => {
  const { elements, summary } = results;
  
  let markdown = `# JSX Element Extraction Report\n\n`;
  
  // Add summary section
  markdown += `## Summary\n\n`;
  markdown += `- **Total UI Elements Found**: ${summary.totalElements}\n`;
  
  // Elements by type
  markdown += `\n## Elements by Type\n\n`;
  markdown += `| Element Type | Count |\n`;
  markdown += `|-------------|-------|\n`;
  
  Object.entries(summary.byType)
    .sort((a, b) => b[1] - a[1]) // Sort by count (descending)
    .forEach(([type, count]) => {
      markdown += `| ${type} | ${count} |\n`;
    });
  
  // Special Sections
  
  // Elements with missing/default names
  if (summary.specialSections.elementsWithDefaultNames.length > 0) {
    markdown += `\n## ⚠️ Elements with Missing or Default Names (${summary.specialSections.elementsWithDefaultNames.length})\n\n`;
    markdown += `| Type | Issue | Location | Line |\n`;
    markdown += `|------|-------|----------|------|\n`;
    
    summary.specialSections.elementsWithDefaultNames.forEach(element => {
      const filePath = element.location.file.split(/[\\/]/).slice(-2).join('/');
      markdown += `| ${element.type} | ${element.issue} | ${filePath} | ${element.location.line} |\n`;
    });
  }
  
  // Inputs missing identifiers
  if (summary.specialSections.inputsMissingIdentifiers.length > 0) {
    markdown += `\n## ⚠️ Inputs Missing Identifiers (${summary.specialSections.inputsMissingIdentifiers.length})\n\n`;
    markdown += `| Type | Location | Line |\n`;
    markdown += `|------|----------|------|\n`;
    
    summary.specialSections.inputsMissingIdentifiers.forEach(element => {
      const filePath = element.location.file.split(/[\\/]/).slice(-2).join('/');
      markdown += `| ${element.type} | ${filePath} | ${element.location.line} |\n`;
    });
  }
  
  // Elements with backend data
  if (summary.specialSections.elementsWithBackendData.length > 0) {
    markdown += `\n## 📊 Elements with Backend Data (${summary.specialSections.elementsWithBackendData.length})\n\n`;
    markdown += `| Type | Name | Backend Data | Event Metadata | Location | Line |\n`;
    markdown += `|------|------|-------------|----------------|----------|------|\n`;
    
    summary.specialSections.elementsWithBackendData.forEach(element => {
      const filePath = element.location.file.split(/[\\/]/).slice(-2).join('/');
      const elementName = element.attributes.name || '(unnamed)';
      const eventMetadata = element.eventMetadata ? 
        `\`${element.eventMetadata}\`` : 
        '';
      
      markdown += `| ${element.type} | ${elementName} | \`${element.backendData}\` | ${eventMetadata} | ${filePath} | ${element.location.line} |\n`;
    });
  }
  
  // Elements by file (could be very long, so making it optional or last)
  markdown += `\n## Elements by File\n\n`;
  
  Object.entries(summary.byFile).forEach(([file, types]) => {
    const filePath = file.split(/[\\/]/).slice(-2).join('/');
    markdown += `### ${filePath}\n\n`;
    
    const totalInFile = Object.values(types).reduce((sum, count) => sum + count, 0);
    markdown += `Total: ${totalInFile} elements\n\n`;
    
    markdown += `| Element Type | Count |\n`;
    markdown += `|-------------|-------|\n`;
    
    Object.entries(types)
      .sort((a, b) => b[1] - a[1]) // Sort by count (descending)
      .forEach(([type, count]) => {
        markdown += `| ${type} | ${count} |\n`;
      });
    
    markdown += `\n`;
  });
  
  return markdown;
};

// Main function
const extractElements = (targetPath, outputPath = './element-extraction-results.json', options = {}) => {
  console.log(`Scanning directory: ${targetPath}`);
  
  // Safety check for special flag paths
  if (targetPath.startsWith('--')) {
    throw new Error(`Invalid path: ${targetPath}. Did you mean to use this as a flag?`);
  }
  
  // Log active filters
  if (options.backendOnly) {
    console.log('Filter mode: Only elements with data-backend attribute');
  }
  if (options.includeClassNames) {
    console.log('Including className attributes in output');
  } else {
    console.log('Excluding className attributes from output (use --include-classnames to include them)');
  }
  if (options.markdownOutput) {
    console.log('Output format: Markdown');
  } else {
    console.log('Output format: JSON (use --markdown to output as Markdown)');
  }
  
  // Check if path exists
  try {
    const isDirectory = fs.statSync(targetPath).isDirectory();
    let elements = [];
    
    if (isDirectory) {
      elements = processDirectory(targetPath, ['.js', '.jsx', '.ts', '.tsx'], options);
    } else {
      const fileElements = extractElementsFromFile(targetPath, options);
      if (options.backendOnly) {
        elements = fileElements.filter(el => el.attributes && el.attributes['data-backend']);
      } else {
        elements = fileElements;
      }
    }
    
    console.log(`Found ${elements.length} UI elements`);
    
    // Generate report
    const report = generateReport(elements);
    
    // Log special section info
    if (report.specialSections.inputsMissingIdentifiers.length > 0) {
      console.log(`\n⚠️ Warning: Found ${report.specialSections.inputsMissingIdentifiers.length} inputs without name or id attributes`);
      console.log(`Check the report for details\n`);
    }
    
    if (report.specialSections.elementsWithDefaultNames.length > 0) {
      console.log(`\n⚠️ Warning: Found ${report.specialSections.elementsWithDefaultNames.length} elements with missing or default names`);
      console.log(`Check the report for details\n`);
    }
    
    if (report.specialSections.elementsWithBackendData.length > 0) {
      console.log(`\n📊 Found ${report.specialSections.elementsWithBackendData.length} elements with data-backend attribute`);
      console.log(`Check the report for details\n`);
    }
    
    // Export results
    exportResults(elements, outputPath, options);
    
    return elements;
  } catch (error) {
    if (error.code === 'ENOENT') {
      throw new Error(`Path does not exist: ${targetPath}`);
    }
    throw error;
  }
};

// Example usage
// extractElements('./src');

module.exports = {
  extractElements,
  processDirectory,
  extractElementsFromFile,
  generateReport
};