import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import { viteSingleFile } from "vite-plugin-singlefile";
import path from "path";

// Shared configuration
const sharedConfig = {
  plugins: [react(), viteSingleFile()],
  resolve: {
    alias: {
      '@': path.resolve(__dirname, './src'),
      'lib': path.resolve(__dirname, './src/lib')
    }
  },
  cacheDir: '.vite',
  optimizeDeps: {
    force: false,
    entries: ['./src/**/*.{js,jsx,ts,tsx}'],
    include: [], // Add specific dependencies you want to cache
    esbuildOptions: {
      // Remove cache options as they're not supported in context() call
    }
  },
  server: {
    force: false // Disable force dependency re-bundling
  },
  build: {
    target: 'esnext',
    minify: 'esbuild',
    cssCodeSplit: false,
    assetsInlineLimit: 10000000,
    emptyOutDir: true,
    outDir: '../UI/dist',
    manifest: true, // Enable manifest for cache busting
    rollupOptions: {
      output: {
        inlineDynamicImports: true,
        manualChunks: undefined,
        entryFileNames: '[name].[hash].js',
        chunkFileNames: '[name].[hash].js',
        assetFileNames: '[name].[hash].[ext]'
      }
    }
  },
  envPrefix: ['VITE_', 'APP_']
};

// Main build configuration
export default defineConfig(() => {
  return {
    ...sharedConfig,
    define: {
      'process.env.IS_SUBSET': JSON.stringify(false)
    },
    build: {
      ...sharedConfig.build,
      rollupOptions: {
        input: {
          index: 'index.html'
        }
      }
    }
  };
});
