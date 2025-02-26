import { defineConfig, UserConfig } from "vite";
import react from "@vitejs/plugin-react";
import { viteSingleFile } from "vite-plugin-singlefile";
import path from "path";

// Shared configuration that both builds will use
const sharedConfig: UserConfig = {
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
    outDir:'../UI/dist2',
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

// Configuration for different build modes
export default defineConfig(({ mode }) => {
  if (mode === 'subset') {
    return {
      ...sharedConfig,
      define: {
        'process.env.IS_SUBSET': JSON.stringify(true)
      },
      build: {
        ...sharedConfig.build,
        //outDir: 'D:/source/WebView2Samples/AnimalMonitor/App/Frontend/UI/dist-subset',
        outDir:'../UI/dist-subset',
        rollupOptions: {
          input: 'src/subset.html',
          output: {
            ...sharedConfig.build?.rollupOptions?.output,
            entryFileNames: 'or-view.[hash].js',
            assetFileNames: 'or-view.[hash][extname]',
            chunkFileNames: 'or-view.[hash].js'
          }
        }
      }
    };
  }
  
  // Main build configuration
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