import { useState, useEffect } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'
import { RadialGauge } from 'react-canvas-gauges'

function App() {
  const [result, setResult] = useState('Result')
  
  // State to track message rates and stats with proper initialization
  const [stats, setStats] = useState({
    string: { bytesReceived: 0, count: 0, rate: 0, lastTimestamp: null, smoothedRate: 0, firstTimestamp: null },
    json: { bytesReceived: 0, count: 0, rate: 0, lastTimestamp: null, smoothedRate: 0, firstTimestamp: null },
    nativeObject: { bytesReceived: 0, count: 0, rate: 0, lastTimestamp: null, smoothedRate: 0, firstTimestamp: null },
    sharedBuffer: { bytesReceived: 0, count: 0, rate: 0, lastTimestamp: null, smoothedRate: 0, firstTimestamp: null },
  });

  // Add state for tracking which senders are running
  const [runningSenders, setRunningSenders] = useState({
    string: false,
    json: false,
    nativeObject: false,
    sharedBuffer: false
  });

  const calculateRate = (stats, type, bytes) => {
    // Guard against invalid inputs
    if (!stats || !type || !stats[type] || bytes === undefined || bytes < 0) {
      console.warn('Invalid inputs to calculateRate', { stats, type, bytes });
      return 0;
    }

    const now = Date.now();
    const currentStats = stats[type];
    const dampingFactor = 0.8; // Increased from 0.5 to 0.8 - much higher = almost no damping
    
    currentStats.bytesReceived += bytes;
    currentStats.count = (currentStats.count || 0) + 1;
    
    // Initialize lastTimestamp if it doesn't exist
    if (!currentStats.lastTimestamp) {
      currentStats.lastTimestamp = now;
      return 0; // Return 0 for the first message to avoid misleading spikes
    }
    
    // Time difference in seconds with strict validation
    const timeDiff = (now - currentStats.lastTimestamp) / 1000;
    
    // Skip rate calculation if time difference is invalid or too small
    if (!timeDiff || timeDiff <= 0 || timeDiff < 0.001) { // Less than 1ms or invalid
      return currentStats.smoothedRate || 0;
    }
    
    // Calculate rate in Mb/s (bytes * 8 for bits / 1,000,000 for Mb)
    const instantRate = (bytes * 8) / (timeDiff * 1000000);
    
    // Validate the instant rate to prevent extreme values
    if (!Number.isFinite(instantRate) || instantRate < 0) {
      console.warn(`Invalid instant rate calculated: ${instantRate}, using previous value`);
      return currentStats.smoothedRate || 0;
    }
    
    // For very early measurements, be more cautious but still more responsive than before
    const effectiveDampingFactor = currentStats.count < 10 ? 0.5 : dampingFactor; // Increased from 0.3 to 0.5
    
    // Apply damping using exponential smoothing
    if (!currentStats.smoothedRate || currentStats.smoothedRate === 0 || !Number.isFinite(currentStats.smoothedRate)) {
      currentStats.smoothedRate = instantRate;
    } else {
      // Sanity check - less aggressive adjustment for outliers
      const ratio = instantRate / currentStats.smoothedRate;
      
      // Default to the effective damping factor
      var adjustedDampingFactor = effectiveDampingFactor;
      
      // More responsive adjustment for extreme outliers - but with higher base damping
      if (ratio > 20 || ratio < 0.05) {
        adjustedDampingFactor = 0.3; // Increased from 0.1 to 0.3
      }
      
      currentStats.smoothedRate = (adjustedDampingFactor * instantRate) + 
                                 ((1 - adjustedDampingFactor) * currentStats.smoothedRate);
    }
    
    // Update timestamp for next calculation
    currentStats.lastTimestamp = now;
    
    // Calculate the unadjusted value (no damping applied)
    const rawRateAverage = (currentStats.bytesReceived * 8) / 
                           ((now - currentStats.firstTimestamp || now) / 1000 * 1000000);
                           
    // Track first timestamp if not set
    if (!currentStats.firstTimestamp) {
      currentStats.firstTimestamp = now;
    }
    
    // For logging/debugging - track both smoothed and raw rates
    const smoothedRate = currentStats.smoothedRate;
    
    // Log every 50th message
    if (currentStats.count % 50 === 0) {
      console.log(`FRONTEND RATE: Type: ${type}, Count: ${currentStats.count}, ` +
                  `Size: ${bytes} bytes, Raw: ${instantRate.toFixed(2)} Mb/s, ` + 
                  `Smoothed: ${smoothedRate.toFixed(2)} Mb/s, ` +
                  `Overall Avg: ${rawRateAverage.toFixed(2)} Mb/s, ` +
                  `Damping: ${adjustedDampingFactor.toFixed(2)}`);
    }
    
    return smoothedRate;
  };

  // Function to send control messages to the backend
  function sendDataSenderControl(senderType, action) {
    if (window.chrome && window.chrome.webview) {
      const controlData = {
        type: "data-sender-control",
        senderType: senderType,
        action: action
      };
      
      console.log(`Sending control: ${senderType} ${action}`);
      
      // Log the full control data for debugging
      console.log('Control data being sent:', JSON.stringify(controlData, null, 2));
      
      // Send the control message to the backend
      window.chrome.webview.hostObjects.sync.nativeWindowControls.SendClick(JSON.stringify(controlData));
      
      // Update the local running state
      setRunningSenders(prev => ({
        ...prev,
        [senderType]: action === 'start'
      }));
      
      // Reset the stats for this sender type when stopping
      if (action === 'stop') {
        setStats(prev => ({
          ...prev,
          [senderType]: { 
            bytesReceived: 0, 
            count: 0, 
            rate: 0, 
            lastTimestamp: null, 
            smoothedRate: 0, 
            firstTimestamp: null 
          }
        }));
      }
    }
  }

  // Toggle sender function
  function toggleSender(senderType) {
    const isRunning = runningSenders[senderType];
    sendDataSenderControl(senderType, isRunning ? 'stop' : 'start');
  }

  useEffect(() => {
    // Set up web message event listener
    if (window.chrome && window.chrome.webview) {
      window.chrome.webview.addEventListener('message', event => {
        try {
          // Get the message data and size
          const message = event.data;
          let messageType = 'unknown'; 
          let dataSize = 0;
          
          // Determine message type and size
          if (typeof message === 'string') {
            try {
              // Try to parse as JSON first - a JSON message sent via PostWebMessageAsJson 
              // will arrive as a JavaScript object, not a string
              const parsed = false; //JSON.parse(message);
              if (parsed && typeof parsed === 'object' && parsed.type === 'json') {
                // This is actually JSON data
                messageType = 'json';
                dataSize = new Blob([message]).size;
                
                const rate = calculateRate(stats, messageType, dataSize);
                setStats(prevStats => ({ ...prevStats, json: { ...prevStats.json, smoothedRate: rate } }));
                
                // Debug output
                if (Math.random() < 0.01) {
                  console.log('JSON data detected in string message:', parsed);
                }
              } else {
                // It's a string that happens to be valid JSON but not our JSON format
                messageType = 'string';
                dataSize = new Blob([message]).size;
                
                const rate = calculateRate(stats, messageType, dataSize);
                setStats(prevStats => ({ ...prevStats, string: { ...prevStats.string, smoothedRate: rate } }));
              }
            } catch {
              // Not valid JSON, so it's a plain string
              messageType = 'string';
              dataSize = new Blob([message]).size;
              
              const rate = calculateRate(stats, messageType, dataSize);
              setStats(prevStats => ({ ...prevStats, string: { ...prevStats.string, smoothedRate: rate } }));
            }
          } 
          else if (typeof message === 'object') {
            // Try to identify the type from the object
            if (message.type === 'json') {
              messageType = 'json';
              dataSize = new Blob([JSON.stringify(message.data || message)]).size;
              
              const rate = calculateRate(stats, messageType, dataSize);
              setStats(prevStats => ({ ...prevStats, json: { ...prevStats.json, smoothedRate: rate } }));
            } 
            else if (message.type === 'nativeObject') {
              messageType = 'nativeObject';
              dataSize = new Blob([JSON.stringify(message.data || message)]).size;
              
              const rate = calculateRate(stats, messageType, dataSize);
              setStats(prevStats => ({ ...prevStats, nativeObject: { ...prevStats.nativeObject, smoothedRate: rate } }));
            } 
            else if (message.type === 'sharedBuffer') {
              messageType = 'sharedBuffer';
              // For shared buffer, message.data might be an ArrayBuffer
              if (message.data instanceof ArrayBuffer) {
                dataSize = message.data.byteLength;
              } else {
                dataSize = new Blob([JSON.stringify(message.data || message)]).size;
              }
              
              // Check if this is a dataready notification
              if (message.action === 'dataready') {
                console.log(`Shared buffer data ready notification: ${message.timestamp}, size: ${message.size} bytes`);
                
                // Update the shared buffer counter for the notification message
                // This ensures we count the buffer even if the actual buffer event is delayed
                setStats(prevStats => ({
                  ...prevStats,
                  sharedBuffer: {
                    ...prevStats.sharedBuffer,
                    count: prevStats.sharedBuffer.count + 1,
                    bytesReceived: prevStats.sharedBuffer.bytesReceived + (message.size || 0),
                    lastTimestamp: Date.now(),
                    firstTimestamp: prevStats.sharedBuffer.firstTimestamp || Date.now()
                  }
                }));
              }
              
              const rate = calculateRate(stats, messageType, dataSize);
              setStats(prevStats => ({ ...prevStats, sharedBuffer: { ...prevStats.sharedBuffer, smoothedRate: rate } }));
            }
            else {
              // Handle generic objects - assume string if we can't identify it
              messageType = 'string';
              dataSize = new Blob([JSON.stringify(message)]).size;
              
              const rate = calculateRate(stats, messageType, dataSize);
              setStats(prevStats => ({ ...prevStats, string: { ...prevStats.string, smoothedRate: rate } }));
            }
          }
          
          // Log some messages for debugging purposes (not every message to avoid spam)
          if (Math.random() < 0.01) { 
            console.log(`Received ${messageType} message, size: ${dataSize} bytes`);
          }
        } catch (error) {
          console.error('Error processing message:', error);
        }
      });
    }
  }, [stats]);

  useEffect(() => {
    if (window.chrome && window.chrome.webview) {
      // Add event listener for shared buffer
      window.chrome.webview.addEventListener('sharedbufferreceived', event => {
        try {
          // Log every shared buffer event for debugging
          console.log('Shared buffer event received!');
          
          // Get the buffer and metadata
          const buffer = event.getBuffer();
          let metadata = {};
          
          try {
            // Try to parse the metadata as JSON
            if (typeof event.additionalData === 'string') 
            {
              metadata = JSON.parse(event.additionalData);
            } 
            else if (typeof event.additionalData === 'object') 
            {
              // If it's already an object, use it directly
              metadata = event.additionalData;
            }
          } catch (parseError) 
          {
            console.warn('Error parsing metadata:', parseError);
            console.log('Raw metadata:', event.additionalData);
            // Continue with empty metadata object
          }
          
          console.log('Shared buffer metadata:', metadata);
          console.log('Buffer size:', buffer.byteLength);
          
          // Update stats for the shared buffer
          const dataSize = metadata.dataSize || buffer.byteLength;
          
          setStats(prevStats => 
          {
            const now = Date.now();
            const firstTimestamp = prevStats.sharedBuffer.firstTimestamp || now;
            const elapsed = (now - firstTimestamp) / 1000; // seconds
            
            // Calculate bytes per second
            const bytesReceived = prevStats.sharedBuffer.bytesReceived + dataSize;
            const bytesPerSecond = elapsed > 0 ? bytesReceived / elapsed : 0;
            const mbPerSecond = bytesPerSecond / (1024 * 1024);
            
            // Cap the rate for display purposes (0-100)
            const displayRate = Math.min(100, mbPerSecond * 10); // Scale MB/s for better visualization
            
            const newCount = prevStats.sharedBuffer.count + 1;
            
            // Log every 10th update to avoid console spam
            if (newCount % 10 === 0) {
              console.log(`Shared buffer stats: Count=${newCount}, Rate=${mbPerSecond.toFixed(2)} MB/s`);
            }
            
            return {
              ...prevStats,
              sharedBuffer: {
                ...prevStats.sharedBuffer,
                count: newCount,
                bytesReceived: bytesReceived,
                lastTimestamp: now,
                firstTimestamp: firstTimestamp,
                rate: mbPerSecond,
                smoothedRate: displayRate
              }
            };
          });
          
          // Process the buffer data
          if (buffer && buffer.byteLength > 0) 
          {
            // Create appropriate views for the buffer
            const floatView = new Float32Array(buffer);
            const uint64View = new BigUint64Array(buffer.slice(4 * 4 * 4096)); // Offset to timestamp array
            
            // Log some sample data (not every buffer to avoid console spam)
            if (Math.random() < 0.001) 
            { // Only log ~5% of buffers
              console.log(`Received shared buffer: ${buffer.byteLength} bytes`);
              console.log(`Buffer ID: ${metadata.bufferId}, Timestamp: ${metadata.timestamp}`);
              console.log(`Sample temperature: ${floatView[0]}`);
              console.log(`Sample pressure: ${floatView[4096]}`);
              console.log(`Sample humidity: ${floatView[2 * 4096]}`);
              console.log(`Sample voltage: ${floatView[3 * 4096]}`);
              console.log(`Sample timestamp: ${uint64View[0]}`);
            }
            
            // Here you would process the data further as needed
            // For example, update charts, display values, etc.
            
            // Send a pong response back to the backend to signal we're ready for more data
            // This implements flow control - backend won't send more until we process this buffer
            if (window.chrome && window.chrome.webview) {
              try {
                const pongMessage = {
                  type: "sharedBuffer",
                  action: "pong",
                  bufferId: parseInt(metadata.bufferId, 10), // Ensure bufferId is a number
                  timestamp: metadata.timestamp,
                  receivedAt: Date.now()
                };
                
                // Always log pong messages for debugging
                console.log(`Sending pong for buffer ${pongMessage.bufferId}`);
                
                const messageString = JSON.stringify(pongMessage);
                window.chrome.webview.postMessage(messageString);
              } catch (error) {
                console.error("Error sending pong message:", error);
              }
            }
          }
        } catch (error) {
          console.error('Error processing shared buffer:', error);
        }
      });
      
      // Also listen for dataready messages
      console.log('Set up shared buffer event listeners');
    }
  }, []);

  const handleSendClick = () => {
    try {
      // Create a dummy JSON string
      const dummyJson = JSON.stringify({
        action: "testClick",
        timestamp: new Date().toISOString(),
        data: {
          message: "Hello from WebView!",
          value: 42
        }
      });
      
      // Call the native function
      const result = window.chrome.webview.hostObjects.sync.nativeWindowControls.SendClick(dummyJson);
      
      // Display the result
      setResult("Result: " + JSON.stringify(result));
      
      console.log("SendClick called successfully", result);
    } catch (error) {
      setResult("Error: " + error.message);
      console.error("Error calling SendClick:", error);
    }
  };

  return (
    <>
      <div style={{ display: 'flex', justifyContent: 'space-between', width: '100%', marginBottom: '20px' }}>
        <div style={{ width: '24%' }}>
          <RadialGauge
            units='Mb/S'
            title='PostWebMessageAsString'
            value={isNaN(stats.string.smoothedRate) ? 0 : Math.min(Math.max(stats.string.smoothedRate, 0), 100)}
            minValue={0}
            maxValue={100}
            majorTicks={['0', '10', '20', '30', '40', '50', '60', '70', '80', '90', '100']}
            minorTicks={2}
            width={200}
            height={200}
          />
        </div>
        <div style={{ width: '24%' }}>
          <RadialGauge
            units='Mb/S'
            title='PostWebMessageAsJson'
            value={isNaN(stats.json.smoothedRate) ? 0 : Math.min(Math.max(stats.json.smoothedRate, 0), 100)}
            minValue={0}
            maxValue={100}
            majorTicks={['0', '10', '20', '30', '40', '50', '60', '70', '80', '90', '100']}
            minorTicks={2}
            width={200}
            height={200}
          />
        </div>
        <div style={{ width: '24%' }}>
          <RadialGauge
            units='Mb/S'
            title='NativeHostObject'
            value={isNaN(stats.nativeObject.smoothedRate) ? 0 : Math.min(Math.max(stats.nativeObject.smoothedRate, 0), 100)}
            minValue={0}
            maxValue={100}
            majorTicks={['0', '10', '20', '30', '40', '50', '60', '70', '80', '90', '100']}
            minorTicks={2}
            width={200}
            height={200}
          />
        </div>
        <div style={{ width: '24%' }}>
          <RadialGauge
            units='Mb/S'
            title='Shared Memory Buffer'
            value={isNaN(stats.sharedBuffer.smoothedRate) ? 0 : Math.min(Math.max(stats.sharedBuffer.smoothedRate, 0), 300)}
            minValue={0}
            maxValue={300}
            majorTicks={['0', '50', '100', '150', '200', '250', '300']}
            minorTicks={5}
            width={200}
            height={200}
          />
        </div>
      </div>
      <div>
        <div style={{ textAlign: 'center' }}>
          <button id="sendClickButton" onClick={handleSendClick}>Send Click to Native</button>
          <div id="result">{result}</div>
        </div>
        <div className="controls-container">
          <div className="control-panel">
            <h2>Data Sender Controls</h2>
            <div className="sender-controls">
              <div className="sender-control">
                <h3>String Messages</h3>
                <button 
                  onClick={() => toggleSender('string')}
                  className={runningSenders.string ? 'running' : ''}
                >
                  {runningSenders.string ? 'Stop' : 'Start'} String Sender
                </button>
                <div className="stats">Messages: {stats.string.count}</div>
              </div>
              
              <div className="sender-control">
                <h3>JSON Messages</h3>
                <button 
                  onClick={() => toggleSender('json')}
                  className={runningSenders.json ? 'running' : ''}
                >
                  {runningSenders.json ? 'Stop' : 'Start'} JSON Sender
                </button>
                <div className="stats">Messages: {stats.json.count}</div>
              </div>
              
              <div className="sender-control">
                <h3>Native Object</h3>
                <button 
                  onClick={() => toggleSender('nativeObject')}
                  className={runningSenders.nativeObject ? 'running' : ''}
                >
                  {runningSenders.nativeObject ? 'Stop' : 'Start'} Native Object
                </button>
                <div className="stats">Messages: {stats.nativeObject.count}</div>
              </div>
              
              <div className="sender-control">
                <h3>Shared Buffer</h3>
                <button 
                  onClick={() => toggleSender('sharedBuffer')}
                  className={runningSenders.sharedBuffer ? 'running' : ''}
                >
                  {runningSenders.sharedBuffer ? 'Stop' : 'Start'} Shared Buffer
                </button>
                <div className="stats">Messages: {stats.sharedBuffer.count}</div>
              </div>
            </div>
          </div>
        </div>
        <a href="https://vite.dev" target="_blank">
          <img src={viteLogo} className="logo" alt="Vite logo" />
        </a>
        <a href="https://react.dev" target="_blank">
          <img src={reactLogo} className="logo react" alt="React logo" />
        </a>
      </div>
    </>
  )
}

export default App
