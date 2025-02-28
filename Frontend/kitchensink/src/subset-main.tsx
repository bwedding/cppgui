import React from 'react';
import ReactDOM from 'react-dom/client';
import { Provider } from 'jotai';
import { useSignals } from '@preact/signals-react/runtime';
import { 
  initializeMessageHandling,
  sensorReadingsSignal,
  leftHeartSignal,
  rightHeartSignal,
  stopwatchSignal
} from './lib/subset-datastore';
import SubsetOperatingRoomView from './components/dashboard/SubsetOperatingRoomView'
import './index.css';

// Component to initialize message handling
const MessageHandler: React.FC = () => {
  React.useEffect(() => {
    // Initialize message handling with signals
    initializeMessageHandling({ useSignals: true });
  }, []);

  return null;
};

// Simple wrapper component
const SubsetApp: React.FC = () => {
  return (
    <div className="min-h-screen bg-neutral-800">
      <SubsetOperatingRoomView />
    </div>
  );
};

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <Provider>
      <MessageHandler />
      <SubsetApp />
    </Provider>
  </React.StrictMode>
);