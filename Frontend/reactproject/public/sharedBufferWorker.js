// sharedBufferWorker.js - Web Worker for processing shared buffer data
// This worker handles the CPU-intensive calculations for shared buffer processing

// Handle messages from the main thread
self.onmessage = function(event) {
  const { metadata, timestamp, sampleData, bufferSize } = event.data;
  
  try {
    // Process the metadata and sample data
    processData(metadata, timestamp, sampleData, bufferSize);
  } catch (error) {
    // Send error back to main thread
    self.postMessage({
      type: 'error',
      error: error.message,
      stack: error.stack
    });
  }
};

// Process the metadata and sample data
function processData(metadata, timestamp, sampleData, bufferSize) {
  // Log processing start for performance tracking
  const processingStart = performance.now();
  
  // Here you would perform any CPU-intensive calculations on the data
  // Since we no longer have the full buffer, we work with the sample data
  
  // Example: Perform some calculations or analysis on the sample data
  let analysisResults = {};
  
  if (sampleData) {
    // Calculate average of available values (just an example)
    const values = [
      sampleData.temperature,
      sampleData.pressure,
      sampleData.humidity,
      sampleData.voltage
    ].filter(val => val !== undefined);
    
    const average = values.length > 0 
      ? values.reduce((sum, val) => sum + val, 0) / values.length 
      : 0;
    
    analysisResults = {
      average,
      timestamp: sampleData.timestamp,
      processingTime: performance.now() - processingStart
    };
  }
  
  // Send the processed data back to the main thread
  self.postMessage({
    type: 'processed',
    bufferId: metadata.bufferId,
    timestamp: metadata.timestamp,
    receivedAt: timestamp,
    bufferSize: bufferSize,
    sampleData: sampleData,
    analysisResults: analysisResults,
    processingTime: performance.now() - processingStart
  });
}
