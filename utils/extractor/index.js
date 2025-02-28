#!/usr/bin/env node
const extractor = require('./extractor');

// Parse command line arguments
const args = process.argv.slice(2);
let targetPath = './src';
let outputPath = './element-extraction-results.json';
let options = {
  backendOnly: false,
  includeClassNames: false,
  markdownOutput: false
};

// Check for help flag first
if (args.includes('--help') || args.includes('-h')) {
  showHelp();
  process.exit(0);
}

// Handle other command line arguments
args.forEach((arg, index) => {
  if (arg === '--backend-only' || arg === '-b') {
    options.backendOnly = true;
  } else if (arg === '--include-classnames' || arg === '-c') {
    options.includeClassNames = true;
  } else if (arg === '--markdown' || arg === '-m') {
    options.markdownOutput = true;
  } else if (arg === '--output' || arg === '-o') {
    outputPath = args[index + 1] || outputPath;
  } else if (!arg.startsWith('-') && index === 0) {
    targetPath = arg;
  }
});

function showHelp() {
  console.log(`
JSX Element Extractor
---------------------
Extracts UI elements from React component files.

Usage:
  jsx-extractor [path] [options]

Arguments:
  path                     Directory or file to scan (default: ./src)

Options:
  -o, --output [file]      Output file path (default: ./element-extraction-results.json)
  -b, --backend-only       Only extract elements with data-backend attribute
  -c, --include-classnames Include className attributes in the output (default: excluded)
  -m, --markdown           Output results as a Markdown file (default: JSON)
  -h, --help               Show this help message
`);
}

try {
  console.log(`Starting element extraction from: ${targetPath}`);
  console.log(`Results will be saved to: ${outputPath}`);

  const elements = extractor.extractElements(targetPath, outputPath, options);
  console.log(`Extraction complete! Found ${elements.length} UI elements.`);
} catch (error) {
  console.error('Error during extraction:', error.message);
  process.exit(1);
}