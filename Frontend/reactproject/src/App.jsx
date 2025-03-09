import { useState } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'

function App() {
  const [count, setCount] = useState(0)
  const [result, setResult] = useState('Result will appear here')

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
      <div>
        <div style={{ textAlign: 'center' }}>
          <button id="sendClickButton" onClick={handleSendClick}>Send Click to Native</button>
          <div id="result">{result}</div>
        </div>
        <a href="https://vite.dev" target="_blank">
          <img src={viteLogo} className="logo" alt="Vite logo" />
        </a>
        <a href="https://react.dev" target="_blank">
          <img src={reactLogo} className="logo react" alt="React logo" />
        </a>
      </div>
      <h1>Vite + React</h1>
      <div className="card">
        <button onClick={() => setCount((count) => count + 1)}>
          count is {count}
        </button>
        <p>
          Edit <code>src/App.jsx</code> and save to test HMR
        </p>
      </div>
      <p className="read-the-docs">
        Click on the Vite and React logos to learn more
      </p>
    </>
  )
}

export default App
