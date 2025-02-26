# Converting React App to Single-File Build

This guide describes how to convert a standard React application to build as a single HTML file, which includes all JavaScript, CSS, and assets inlined.

## Prerequisites

- An existing React application using Vite
- Node.js and npm installed

## Steps

### 1. Install Required Dependencies

```bash
npm install --save-dev vite-plugin-singlefile terser cross-env
```

### 2. Update Package Configuration

Modify your `package.json` to include the following build script:

```json
{
  "scripts": {
    "build": "npm install && cross-env NO_COLOR=1 vite build --minify"
  }
}
```

### 3. Configure Vite

Create or update your `vite.config.ts` with the following configuration:

```typescript
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import { viteSingleFile } from "vite-plugin-singlefile";

export default defineConfig({
  base: "",
  plugins: [
    react(),
    viteSingleFile()
  ],
  build: {
    target: 'esnext',
    minify: 'terser',
    cssCodeSplit: false,
    rollupOptions: {
      output: {
        inlineDynamicImports: true,
        manualChunks: undefined
      }
    }
  }
});
```

### 4. Simplify Application Structure

1. Remove unnecessary routing if not needed:
   ```typescript
   // src/App.tsx
   import React from 'react';
   import MainComponent from './components/MainComponent';

   function App() {
     return (
       <MainComponent />
     );
   }

   export default App;
   ```

2. Simplify the main entry point:
   ```typescript
   // src/main.tsx
   import React from "react";
   import ReactDOM from "react-dom/client";
   import App from "./App";
   import "./index.css";

   ReactDOM.createRoot(document.getElementById("root")!).render(
     <React.StrictMode>
       <App />
     </React.StrictMode>
   );
   ```

### 5. Update HTML Template

Ensure your `index.html` has a clean structure:

```html
<!doctype html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Your App Title</title>
  </head>
  <body>
    <div id="root"></div>
    <script type="module" src="/src/main.tsx"></script>
  </body>
</html>
```

## Building the Application

1. Run the build command:
   ```bash
   npm run build
   ```

2. Find the output in the `dist` directory:
   - A single `index.html` file containing all JavaScript, CSS, and assets inlined
   - The file size will be larger than usual since everything is bundled together

## Notes

- This approach works best for smaller applications
- All assets will be base64 encoded and inlined in the HTML
- The resulting file will be larger than a traditional build but requires no additional server setup
- Sourcemaps can still be generated but will be separate files
