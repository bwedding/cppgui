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
      window.chrome.webview.hostObjects.sync.nativeWindowControls.SendClick(JSON.stringify(controlData));
      
      // Update the local running state
      setRunningSenders(prev => ({
        ...prev,
        [senderType]: action === 'start'
      }));
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
          let messageType = 'string'; // Default type
          let dataSize = 0;
          
          // Determine message type and size
          if (typeof message === 'string') {
            messageType = 'string';
            dataSize = new Blob([message]).size;
            
            // Calculate rate and update state
            const rate = calculateRate(stats, messageType, dataSize);
            setStats(prevStats => ({ ...prevStats, string: { ...prevStats.string, smoothedRate: rate } }));
          } 
          else if (typeof message === 'object') {
            // Try to identify the type
            if (message.type === 'json') {
              messageType = 'json';
              dataSize = new Blob([JSON.stringify(message.data)]).size;
              
              const rate = calculateRate(stats, messageType, dataSize);
              setStats(prevStats => ({ ...prevStats, json: { ...prevStats.json, smoothedRate: rate } }));
            } 
            else if (message.type === 'nativeObject') {
              messageType = 'nativeObject';
              dataSize = new Blob([JSON.stringify(message.data)]).size;
              
              const rate = calculateRate(stats, messageType, dataSize);
              setStats(prevStats => ({ ...prevStats, nativeObject: { ...prevStats.nativeObject, smoothedRate: rate } }));
            } 
            else if (message.type === 'sharedBuffer') {
              messageType = 'sharedBuffer';
              // For shared buffer, message.data might be an ArrayBuffer
              if (message.data instanceof ArrayBuffer) {
                dataSize = message.data.byteLength;
              } else {
                dataSize = new Blob([JSON.stringify(message.data)]).size;
              }
              
              const rate = calculateRate(stats, messageType, dataSize);
              setStats(prevStats => ({ ...prevStats, sharedBuffer: { ...prevStats.sharedBuffer, smoothedRate: rate } }));
            }
            else {
              // Handle generic objects
              messageType = 'json'; // Default to JSON for objects
              dataSize = new Blob([JSON.stringify(message)]).size;
              
              const rate = calculateRate(stats, messageType, dataSize);
              setStats(prevStats => ({ ...prevStats, json: { ...prevStats.json, smoothedRate: rate } }));
            }
          }
          
          console.log(`Received ${messageType} of size ${dataSize} bytes, current rate: ${stats[messageType].smoothedRate.toFixed(2)} Mb/s`);
          
        } catch (error) {
          console.error('Error processing message:', error);
        }
      });
      
      console.log('Web message event listener registered');
    } else {
      console.warn('chrome.webview not available - running outside of WebView2?');
    }
    
    // Cleanup function
    return () => {
      if (window.chrome && window.chrome.webview) {
        // WebView2 doesn't currently support removeEventListener for message events
        // This is a placeholder for when it becomes available
      }
    };
  }, [stats]); // Add stats to the dependency array

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
      const result = window.chrome.webview.hostObjects.sync.native.SendClick(dummyJson);
      
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
            value={isNaN(stats.sharedBuffer.smoothedRate) ? 0 : Math.min(Math.max(stats.sharedBuffer.smoothedRate, 0), 100)}
            minValue={0}
            maxValue={100}
            majorTicks={['0', '10', '20', '30', '40', '50', '60', '70', '80', '90', '100']}
            minorTicks={2}
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
